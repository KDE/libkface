/** ===========================================================
 *
 * This file is a part of digiKam project
 * <a href="http://www.digikam.org">http://www.digikam.org</a>
 *
 * @date  2010-09-02
 * @brief A convenience class for a standalone face detector
 *
 * @author Copyright (C) 2010 by Marcel Wiesweg
 *         <a href="mailto:marcel dot wiesweg at gmx dot de">marcel dot wiesweg at gmx dot de</a>
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

#include "facedetector.h"

// Qt includes

#include <QSharedData>

// Local includes

#include "database.h"

namespace KFaceIface
{

class FaceDetector::FaceDetectorPriv : public QSharedData
{
public:

    FaceDetectorPriv()
    {
        db                = 0;
        detectionAccuracy = -1;
    }

    ~FaceDetectorPriv()
    {
        delete db;
    }

    int detectionAccuracy;

public:

    Database* database()
    {
        if (!db)
        {
            db = new Database(Database::InitDetection);
            if (detectionAccuracy != -1)
                db->setDetectionAccuracy(detectionAccuracy);
        }
        return db;
    }

    const Database* constDatabase() const
    {
        return db;
    }

private:

    Database* db;
};

FaceDetector::FaceDetector()
            : d(new FaceDetectorPriv)
{
}

FaceDetector::FaceDetector(const FaceDetector& other)
{
    d = other.d;
}

FaceDetector& FaceDetector::operator=(const KFaceIface::FaceDetector& other)
{
    d = other.d;
    return *this;
}

FaceDetector::~FaceDetector()
{
}

QList<Face> FaceDetector::detectFaces(const Image& image)
{
    return d->database()->detectFaces(image);
}

void FaceDetector::setAccuracy(int value)
{
    // deferred creation
    if (d->constDatabase())
        d->database()->setDetectionAccuracy(value);
    else
        d->detectionAccuracy = value;
}

int FaceDetector::accuracy() const
{
    if (d->constDatabase())
        return d->constDatabase()->detectionAccuracy();
    else
        return 3;
}

} // namespace KFaceIface
