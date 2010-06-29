/** ===========================================================
 *
 * This file is a part of digiKam project
 * <a href="http://www.digikam.org">http://www.digikam.org</a>
 *
 * @date   2010-06-18
 * @brief  Wrapper class for image data
 *
 * @author Copyright (C) 2010 by Marcel Wiesweg
 *         <a href="mailto:marcel dot wiesweg at gmx dot de">marcel dot wiesweg at gmx dot de</a>
 *         Copyright (C) 2010 by Aditya Bhatt
 *         <a href="mailto:adityabhatt1991 at gmail dot com">adityabhatt1991 at gmail dot com</a>
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

// OpenCV includes

#include <opencv/cv.h>
#include <opencv/cxcore.h>
#include <opencv/highgui.h>

// Local includes

#include "image_p.h"
#include "image.h"
#include "kfaceutils.h"

namespace KFaceIface
{

class ImagePriv : public QSharedData
{
public:

    ImagePriv() : image(0)
    {
    }

    ImagePriv(const ImagePriv& other) : QSharedData(other)
    {
        // this code is called of we want to detach()
        image = cvCloneImage(other.image);
    }

    ~ImagePriv()
    {
        if (image)
            cvReleaseImage(&image);
    }

    IplImage* image;
};

Image::Image()
{
}

Image::Image(const QString& filePath)
     : d(new ImagePriv)
{
    d->image = cvLoadImage(filePath.toLocal8Bit(), CV_LOAD_IMAGE_GRAYSCALE);
}

Image::Image(const QImage& givenImage)
     : d(new ImagePriv)
{
    d->image = KFaceUtils::QImage2IplImage(givenImage);
}

Image::Image(uint width, uint height, bool sixteenBit, bool alpha, const uchar *data)
     : d(new ImagePriv)
{
    d->image = KFaceUtils::Data2IplImage(width, height, sixteenBit, alpha, data);
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
    return d;
}

QSize Image::size() const
{
    if (!d)
        return QSize();

    return QSize(d->image->width, d->image->height);
}

const ImageData Image::imageData() const
{
    return d->image;
}

} // namespace KFaceIface
