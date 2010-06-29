/** ===========================================================
 *
 * This file is a part of digiKam project
 * <a href="http://www.digikam.org">http://www.digikam.org</a>
 *
 * @date   2010-06-16
 * @brief  The Database class wraps the libface database
 *
 * @author Copyright (C) 2010 by Marcel Wiesweg
 *         <a href="mailto:marcel dot wiesweg at gmx dot de">marcel dot wiesweg at gmx dot de</a>
 *         Copyright (C) 2010 by Aditya Bhatt
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

#include "database.h"

// Qt includes

#include <QMutex>
#include <QMutexLocker>
#include <QSharedData>
#include <QDebug>

// Libface includes

#include <libface/LibFace.h>
#include <libface/Face.h>

// Local includes

#include "kfaceutils.h"
#include "image_p.h"
#include <QFile>

namespace KFaceIface
{

const QString mappingFilename = QString("/dictionary");

class DatabasePriv : public QSharedData
{
public:

    DatabasePriv()
    {
        libface = 0;
    }

    ~DatabasePriv()
    {
        delete libface;
    }

    libface::LibFace*   libface;
    QHash<QString, int> hash;
    QString             configPath;
};

Database::Database(InitFlags flags, const QString& configurationPath)
        : d(new DatabasePriv)
{
    d->configPath = configurationPath;
    d->hash = KFaceUtils::hashFromFile(d->configPath+mappingFilename);

    if (flags == InitDetection)
    {
        d->libface = new libface::LibFace(libface::DETECT);
    }
    else
    {
        libface::Mode mode;
        if (flags == InitAll)
            mode = libface::ALL;
        else
            mode = libface::EIGEN;

        d->libface = new libface::LibFace(mode, d->configPath.toStdString());
    }
}

Database::Database(const Database& other)
{
    d = other.d;
}

Database& Database::operator=(const KFaceIface::Database& other)
{
    d = other.d;
    return *this;
}

Database::~Database()
{
}

QList<Face> Database::detectFaces(const Image& image)
{
    IplImage* img = image.imageData();

    std::vector<libface::Face> result = d->libface->detectFaces(img);

    QList<Face> faceList;
    foreach(const libface::Face& f, result)
    {
        faceList << Face(f);
    }
    return faceList;
}

bool Database::updateFaces(QList<Face>& faces)
{
    if(faces.isEmpty())
        return false;
    std::vector<libface::Face> faceVec;
    foreach (Face face, faces)
    {
        // If a name is already there in the dictionary, then set the ID from the dictionary, so that libface won't set it's own ID
        if(d->hash.contains(face.name()))
        {
            face.setId(d->hash[face.name()]);
        }
        faceVec.push_back(face);
    }

    std::vector<int> ids;
    ids = d->libface->update(&faceVec);

    for(int i = 0; i<(int)ids.size(); ++i)
    {
            faces[i].setId(ids.at(i));

            // If the name was not in the mapping before (new name), add it to the dictionary
            if(!d->hash.contains(faces[i].name()))
            {
                // Add to file
                KFaceUtils::addNameToFile(d->configPath+mappingFilename, faces[i].name(), faces[i].id());
                // Add to d->hash
                d->hash[faces[i].name()] = faces[i].id();
            }
    }
    
    return true;
}

QList<double> Database::recognizeFaces(QList<Face>& faces)
{
    QList<double> closeness;
    if(faces.isEmpty())
    {
        return closeness;
    }

    if(!QFile::exists(d->configPath+mappingFilename))
    {
        qDebug()<<"ERROR: No database exists.";
        return closeness;
    }

    std::vector<libface::Face> faceVec;
    foreach (const Face& face, faces)
    {
        faceVec.push_back(face);
    }

    std::vector< std::pair<int, double> > result;

    result = d->libface->recognise(&faceVec);

    for(int i = 0; i <faces.size() && i<(int)result.size(); ++i)
    {
        faces[i].setId(result.at(i).first);
        closeness.append(result.at(i).second);

        // Locate the name from the hash, pity we don't have a bi-directional hash in Qt
        QHashIterator<QString, int> it(d->hash);
        it.toFront();
        while(it.hasNext())
        {
            it.next();
            if(it.value() == faces[i].id())
            {
                faces[i].setName(it.key());
                break;
            }
        }
    }
    return closeness;
}

void Database::saveConfig()
{
    std::string path = d->configPath.toStdString();
    d->libface->saveConfig(path);
}

QString Database::configPath()
{
    return d->configPath;
}

void Database::setDetectionAccuracy(int value)
{
    d->libface->setDetectionAccuracy(value);
}

int Database::detectionAccuracy()
{
    return d->libface->getDetectionAccuracy();
}

}; // namespace KFaceIface
