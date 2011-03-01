/** ===========================================================
 * @file
 *
 * This file is a part of digiKam project
 * <a href="http://www.digikam.org">http://www.digikam.org</a>
 *
 * @date   2010-06-18
 * @brief  Wrapper class for image data
 *
 * @author Copyright (C) 2010 by Marcel Wiesweg
 *         <a href="mailto:marcel dot wiesweg at gmx dot de">marcel dot wiesweg at gmx dot de</a>
 * @author Copyright (C) 2010 by Aditya Bhatt
 *         <a href="mailto:adityabhatt1991 at gmail dot com">adityabhatt1991 at gmail dot com</a>
 * @author Copyright (C) 2010 by Gilles Caulier
 *         <a href="mailto:caulier dot gilles at gmail dot com">caulier dot gilles at gmail dot com</a>
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

#include "image_p.h"

// Qt includes

#include <QFile>

// OpenCV includes

#include <opencv/cv.h>
#include <opencv/cxcore.h>
#include <opencv/highgui.h>

// Libface includes

#include <libface/LibFace.h>

// Local includes

#include "kfaceutils.h"

namespace KFaceIface
{

Image::Image()
{
}

Image::Image(const QString& filePath)
     : d(new ImagePriv)
{
    d->image = cvLoadImage(QFile::encodeName(filePath), CV_LOAD_IMAGE_GRAYSCALE);
}

Image::Image(const QImage& givenImage)
     : d(new ImagePriv)
{
    d->image = KFaceUtils::QImage2GrayscaleIplImage(KFaceUtils::QImage2Grayscale(givenImage));
}

Image::Image(uint width, uint height, bool sixteenBit, bool alpha, const uchar* data)
     : d(new ImagePriv)
{
    d->image = KFaceUtils::Data2GrayscaleIplImage(width, height, sixteenBit, alpha, data);
}

Image::Image(const Image& other)
     : d(other.d)
{
}

Image::Image(ImageData image)
     : d(new ImagePriv)
{
    // take ownership of IplImage
    d->image = image;
}

Image& Image::operator=(const Image& other)
{
    d = other.d;
    return *this;
}

Image::~Image()
{
}

bool Image::isNull() const
{
    return d ? false : true;
}

QSize Image::size() const
{
    if (!d)
        return QSize();

    return QSize(d->image->width, d->image->height);
}

void Image::setOriginalSize(const QSize& size)
{
    if (!d)
        return;

    d->originalSize = size;
}

QSize Image::originalSize() const
{
    return d->originalSize;
}

ImageData Image::imageData()
{
    return d ? d->image : 0;
}

const ImageData Image::imageData() const
{
    return d ? d->image : 0;
}

QImage Image::toQImage() const
{
    return KFaceUtils::IplImage2QImage(d->image);
}

} // namespace KFaceIface
