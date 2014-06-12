/** ===========================================================
 * @file
 *
 * This file is a part of digiKam project
 * <a href="http://www.digikam.org">http://www.digikam.org</a>
 *
 * @date    2013-06-14
 * @brief   Alignment by Image Congealing
 *
 * Gary B. Huang, Vidit Jain, and Erik Learned-Miller.
 * Unsupervised joint alignment of complex images.
 * International Conference on Computer Vision (ICCV), 2007.
 *
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

#ifndef KFACE_ALIGNMENT_FLANDMARK_ALIGNER
#define KFACE_ALIGNMENT_FLANDMARK_ALIGNER

// OpenCV includes

#include "libopencv.h"

// Qt include

#include <QImage>

namespace KFaceIface
{

class FlandmarkAligner
{

public:

    FlandmarkAligner();
    ~FlandmarkAligner();

    cv::Mat align(const cv::Mat& inputImage);

private:

    class Private;
    Private* const d;
};

} // namespace KFaceIface

#endif // KFACE_ALIGNMENT_CONGEALING_FUNNELREAL_H