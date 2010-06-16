/**
* This file is part of libkface.
*
* libkface is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 2 of the License, or
* (at your option) any later version.
*
* libkface is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with libkface.  If not, see <http://www.gnu.org/licenses/>.
*
* @note: The Database class wraps the libface database
* @author: Aditya Bhatt, Marcel Wiesweg
*/

#include "database.h"

#include <QMutex>
#include <QMutexLocker>
#include <QSharedData>

#include <libface/LibFace.h>
#include <libface/Face.h>

#include "kfaceutils.h"

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

    libface::LibFace *libface;
    QString           configPath;
};

Database::Database(InitFlags flags, const QString& configurationPath)
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

Database::~Database()
{
}

QList<Face> Database::detectFaces(const QImage& image)
{
    IplImage *img = KFaceUtils::QImage2IplImage(image);
    std::vector<libface::Face> result = d->libface->detectFaces(img->imageData, img->width, img->height,
                                                                img->widthStep, img->depth, img->nChannels);

    QList<Face> faceList;
    foreach(const libface::Face &f, result)
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

};
