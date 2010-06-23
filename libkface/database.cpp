/** ===========================================================
 *
 * This file is a part of digiKam project
 * <a href="http://www.digikam.org">http://www.digikam.org</a>
 *
 * @date  : 2010-06-16
 * @brief : The Database class wraps the libface database
 *
 * @author: Copyright (C) 2010 by Marcel Wiesweg
 *          <a href="mailto:marcel dot wiesweg at gmx dot de">marcel dot wiesweg at gmx dot de</a>
 *          Copyright (C) 2010 by Aditya Bhatt
 *          <a href="mailto:adityabhatt1991 at gmail dot com">adityabhatt1991 at gmail dot com</a>
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

// Libface includes

#include <libface/LibFace.h>
#include <libface/Face.h>

// Local includes

#include "kfaceutils.h"
#include "image_p.h"

namespace KFace
{

class DatabaseStatic
{
public:

    DatabaseStatic()
    {
    }

    QMutex  mutex;
    //QHash<QString, Database> hash;
};

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

    libface::LibFace* libface;
    QString           configPath;
};

Database::Database(InitFlags flags, const QString& configurationPath)
        : d(new DatabasePriv)
{
    d->configPath = configurationPath;
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

        d->libface = new libface::LibFace(mode, configurationPath.toStdString());
    }
}

Database::Database(const Database& other)
{
    d = other.d;
}

Database& Database::operator=(const KFace::Database& other)
{
    d = other.d;
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

void Database::updateFaces(QList<Face>& faces)
{
    std::vector<libface::Face> faceVec;
    foreach (const Face& face, faces)
    {
        faceVec.push_back(face);
    }

    std::vector<int> ids;
    ids = d->libface->update(&faceVec);

    for(int i = 0; i<faces.size() && i<(int)ids.size(); ++i)
    {
        faces[i].setId(ids.at(i));
    }
}

QList<double> Database::recognizeFaces(QList<Face>& faces)
{
    std::vector<libface::Face> faceVec;
    foreach (const Face& face, faces)
    {
        faceVec.push_back(face);
    }

    QList<double> closeness;
    std::vector< std::pair<int, double> > result;

    result = d->libface->recognise(&faceVec);

    for(int i = 0; i <faces.size() && i<(int)result.size(); ++i)
    {
        faces[i].setId(result.at(i).first);
        closeness.append(result.at(i).second);
    }

    return closeness;
}

void Database::saveConfig()
{
    d->libface->saveConfig(d->configPath.toStdString());
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


}; // namespace KFace
