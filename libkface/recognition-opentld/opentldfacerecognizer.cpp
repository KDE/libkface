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

#include "opentldfacerecognizer.h"

// Qt includes

#include <QList>

// KDE includes

#include <kdebug.h>

// OpenTLD (local) includes

#include "TLD.h"

// local includes

#include "databaseaccess.h"
#include "libopencv.h"
#include "trainingdb.h"

namespace KFaceIface
{

class OpenTLDFaceRecognizer::Private
{
public:

    Private(DatabaseAccessData* const db)
        : db(db),
          threshold(0.8F),
          m_tld(0)
    {
    }

    ~Private()
    {
        delete m_tld;
    }

public:

    tld::TLD* tld()
    {
        if (!m_tld)
        {
            // Generate TLD core
            m_tld = new tld::TLD();

            // Read database
            QList<int> identityIds = DatabaseAccess(db).db()->identityIds();

            foreach (int id, identityIds)
            {
                modelCache[id] = DatabaseAccess(db).db()->tldFaceModels(id);
            }
        }

        return m_tld;
    }

public:

    DatabaseAccessData* db;
    float               threshold;
    QMap<int, QList<UnitFaceModel> > modelCache;

private:

    tld::TLD *m_tld;
};

OpenTLDFaceRecognizer::OpenTLDFaceRecognizer(DatabaseAccessData* const db)
    : d(new Private(db))
{
}

OpenTLDFaceRecognizer::~OpenTLDFaceRecognizer()
{
    delete d;
}

void OpenTLDFaceRecognizer::setThreshold(const float threshold) const
{
    d->threshold = qBound(0.f, threshold, 1.f);
}

namespace
{

    enum
    {
        TargetInputSize = 47
    };
}

cv::Mat OpenTLDFaceRecognizer::prepareForRecognition(const QImage& inputImage)
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

float OpenTLDFaceRecognizer::recognitionConfidence(const cv::Mat& im, const UnitFaceModel& compareModel)
{
    d->tld()->detectorCascade->imgWidth     = im.cols;
    d->tld()->detectorCascade->imgHeight    = im.rows;
    d->tld()->detectorCascade->imgWidthStep = im.step;
    d->tld()->getObjModel(compareModel);
    d->tld()->processImage(im);
    float result = d->tld()->currConf;
    d->tld()->release();
    return result;
}

int OpenTLDFaceRecognizer::recognize(const cv::Mat& inputImage)
{
    QMap<int, int> count;
    QMap<int, QList<UnitFaceModel> >::const_iterator it;

    for (it = d->modelCache.constBegin(); it != d->modelCache.constEnd(); ++it)
    {
        foreach (const UnitFaceModel& compareModel, it.value())
        {
            float confidence = recognitionConfidence(inputImage, compareModel);

            if (confidence > d->threshold)
            {
                count[it.key()]++;
            }
        }
    }

    int maxCount = 0;
    int bestId   = -1;

    for (QMap<int,int>::const_iterator it = count.constBegin(); it != count.constEnd(); ++it)
    {
        if (it.value() > maxCount)
        {
            maxCount = it.value();
            bestId = it.key();
        }
    }

    return bestId;
}

void OpenTLDFaceRecognizer::createFaceModel(const cv::Mat& im, UnitFaceModel& model)
{
    d->tld()->detectorCascade->imgWidth     = im.cols;
    d->tld()->detectorCascade->imgHeight    = im.rows;
    d->tld()->detectorCascade->imgWidthStep = im.step;

    cv::Rect bb(5, 5, im.cols-5, im.rows-5);
    d->tld()->selectObject(im, &bb);
    d->tld()->putObjModel(model);
    d->tld()->release();
}

void OpenTLDFaceRecognizer::train(int identity, const cv::Mat& inputImage, const QString&)
{
    UnitFaceModel model;
    // create model
    createFaceModel(inputImage, model);
    // add to database
    DatabaseAccess(d->db).db()->addTLDFaceModel(identity, model);
    // update local cache
    d->modelCache[identity] << model;
}

} // namespace KFaceIface
