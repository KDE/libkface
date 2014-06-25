/* ============================================================
 *
 * This file is a part of digikam/libkface
 *
 * Date        : 2013-26-05
 *
 * Copyright (C) 2013 by Marcel Wiesweg <marcel dot wiesweg at uk-essen dot de>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "dbcontainers.h"

// KDE includes

#include <kdebug.h>

namespace KFaceIface
{

OpenCVMatData::OpenCVMatData()
    : type(-1), 
      rows(0),
      cols(0)
{
}

OpenCVMatData::OpenCVMatData(const cv::Mat& mat)
    : type(-1), 
      rows(0),
      cols(0)
{
    setMat(mat);
}

void OpenCVMatData::setMat(const cv::Mat& mat)
{
    type                   = mat.type();
    rows                   = mat.rows;
    cols                   = mat.cols;
    const size_t data_size = cols * rows * mat.elemSize();
    data                   = QByteArray::fromRawData((const char*)mat.ptr(), data_size);
}

cv::Mat OpenCVMatData::toMat() const
{
    // shallow copy (only creates header)

    if (data.isEmpty())
    {
        kWarning() << "Array data to clone is empty.";
    }

    cv::Mat mat(rows, cols, type, (void*)data.constData());

    kDebug() << "Clone Array size [" << rows << ", " << cols << "] of type " << type;

    return mat.clone();
}

void OpenCVMatData::clearData()
{
    type = -1;
    rows = 0;
    cols = 0;
    data.clear();
}

} // namespace KFaceIface
