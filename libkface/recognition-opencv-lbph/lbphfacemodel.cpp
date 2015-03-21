/** ===========================================================
 * @file
 *
 * This file is a part of digiKam project
 * <a href="http://www.digikam.org">http://www.digikam.org</a>
 *
 * @date   2010-03-03
 * @brief  LBPH interface.
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

#include "lbphfacemodel.h"

// Qt includes

#include <QList>

// KDE includes

#include <kdebug.h>

// local includes

#include "databaseaccess.h"

namespace KFaceIface
{

LBPHistogramMetadata::LBPHistogramMetadata()
    : databaseId(0), 
      identity(0), 
      storageStatus(Created)
{
}

LBPHistogramMetadata::~LBPHistogramMetadata()
{
}

// ------------------------------------------------------------------------------------

LBPHFaceModel::LBPHFaceModel()
    : cv::Ptr<LBPHFaceRecognizer>(LBPHFaceRecognizer::create()),
      databaseId(0)
{
    ptr()->set("threshold", 100.0);
}

LBPHFaceModel::~LBPHFaceModel()
{
}

LBPHFaceRecognizer* LBPHFaceModel::ptr()
{
    LBPHFaceRecognizer* const ptr = cv::Ptr<LBPHFaceRecognizer>::operator KFaceIface::LBPHFaceRecognizer*();

    if (!ptr) 
        kWarning() << "LBPHFaceRecognizer pointer is null";

    return ptr;
}

const LBPHFaceRecognizer* LBPHFaceModel::ptr() const
{
    const LBPHFaceRecognizer* const ptr = cv::Ptr<LBPHFaceRecognizer>::operator const KFaceIface::LBPHFaceRecognizer*();

    if (!ptr) 
        kWarning() << "LBPHFaceRecognizer pointer is null";

    return ptr;
}

int LBPHFaceModel::radius() const
{
    return ptr()->get<int>("radius");
}

void LBPHFaceModel::setRadius(int radius)
{
    ptr()->set("radius", radius);
}

int LBPHFaceModel::neighbors() const
{
    return ptr()->get<int>("neighbors");
}

void LBPHFaceModel::setNeighbors(int neighbors)
{
    ptr()->set("neighbors", neighbors);
}

int LBPHFaceModel::gridX() const
{
    return ptr()->get<int>("grid_x");
}

void LBPHFaceModel::setGridX(int grid_x)
{
    ptr()->set("grid_x", grid_x);
}

int LBPHFaceModel::gridY() const
{
    return ptr()->get<int>("grid_y");
}

void LBPHFaceModel::setGridY(int grid_y)
{
    ptr()->set("grid_y", grid_y);
}

OpenCVMatData LBPHFaceModel::histogramData(int index) const
{
    return OpenCVMatData(ptr()->get<std::vector<cv::Mat> >("histograms").at(index));
}

QList<LBPHistogramMetadata> LBPHFaceModel::histogramMetadata() const
{
    return m_histogramMetadata;
}

void LBPHFaceModel::setWrittenToDatabase(int index, int id)
{
    m_histogramMetadata[index].databaseId    = id;
    m_histogramMetadata[index].storageStatus = LBPHistogramMetadata::InDatabase;
}

void LBPHFaceModel::setHistograms(const QList<OpenCVMatData>& histograms, const QList<LBPHistogramMetadata>& histogramMetadata)
{
    /*
     * Does not work with standard OpenCV, as these two params are declared read-only in OpenCV.
     * One reason why we copied the code.
     */
    std::vector<cv::Mat> newHistograms;
    cv::Mat newLabels;
    newHistograms.reserve(histograms.size());
    newLabels.reserve(histogramMetadata.size());

    foreach (const OpenCVMatData& histogram, histograms)
    {
        newHistograms.push_back(histogram.toMat());
    }

    m_histogramMetadata.clear();

    foreach (const LBPHistogramMetadata& metadata, histogramMetadata)
    {
        newLabels.push_back(metadata.identity);
        m_histogramMetadata << metadata;
    }

    std::vector<cv::Mat> currentHistograms = ptr()->get<std::vector<cv::Mat> >("histograms");
    cv::Mat currentLabels                  = ptr()->get<cv::Mat>("labels");
    currentHistograms.insert(currentHistograms.end(), newHistograms.begin(), newHistograms.end());
    currentLabels.push_back(newLabels);
    ptr()->set("histograms", currentHistograms);
    ptr()->set("labels", currentLabels);

/*
    //Most cumbersome and inefficient way through a file storage which we were forced to use if we used standard OpenCV
    cv::FileStorage store(".yml", cv::FileStorage::WRITE + cv::FileStorage::MEMORY);
    // store current parameters to preserve them
    store << "radius"     << radius();
    store << "neighbors"  << neighbors();
    store << "grid_x"     << gridX();
    store << "grid_y"     << gridY();
    // Write histogram data
    store << "histograms" << "[";

    foreach (const OpenCVMatData& histogram, histograms)
    {
        store << histogram.toMat();
    }

    store << "]";
    // write matching labels
    cv::Mat labels;

    foreach (const LBPHistogramMetadata& metadata, histogramMetadata)
    {
        labels.push_back(metadata.identity);
    }

    store << "labels" << labels;
    // harvest
    cv::String yaml = store.releaseAndGetString();

    cv::FileStorage read(yaml, cv::FileStorage::READ + cv::FileStorage::MEMORY);
    ptr()->load(read);
*/
}

void LBPHFaceModel::update(const std::vector<cv::Mat>& images, const std::vector<int>& labels, const QString& context)
{
    ptr()->update(images, labels);

    // Update local information
    // We assume new labels are simply appended
    cv::Mat currentLabels = ptr()->get<cv::Mat>("labels");

    for (int i = m_histogramMetadata.size() ; i < currentLabels.rows ; i++)
    {
        LBPHistogramMetadata metadata;
        metadata.storageStatus = LBPHistogramMetadata::Created;
        metadata.identity      = currentLabels.at<int>(i);
        metadata.context       = context;
        m_histogramMetadata << metadata;
    }
}

} // namespace KFaceIface
