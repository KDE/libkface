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
 * @author Copyright (C) 2010-2012 by Gilles Caulier
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

#ifndef KFACE_IMAGE_H
#define KFACE_IMAGE_H

// Qt includes

#include <QtGui/QImage>
#include <QtCore/QSharedDataPointer>

// Local includes

#include "libkface_export.h"

namespace KFaceIface
{

class ImageData;

class KFACE_EXPORT Image
{
public:

    /** Creates a null Image */
    Image();

    /** Load the image from the given file. */
    Image(const QString& filePath);

    /** Create an image from the given QImage object.
     *  If the image has been scaled down, please call setOriginalSize.
     */
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
     *  If the image has been scaled down, please call setOriginalSize.
     */
    Image(uint width, uint height, bool sixteenBit, bool alpha, const uchar* const data);

    /**
     * Copy Constructor.
     */
    Image(const Image& other);

    /**
     * Constructor. Create an Image from an ImageData struct
     */
    Image(ImageData other);

    /**
     * Assignment operator
     * @param other An Image to be assigned
     */
    Image& operator=(const Image& other);

    /**
     * Destructor
     */
    ~Image();

    /**
     * Check if the image is null
     * @return A boolean value, that is true if the image is null
     */
    bool isNull() const;

    QSize size() const;

    /**
     * If the image has been scaled down already, give the original size.
     */
    void setOriginalSize(const QSize& size);
    QSize originalSize() const;

    ImageData imageData();
    const ImageData imageData() const;

    QImage toQImage() const;

private:

    class ImagePriv;
    QSharedDataPointer<ImagePriv> d;
};

} // namespace KFaceIface

#endif // K_FACE_IMAGE_H
