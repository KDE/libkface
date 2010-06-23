/** ===========================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * @date  : 2010-06-18
 * @brief : Wrapper class for image data
 *
 * @author: Copyright (C) 2010 by Marcel Wiesweg
 *          <a href="mailto:marcel dot wiesweg at gmx dot de">marcel dot wiesweg at gmx dot de</a>
 *          Copyright (C) 2010 by Aditya Bhatt
 *          <a href="mailto:adityabhatt1991 at gmail dot com">adityabhatt1991 at gmail dot com</a>
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

#ifndef KFACE_IMAGE_H
#define KFACE_IMAGE_H

// Qt includes

#include <QImage>
#include <QExplicitlySharedDataPointer>

// Local includes

#include "libkface_export.h"

namespace KFace
{

class ImagePriv;
class ImageData;

class KFACE_EXPORT Image
{
public:

    /** Creates a null Image */
    Image();
    /** Load the image from the given file. */
    Image(const QString& filePath);
    /** Create an image from the given QImage object */
    Image(const QImage& image);
    /**
     *  Create an image from the given data.
     *  The image data is in the BGRA format, in a buffer suitable for the given image size.
     *  If sixteenBit is true, each channel has a size of 16bit (48 bits per pixel),
     *  if sixteenBit is false, each channel has a size of 8bit (32 bits per pixel).
     *  If alpha channel is true, the fourth channel contains information on color alpha,
     *  if false, the fourth channel shall be ignored and can assumed to be set to 0xFF.
     *  Ownership of the data buffer remains with the caller, this constructor will take a copy
     *  of the data.
     */
    Image(uint width, uint height, bool sixteenBit, bool alpha, const uchar* data);

    Image(const Image& other);
    Image(ImageData other);
    Image& operator=(const Image& other);

    ~Image();

    bool isNull() const;
    QSize size() const;

    const ImageData imageData() const;

private:

    QExplicitlySharedDataPointer<ImagePriv> d;
};

}; // namespace KFace

#endif // K_FACE_IMAGE_H
