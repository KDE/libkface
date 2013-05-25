/** ===========================================================
 * @file
 *
 * This file is a part of digiKam project
 * <a href="http://www.digikam.org">http://www.digikam.org</a>
 *
 * @date   2010-06-16
 * @brief  The Database class wraps the libface database
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

QStringList toPaths(char** argv, int startIndex, int argc)
{
    QStringList files;
    for (int i=startIndex; i<argc; i++)
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

class SimpleTrainingDataProvider : public TrainingDataProvider
{
public:

    SimpleTrainingDataProvider(const Identity& identity, const QList<QImage>& newImages)
        : identity(identity), toTrain(newImages)
    {
    }

    ImageListProvider* newImages(const Identity& id)
    {
        if (identity == id)
        {
            return &toTrain;
        }
        return &empty;
    }
    ImageListProvider* images(const Identity&)
    {
        return &empty;
    }

    Identity identity;
    QListImageListProvider toTrain;
    QListImageListProvider empty;
};


int main(int argc, char** argv)
{
    if (argc < 2 || (argv[1] == QLatin1String("train") && argc < 3))
    {
        kDebug() << "Bad Args!!!\nUsage: " << argv[0] << " identify <image1> <image2> ... |train name <image1> <image2> ... "
                                                         "|orl <path to orl_faces>";
        return 0;
    }

    QCoreApplication app(argc, argv);

    RecognitionDatabase db = RecognitionDatabase::addDatabase(QDir::currentPath());

    if (argv[1] == QLatin1String("identify"))
    {
        QStringList paths = toPaths(argv, 2, argc);
        QList<QImage> images = toImages(paths);

        QTime time;
        time.start();
        QList<Identity> identities = db.recognizeFaces(images);
        int elapsed = time.elapsed();

        qDebug() << "Recognition took" << elapsed << "for" << images.size() << "," << ((float)elapsed/images.size()) << "per image";
        for (int i = 0; i<paths.size(); i++)
        {
            qDebug() << "Identified" << identities[i].attributes.value("name") << "in" << paths[i];
        }
    }
    else if (argv[1] == QLatin1String("train"))
    {
        QString name = QString::fromLocal8Bit(argv[2]);
        qDebug()  << "Training" << name;

        QStringList paths = toPaths(argv, 3, argc);
        QList<QImage> images = toImages(paths);

        Identity identity = db.findIdentity("name", name);
        if (identity.isNull())
        {
            qDebug() << "Adding new identity to database for name" << name;
            QMap<QString, QString> attributes;
            attributes["name"] = name;
            identity = db.addIdentity(attributes);
        }

        QTime time;
        time.start();

        SimpleTrainingDataProvider data(identity, images);
        db.train(identity, &data, "test application");

        int elapsed = time.elapsed();
        qDebug() << "Training took" << elapsed << "for" << images.size() << "," << ((float)elapsed/images.size()) << "per image";
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
            qDebug() << "Cannot find orl_faces directory";
            return 0;
        }

        QMap<int, QStringList> trainingImages, recognitionImages;
        QMap<int, Identity> idMap;
        for (int i=1; i<=40; i++)
        {
            for (int j=1; j<=10; j++)
            {
                QString path = orlDir.path() + QString("/s%1/%2.pgm").arg(i).arg(j);
                if (j<=5)
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
            qDebug() << "Could not find files of ORL database";
            return 0;
        }

        QTime time;
        time.start();

        int correct = 0, notRecognized = 0, falsePositive = 0;

        for (QMap<int, QStringList>::const_iterator it = trainingImages.begin(); it != trainingImages.end(); ++it)
        {
            Identity identity = db.addIdentity(QMap<QString, QString>());
            idMap[it.key()] = identity;
            qDebug() << "Mapped person" << it.key() << "to identity" << identity.id;

            QList<QImage> images = toImages(it.value());
            SimpleTrainingDataProvider data(identity, images);
            db.train(identity, &data, "test application");
        }
        int elapsed = time.restart();
        qDebug() << "Training 5/10 or ORL took" << elapsed << "," << ((float)elapsed/200) << "per image";

        for (QMap<int, QStringList>::const_iterator it = trainingImages.begin(); it != trainingImages.end(); ++it)
        {
            Identity identity = idMap.value(it.key());
            QList<QImage> images = toImages(it.value());
            QList<Identity> results = db.recognizeFaces(images);
            qDebug() << "Result for" << it.value().first() << "is identity" << results.first().id;
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
        }
        qDebug() << "Recognition of 5/10 or ORL took" << elapsed << "," << ((float)elapsed/200) << "per image";

        qDebug() << correct << "of 200 (" << (float(correct)/2) << "%) were correctly recognized";
        qDebug() << falsePositive << "of 200 (" << (float(falsePositive)/2) << "%) were falsely assigned to an identity";
        qDebug() << notRecognized << "of 200 (" << (float(notRecognized)/2) << "%) were not recognized";
    }

    return 0;
}
