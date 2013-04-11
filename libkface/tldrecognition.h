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

#ifndef TLD_RECOGNITION_H
#define TLD_RECOGNITION_H

// Qt includes

#include <QtGui/QImage>

// OpenTLD includes

#include "tldface.h"
#include "libopencv.h"
#include "libkface_export.h"

namespace KFaceIface
{

class KFACE_EXPORT Tldrecognition
{
public:

    /**
     * @brief Tldrecognition
     *This is convinient class to interface OpenTLD core recognition modules
     */
    Tldrecognition();
   ~Tldrecognition();

    /**
     * @brief getModeltoStore
     * @param is face IplImage* typically coloured face image as arguments to get facemodel obtained from initial learning
     * performed by OpenTLD core recognition
     * @return facemodel containing all features and face details in the form of OpenTLD compatible
     * mathematical features.
     */
    unitFaceModel* getModeltoStore(IplImage* const) const;

    int updateDatabase(IplImage* const, const char* const) const;

    /**
     * @brief getRecognitionConfidence
     * @param comparemodel are image to be recognized and mathematical model
     * typically models retrieced from database(confirmed face entry).
     * @return recognition confidence as float number (1(confident) through 0(not confident))
     */
    float getRecognitionConfidence(IplImage* const, unitFaceModel* const comparemodel) const;

private:

    class Private;
    Private* const d;
};

} // namespace KFaceIface

#endif // TLD_RECOGNITION_H
