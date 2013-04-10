/** ===========================================================
 * @file
 *
 * This file is a part of digiKam project
 * <a href="http://www.digikam.org">http://www.digikam.org</a>
 *
 * @date    2010-03-03
 * @brief   openTLD interface.
 *
 * @author Copyright (C) 2012-2013 by Mahesh Hegde
 *         <a href="mailto:maheshmhegade at gmail dot com">maheshmhegade at gmail dot com</a>
 *
 * @section LICENSE
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

//Qt includes

#include <QList>

//KDE includes

#include <kdebug.h>

//local includes

#include "facerecognizer.h"
#include "libopencv.h"
#include "tlddatabase.h"
#include "tldrecognition.h"

namespace KFaceIface
{

class FaceRecognizer::Private
{
public:

    Private()
    {
        tldRecognitionCore = 0;
        db                 = 0;
        threshold          = 0.3;
    }

    ~Private()
    {
        delete db;
    }

public:

    Tlddatabase* database()
    {
        if (!db)
        {
            db = new Tlddatabase();
        }

        return db;
    }

    Tldrecognition* recognition() const
    {
        return (new Tldrecognition);
    }

public:

    float           threshold;

private:

    Tldrecognition* tldRecognitionCore;
    Tlddatabase*    db;
};

// -------------------------------------------------------------------------------

FaceRecognizer::FaceRecognizer()
    : d(new Private())
{
}

FaceRecognizer::~FaceRecognizer()
{
}

QList<float> FaceRecognizer::recognizeFaces(const QList<Face>& faces)
{
    QList<float> recognitionRate;

    foreach(Face face, faces)
    {
        vector<float> recognitionconfidence;

        IplImage* const img1           = face.image().toIplImage();
        IplImage* const inputfaceimage = cvCreateImage(cvSize(47,47),img1->depth,img1->nChannels);
        int count                      = -1;
        cvResize(img1, inputfaceimage);

        for (int i = 1; i <= d->database()->queryNumfacesinDatabase(); i++)
        {
            unitFaceModel* const comparemodel = d->database()->getFaceModel(i);
            recognitionconfidence.push_back(d->recognition()->getRecognitionConfidence(inputfaceimage,comparemodel));
            count++;
        }

        float maxConfidence = recognitionconfidence[0];

        if(count != -1)
        {
            int maxConfIndex = 0;

            for(int tmpInt = 0; tmpInt <= count; tmpInt++ )
            {
                if(recognitionconfidence[tmpInt] > maxConfidence)
                {
                    maxConfIndex  = tmpInt;
                    maxConfidence = recognitionconfidence[tmpInt];
                }
            }

            if(maxConfidence > d->threshold )
            {
                face.setName(d->database()->querybyFaceid(maxConfIndex+1));
                kDebug() << "person  " << qPrintable(d->database()->querybyFaceid(maxConfIndex+1));
            }
        }

        recognitionRate.append(maxConfidence);
    }

    return recognitionRate;
}

void FaceRecognizer::storeFaces(const QList<Face>& faces)
{
    foreach(Face face, faces)
    {
        IplImage* const img1                              = face.image().toIplImage();// tlddatabase->QImage2IplImage(face.image().toQImage());

        IplImage* const inputfaceimage                    = cvCreateImage(cvSize(47,47),img1->depth,img1->nChannels);
        cvResize(img1,inputfaceimage);

        KFaceIface::unitFaceModel* const facemodeltostore = d->recognition()->getModeltoStore(inputfaceimage);
        facemodeltostore->Name                            = face.name();
        facemodeltostore->faceid                          = face.id();

        d->database()->insertFaceModel(facemodeltostore);             //store facemodel in tlddatabase
    }
}

} // namespace KFaceIface

