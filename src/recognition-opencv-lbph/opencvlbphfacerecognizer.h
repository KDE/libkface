/** ===========================================================
 * @file
 *
 * This file is a part of KDE project
 *
 *
 * @date   2010-03-03
 * @brief  LBPH Recognizer.
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

#ifndef OPENCV_LBPH_FACE_RECOGNIZER_H
#define OPENCV_LBPH_FACE_RECOGNIZER_H

// OpenCV library

#include "libopencv.h"

// Qt include

#include <QImage>

// local includes

#include "databasefacecontainers.h"

namespace KFaceIface
{

class DatabaseFaceAccessData;

class OpenCVLBPHFaceRecognizer
{

public:

    /**
     *  @brief FaceRecognizer:Master class to control entire recognition using LBPH algorithm
     */
    OpenCVLBPHFaceRecognizer(DatabaseFaceAccessData* const);
    ~OpenCVLBPHFaceRecognizer();

    void setThreshold(float threshold) const;

    /**
     *  Returns a cvMat created from the inputImage, optimized for recognition
     */
    cv::Mat prepareForRecognition(const QImage& inputImage);

    /**
     *  Try to recognize the given image.
     *  Returns the identity id.
     *  If the identity cannot be recognized, returns -1.
     */
    int recognize(const cv::Mat& inputImage);

    /**
     *  Trains the given images, representing faces of the given matched identities.
     */
    void train(const std::vector<cv::Mat>& images, const std::vector<int>& labels, const QString& context);

private:

    class Private;
    Private* const d;
};

} // namespace KFaceIface

#endif // OPENCV_LBPH_FACE_RECOGNIZER_H
