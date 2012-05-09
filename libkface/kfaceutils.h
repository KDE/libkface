/** ===========================================================
 * @file
 *
 * This file is a part of digiKam project
 * <a href="http://www.digikam.org">http://www.digikam.org</a>
 *
 * @date   2010-06-16
 * @brief  This is a collection of useful functions
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

#ifndef KFACEUTILS_H
#define KFACEUTILS_H

// OpenCV includes

#include "libopencv.h"

// Qt includes

#include <QtGui/QImage>

namespace KFaceIface
{

namespace KFaceUtils
{
    /**
     * This code is adapted from code (C) Rik Hemsley <rik@kde.org>
     * and further adapted from code by Mosfet, ported from old KDE 3.5 imaging code.
     * @param img The image whose grayscale version is desired
     * @return A grayscale QImage, RGB+Alpha
     */
    QImage QImage2Grayscale(const QImage& img);

    /**
     * Takes QImage and converts returns a pointer to an IplImage
     * @param img The QImage to be converted.
     * @return the IplImage pointer
     */
    IplImage* QImage2GrayscaleIplImage(const QImage& img);

    /**
     * Creates and returns a pointer to an IplImage from raw image data.
     * @param width Width of Image
     * @param height Height of Image
     * @param sixteenBit Boolean value indicating "sixteen-bit-ness"
     * @param alpha Boolean value indicating presence of alpha channel
     * @param data Unsigned char* image data
     * @return A pointer to the new IplImage
     */
    IplImage* Data2GrayscaleIplImage(uint width, uint height, bool sixteenBit, bool alpha, const uchar* const data);

    /**
     * Takes a pointer to an IplImage and returns a Qimage
     * @param img IplImage pointer
     * @return QImage version of image
     */
    QImage IplImage2QImage(const IplImage* const img);

    /**
     * Takes a filename and returns a string to integer hash.
     * @param fileName The name of the file from which the hash is to be retrieved
     * @return A QHash<QString, int>, which maps the name of a person to his ID in the database
     */
    QHash<QString, int> hashFromFile(const QString& fileName);

    /**
     * Adds a person's name and ID to the specified file
     * @param fileName The QString name of the file which is to be update with the new person
     * @param name The QString name of the new person
     * @param id The integer ID of the new person
     */
    void addNameToFile(const QString& fileName, const QString& name, const int id);

    QSize  fromCvSize(const CvSize& size);
    CvSize toCvSize(const QSize& size);
}

} // namespace KFaceIface

#endif // KFACEUTILS_H
