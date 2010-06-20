/** ===========================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * @date  : 2010-06-16
 * @note  : This is a collection of useful functions
 *
 * @author: Copyright (C) 2010 by Marcel Wiesweg <marcel.wiesweg at gmx dot de>
 *          Copyright (C) 2010 by Aditya Bhatt <adityabhatt1991 at gmail dot com>
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

#ifndef KFACEUTILS_H
#define KFACEUTILS_H

// OpenCV includes

#include <opencv/cv.h>

// Qt includes

#include <QImage>

namespace KFace
{

namespace KFaceUtils
{
    QImage QImage2Grayscale(const QImage& img);
    IplImage* QImage2IplImage(const QImage& img);
    IplImage* Data2IplImage(uint width, uint height, bool sixteenBit, bool alpha, const uchar *data);
    QImage IplImage2QImage(const IplImage* img);
};

} // namespace KFace

#endif // KFACEUTILS_H
