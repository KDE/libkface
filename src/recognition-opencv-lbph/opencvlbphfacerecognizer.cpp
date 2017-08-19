/** ===========================================================
 * @file
 *
 * This file is a part of KDE project
 *
 *
 * @date   2010-03-03
 * @brief  LBPH Recognizer
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

#include "opencvlbphfacerecognizer.h"

// local includes

#include "libkface_debug.h"
#include "databasefaceaccess.h"
#include "libopencv.h"
#include "lbphfacemodel.h"
#include "trainingdb.h"

namespace KFaceIface
{

class OpenCVLBPHFaceRecognizer::Private
{
public:

    Private(DatabaseFaceAccessData* const db)
        : db(db),
          threshold(100),
          loaded(false)
    {
    }

public:

    LBPHFaceModel& lbph()
    {
        if (!loaded)
        {
            m_lbph = DatabaseFaceAccess(db).db()->lbphFaceModel();
            loaded = true;
        }

        return m_lbph;
    }

public:

    DatabaseFaceAccessData* db;
    float               threshold;

private:

    LBPHFaceModel       m_lbph;
    bool                loaded;
};

OpenCVLBPHFaceRecognizer::OpenCVLBPHFaceRecognizer(DatabaseFaceAccessData* const db)
    : d(new Private(db))
{
    setThreshold(0.5);
}

OpenCVLBPHFaceRecognizer::~OpenCVLBPHFaceRecognizer()
{
    delete d;
}

void OpenCVLBPHFaceRecognizer::setThreshold(float threshold) const
{
    // threshold for our purposes within 20..150
    const float min = 30.0;
    const float max = 150.0;
    // Applying a mirrored sigmoid curve
    // map threshold [0,1] to [-4, 4]
    float t         = (8.0 * qBound(0.f, threshold, 1.f)) - 4.0;
    // 1/(1+e^(t))
    float factor    = 1.0 / (1.0 + exp(t));
    d->threshold    = min + factor*(max-min);
}

namespace
{
    enum
    {
        TargetInputSize = 256
    };
}

cv::Mat OpenCVLBPHFaceRecognizer::prepareForRecognition(const QImage& inputImage)
{
    QImage image(inputImage);

    if (inputImage.width() > TargetInputSize || inputImage.height() > TargetInputSize)
    {
        image = inputImage.scaled(TargetInputSize, TargetInputSize, Qt::IgnoreAspectRatio);
    }

    cv::Mat cvImage = cv::Mat(image.height(), image.width(), CV_8UC1);
    cv::Mat cvImageWrapper;

    switch (image.format())
    {
        case QImage::Format_RGB32:
        case QImage::Format_ARGB32:
        case QImage::Format_ARGB32_Premultiplied:
            // I think we can ignore premultiplication when converting to grayscale
            cvImageWrapper = cv::Mat(image.height(), image.width(), CV_8UC4, image.scanLine(0), image.bytesPerLine());
            cvtColor(cvImageWrapper, cvImage, CV_RGBA2GRAY);
            break;
        default:
            image          = image.convertToFormat(QImage::Format_RGB888);
            cvImageWrapper = cv::Mat(image.height(), image.width(), CV_8UC3, image.scanLine(0), image.bytesPerLine());
            cvtColor(cvImageWrapper, cvImage, CV_RGB2GRAY);
            break;
    }

    equalizeHist(cvImage, cvImage);
    return cvImage;
}

int OpenCVLBPHFaceRecognizer::recognize(const cv::Mat& inputImage)
{
    int predictedLabel = -1;
    double confidence  = 0;
    d->lbph()->predict(inputImage, predictedLabel, confidence);
    qCDebug(LIBKFACE_LOG) << predictedLabel << confidence;

    if (confidence > d->threshold)
    {
        return -1;
    }

    return predictedLabel;
}

void OpenCVLBPHFaceRecognizer::train(const std::vector<cv::Mat>& images, const std::vector<int>& labels, const QString& context)
{
    if (images.empty() || labels.size() != images.size())
    {
        return;
    }

    d->lbph().update(images, labels, context);
    // add to database
    DatabaseFaceAccess(d->db).db()->updateLBPHFaceModel(d->lbph());
}

} // namespace KFaceIface
