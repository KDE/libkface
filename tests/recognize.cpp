/** ===========================================================
 * @file
 *
 * This file is a part of digiKam project
 * <a href="http://www.digikam.org">http://www.digikam.org</a>
 *
 * @date   2010-06-16
 * @brief  Face Recognition CLI tool
 *         NOTE: This tool is able to use ORL database which are
 *         freely available set of images to test face recognition.
 *         It contain 10 photos of 20 different peoples from slightly
 *         different angles. See here for details:
 *         http://www.cl.cam.ac.uk/research/dtg/attarchive/facedatabase.html
 *
 * @author Copyright (C) 2010 by Aditya Bhatt
 *         <a href="mailto:adityabhatt1991 at gmail dot com">adityabhatt1991 at gmail dot com</a>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

// Qt includes

#include <QApplication>
#include <QDir>
#include <QImage>
#include <QTime>

// KDE includes

#include <kdebug.h>

// libkface includes

#include "libkface/recognitiondatabase.h"

using namespace KFaceIface;

// --------------------------------------------------------------------------------------------------

QStringList toPaths(char** argv, int startIndex, int argc)
{
    QStringList files;

    for (int i = startIndex ; i < argc ; i++)
    {
        files << QString::fromLocal8Bit(argv[i]);
    }

    return files;
}

QList<QImage> toImages(const QStringList& paths)
{
    QList<QImage> images;

    foreach (const QString& path, paths)
    {
        images << QImage(path);
    }

    return images;
}

// --------------------------------------------------------------------------------------------------

int main(int argc, char** argv)
{
    if (argc < 2 || (argv[1] == QLatin1String("train") && argc < 3))
    {
        kDebug() << "Bad Arguments!!!\nUsage: " << argv[0] << " identify <image1> <image2> ... | train name <image1> <image2> ... "
                                                              "| ORL <path to orl_faces>";
        return 0;
    }

    QCoreApplication app(argc, argv);

    RecognitionDatabase db = RecognitionDatabase::addDatabase(QDir::currentPath());

    if (argv[1] == QLatin1String("identify"))
    {
        QStringList paths    = toPaths(argv, 2, argc);
        QList<QImage> images = toImages(paths);

        QTime time;
        time.start();
        QList<Identity> identities = db.recognizeFaces(images);
        int elapsed                = time.elapsed();

        kDebug() << "Recognition took " << elapsed << " for " << images.size() << ", " << ((float)elapsed/images.size()) << " per image";

        for (int i = 0 ; i < paths.size() ; i++)
        {
            kDebug() << "Identified " << identities[i].attribute("name") << " in " << paths[i];
        }
    }
    else if (argv[1] == QLatin1String("train"))
    {
        QString name = QString::fromLocal8Bit(argv[2]);
        kDebug() << "Training " << name;

        QStringList paths    = toPaths(argv, 3, argc);
        QList<QImage> images = toImages(paths);
        Identity identity    = db.findIdentity("name", name);

        if (identity.isNull())
        {
            kDebug() << "Adding new identity to database for name " << name;
            QMap<QString, QString> attributes;
            attributes["name"] = name;
            identity           = db.addIdentity(attributes);
        }

        QTime time;
        time.start();

        db.train(identity, images, "test application");

        int elapsed = time.elapsed();
        kDebug() << "Training took " << elapsed << " for " << images.size() << ", " << ((float)elapsed/images.size()) << " per image";
    }
    else if (argv[1] == QLatin1String("orl"))
    {
        QString orlPath = QString::fromLocal8Bit(argv[2]);

        if (orlPath.isEmpty())
        {
            orlPath = "orl_faces"; // relative to current dir
        }

        QDir orlDir(orlPath);

        if (!orlDir.exists())
        {
            kDebug() << "Cannot find orl_faces directory";
            return 0;
        }

        const int OrlIdentities       = 40;
        const int OrlSamples          = 10;
        const QString trainingContext = "test application";

        QMap<int, Identity> idMap;
        QList<Identity> trainingToBeCleared;

        for (int i = 1 ; i <= OrlIdentities ; i++)
        {
            QMap<QString, QString> attributes;
            attributes["name"] = QString::number(i);
            Identity identity  = db.findIdentity(attributes);

            if (identity.isNull())
            {
                Identity identity = db.addIdentity(attributes);
                idMap[i]          = identity;
                kDebug() << "Created identity " << identity.id() << " for ORL directory " << i;
            }
            else
            {
                kDebug() << "Already have identity for ORL directory " << i << ", clearing training data";
                idMap[i] = identity;
                trainingToBeCleared << identity;
            }
        }

        db.clearTraining(trainingToBeCleared, trainingContext);
        QMap<int, QStringList> trainingImages, recognitionImages;

        for (int i=1 ; i <= OrlIdentities ; i++)
        {
            for (int j = 1 ; j <= OrlSamples ; j++)
            {
                QString path = orlDir.path() + QString("/s%1/%2.pgm").arg(i).arg(j);

                if (j <= OrlSamples/2)
                {
                    trainingImages[i] << path;
                }
                else
                {
                    recognitionImages[i] << path;
                }
            }
        }

        if (!QFileInfo(trainingImages.value(1).first()).exists())
        {
            kDebug() << "Could not find files of ORL database";
            return 0;
        }

        // close previous db
        RecognitionDatabase();

        // reload db
        db = RecognitionDatabase::addDatabase(QDir::currentPath());

        QTime time;
        time.start();

        int correct = 0, notRecognized = 0, falsePositive = 0, totalTrained = 0, totalRecognized = 0, elapsed = 0;

        for (QMap<int, QStringList>::const_iterator it = trainingImages.constBegin() ; it != trainingImages.constEnd() ; ++it)
        {
            Identity identity = db.findIdentity("name", QString::number(it.key()));

            if (identity.isNull())
            {
                kDebug() << "Identity management failed for ORL person " << it.key();
            }

            QList<QImage> images = toImages(it.value());
            kDebug() << "Training ORL directory " << it.key();
            db.train(identity, images, trainingContext);
            totalTrained        += images.size();
        }

        elapsed = time.restart();

        if (totalTrained)
        {
            kDebug() << "Training 5/10 or ORL took " << elapsed << " ms, " << ((float)elapsed/totalTrained) << " ms per image";
        }

        // reload db
        db      = RecognitionDatabase();
        db      = RecognitionDatabase::addDatabase(QDir::currentPath());
        elapsed = time.restart();
        kDebug() << "Reloading database (probably from disk cache) took " << elapsed << " ms";

        for (QMap<int, QStringList>::const_iterator it = recognitionImages.constBegin() ; it != recognitionImages.constEnd() ; ++it)
        {
            Identity identity       = idMap.value(it.key());
            QList<QImage> images    = toImages(it.value());
            QList<Identity> results = db.recognizeFaces(images);

            kDebug() << "Result for " << it.value().first() << " is identity " << results.first().id();

            foreach (const Identity& foundId, results)
            {
                if (foundId.isNull())
                {
                    notRecognized++;
                }
                else if (foundId == identity)
                {
                    correct++;
                }
                else
                {
                    falsePositive++;
                }
            }

            totalRecognized += images.size();
        }

        elapsed = time.elapsed();

        if (totalRecognized)
        {
            kDebug() << "Recognition of 5/10 or ORL took " << elapsed << " ms, " << ((float)elapsed/totalRecognized) << " ms per image";
            kDebug() << correct       << " of 200 (" << (float(correct)       / totalRecognized*100) << "%) were correctly recognized";
            kDebug() << falsePositive << " of 200 (" << (float(falsePositive) / totalRecognized*100) << "%) were falsely assigned to an identity";
            kDebug() << notRecognized << " of 200 (" << (float(notRecognized) / totalRecognized*100) << "%) were not recognized";
        }
        else
        {
            kDebug() << "No face recognized";
        }
    }

    return 0;
}
