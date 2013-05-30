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
 * @author Copyright (C) 2013 by Marcel Wiesweg
 *         <a href="mailto:marcel dot wiesweg at gmx dot de">marcel dot wiesweg at gmx dot de</a>
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

#ifndef KFACE_OPENTLDFACERECOGNIZER_H
#define KFACE_OPENTLDFACERECOGNIZER_H

// OpenCV includes

#include "libopencv.h"

// Qt include

#include <QImage>

// local includes

#include "unitfacemodel.h"

namespace KFaceIface
{

class DatabaseAccessData;

class OpenTLDFaceRecognizer
{

public:

    /**
     * @brief FaceRecognizer:Master class to control entire recognition using OpenTLD
     */
    OpenTLDFaceRecognizer(DatabaseAccessData*);
    ~OpenTLDFaceRecognizer();

    void setThreshold(float threshold) const;

    /** Returns a cvMat created from the inputImage, optimized for recognition */
    cv::Mat prepareForRecognition(const QImage& inputImage);
    /**
     *  Try to recognize the given image.
     *  Returns the identity id.
     *  If the identity cannot be recognized, returns -1.
     */
    int recognize(const cv::Mat& inputImage);

    /**
     * Trains the given image, which represents a face of the given identity.
     */
    void train(int identity, const cv::Mat& inputImage, const QString& context);

    //void   setThreshold(float value);

private:

    float recognitionConfidence(const cv::Mat& im, const UnitFaceModel& compareModel);
    void createFaceModel(const cv::Mat& im, UnitFaceModel& model);

    class Private;
    Private* const d;
};

} // namespace KFaceIface

#endif // KFACE_FACERECOGNIZER_H
