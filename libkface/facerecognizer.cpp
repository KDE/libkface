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

// Qt includes

#include <QList>

// KDE includes

#include <kdebug.h>

// local includes

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
        threshold          = 0.3;
        faceModelToStore   = 0;
        tldRecognitionCore = 0;
        db                 = 0;
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

    Tldrecognition* recognition()
    {
        tldRecognitionCore = new Tldrecognition();
        return tldRecognitionCore;
    }

public:

    float           threshold;
    unitFaceModel*  faceModelToStore;

private:

    Tldrecognition* tldRecognitionCore;
    Tlddatabase*    db;
};

FaceRecognizer::FaceRecognizer()
    : d(new Private())
{
}

FaceRecognizer::~FaceRecognizer()
{
    delete d;
}

QList<float> FaceRecognizer::recognizeFaces(QList<Face>& faces) const
{
    QList<float> recognitionRate;

    for(int faceindex = 0; faceindex < faces.size(); faceindex++)
    {
        vector<float> recognitionconfidence;

        IplImage* const img1           = d->database()->QImage2IplImage(faces[faceindex].image().toColorQImage());
        IplImage* const inputfaceimage = cvCreateImage(cvSize(47, 47), img1->depth, img1->nChannels);

        try
        {
            if(img1->height > 0)
            {
                cvResize(img1, inputfaceimage);
            }
            else
            {
                break;
            }

            int count = -1;

            for (int i = 1; i <= d->database()->queryNumfacesinDatabase();i++ )
            {
                unitFaceModel* const comparemodel = d->database()->getFaceModel(i);
                recognitionconfidence.push_back(d->recognition()->getRecognitionConfidence(inputfaceimage, comparemodel));
                count++;
                kDebug() << "............................." << count;
            }

            if(d->database()->queryNumfacesinDatabase() == 0)
            {
                return recognitionRate;
            }

            float maxConfidence = recognitionconfidence[0];

            if(count != -1)
            {
                int maxConfIndex = 0;

                for(int tmpInt = 0; tmpInt <= count ; tmpInt++ )
                {
                    if(recognitionconfidence[tmpInt] > maxConfidence)
                    {
                        maxConfIndex  = tmpInt;
                        maxConfidence = recognitionconfidence[tmpInt];
                    }
                }

                faces[faceindex].setName(d->database()->querybyFaceid(maxConfIndex+1));
                faces[faceindex].setId(d->database()->queryFaceID(maxConfIndex+1));
                kDebug() << "preson  " << qPrintable(d->database()->querybyFaceid(maxConfIndex+1)) << "recognized";
            }

            recognitionRate.append(maxConfidence);
            kDebug() << maxConfidence ;
        }
        catch (cv::Exception& e)
        {
            kError() << "cv::Exception:" << e.what();
        }
        catch(...)
        {
            kDebug() << "cv::Exception";
        }
    }

    return recognitionRate;
}

void FaceRecognizer::storeFaces(const QList<Face>& faces)
{
    foreach(Face face, faces)
    {
        IplImage* const img1            = d->database()->QImage2IplImage(face.image().toColorQImage());
        IplImage* const inputfaceimage  = cvCreateImage(cvSize(47,47),img1->depth,img1->nChannels);
        cvResize(img1,inputfaceimage);

        unitFaceModel* faceModelToStore = new unitFaceModel;
        d->recognition()->getModeltoStore(inputfaceimage,faceModelToStore);
        faceModelToStore->Name          = face.name();
        faceModelToStore->faceid        = face.id();

        kDebug() << face.name() << face.id() ;

        d->database()->insertFaceModel(faceModelToStore);             //store facemodel in tlddatabase
        delete faceModelToStore;
    }
}

} // namespace KFaceIface

