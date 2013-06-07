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

#ifndef KFACE_LBPHFACEMODEL_H
#define KFACE_LBPHFACEMODEL_H

// OpenCV includes

#include "libopencv.h"

// Qt include

#include <QImage>

// local includes

#include "dbcontainers.h"

namespace KFaceIface
{

class DatabaseAccessData;

class LBPHistogramMetadata
{
public:

    LBPHistogramMetadata();

    enum StorageStatus
    {
        Created,
        InDatabase
    };

    int     databaseId;
    int     identity;
    QString context;

    StorageStatus storageStatus;
};

class LBPHFaceModel : public cv::Ptr<cv::FaceRecognizer>
{
public:

    LBPHFaceModel();

    cv::FaceRecognizer* ptr() { return cv::Ptr<cv::FaceRecognizer>::operator cv::FaceRecognizer*(); }
    const cv::FaceRecognizer* ptr() const { return cv::Ptr<cv::FaceRecognizer>::operator const cv::FaceRecognizer*(); }

    int radius() const;
    void setRadius(int radius);

    int neighbors() const;
    void setNeighbors(int neighbors);

    int gridX() const;
    void setGridX(int grid_x);

    int gridY() const;
    void setGridY(int grid_y);

    QList<LBPHistogramMetadata> histogramMetadata() const;
    OpenCVMatData histogramData(int index) const;

    void setWrittenToDatabase(int index, int databaseId);

    void setHistograms(const QList<OpenCVMatData>& histograms, const QList<LBPHistogramMetadata>& histogramMetadata);

    /// Make sure to call this instead of FaceRecognizer::update directly!
    void update(const std::vector<cv::Mat>& images, const std::vector<int>& labels, const QString& context);

    int databaseId;

protected:

    QList<LBPHistogramMetadata> m_histogramMetadata;
};

} // namespace KFaceIface

#endif // KFACE_LBPHFACEMODEL_H