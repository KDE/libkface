/** ===========================================================
 * @file
 *
 * This file is a part of digiKam project
 * <a href="http://www.digikam.org">http://www.digikam.org</a>
 *
 * @date    2010-03-03
 * @brief   openTLD interface.
 * @section DESCRIPTION
 *
 * Face detection through openTLD interface.
 *
 * @author Copyright (C) 2012-2013 by Mahesh Hegde
 *         <a href="mailto:maheshmhegade at gmail dot com">maheshmhegade at gmail dot com</a>
 * @author Copyright (C) 2013 by Gilles Caulier
 *         <a href="mailto:caulier dot gilles at gmail dot com">caulier dot gilles at gmail dot com</a>
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

#include "tldrecognition.h"
#include "Main.h"

using namespace std;

namespace KFaceIface
{

class Tldrecognition::Private
{

public:

    Private()
    {
        main  = 0;
    }

    Main* main;
};

Tldrecognition::Tldrecognition()
    : d(new Private)
{
    d->main = new Main();
    srand(d->main->seed);
}

Tldrecognition::~Tldrecognition()
{
//    delete d->main;
    delete d;
}

void Tldrecognition::getModeltoStore(IplImage* const inputImage, unitFaceModel *facemodeltostore) const
{
    d->main->generatefacemodel(inputImage,facemodeltostore);
}

float Tldrecognition::getRecognitionConfidence(IplImage* const inputImage, unitFaceModel* const comparemodel) const
{
    return d->main->getrecognitionconfidence(inputImage, comparemodel);
}

int Tldrecognition::updateDatabase(IplImage* const /*inputImage*/, const char* const /*faceModelFilename*/) const
{
//    d->main->doWork(inputImage, faceModelFilename, 3);
    return 0;
}

} // namespace KFaceIface
