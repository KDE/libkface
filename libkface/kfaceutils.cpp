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

// KDE include

#include <kdebug.h>

// Local includes

#include "kfaceutils.h"

#include <opencv/highgui.h>

namespace KFace
{
/**
 This code is adapted from code (C) Rik Hemsley <rik@kde.org>
 and further adapted from code by Mosfet, ported from old KDE 3.5 imaging code.
 @param qimg The image whose grayscale version is desired
 @return The grayscale version, RGB+Alpha
 */

QImage KFaceUtils::QImage2Grayscale(const QImage &qimg)
{
    QImage img = qimg;
    if (img.width() == 0 || img.height() == 0)
      return img;

        int pixels = img.width()*img.height() ;
        unsigned int *data =  (unsigned int *)img.bits();
        int val, i;
        for(i=0; i < pixels; ++i){
            val = qGray(data[i]);
            data[i] = qRgba(val, val, val, qAlpha(data[i]));
        }
    return img;
}


IplImage* KFaceUtils::QImage2IplImage(const QImage& qimg)
{
    QImage img           = QImage2Grayscale(qimg);
    IplImage* imgHeader  = cvCreateImageHeader( cvSize(img.width(), img.height()), IPL_DEPTH_8U, 4);
#if QT_VERSION > 0x040503
    uchar* newdata       = (uchar*) malloc(sizeof(uchar) * img.byteCount());
    memcpy(newdata, img.bits(), img.byteCount());
#else
    uchar* newdata       = (uchar*) malloc(sizeof(uchar) * img.numBytes());
    memcpy(newdata, img.bits(), img.numBytes());
#endif
    imgHeader->imageData = (char*) newdata;

    return imgHeader;
}

QImage KFaceUtils::IplImage2QImage(const IplImage* iplImg)
{
    int h        = iplImg->height;
    int w        = iplImg->width;
    int channels = iplImg->nChannels;
    QImage qimg(w, h, QImage::Format_ARGB32);
    char* data   = iplImg->imageData;

    if (channels != 1 && channels != 3 && channels != 4)
    {
        kError() << "Unsupported number of channels in IplImage:" << channels;
        return QImage();
    }

    for (int y = 0; y < h; y++, data += iplImg->widthStep)
    {
        for (int x = 0; x < w; x++)
        {
            char r, g, b, a = 0;
            if (channels == 1)
            {
                r = data[x * channels];
                g = data[x * channels];
                b = data[x * channels];
            }
            else // if (channels == 3 || channels == 4)
            {
                r = data[x * channels + 2];
                g = data[x * channels + 1];
                b = data[x * channels];
            }

            if (channels == 4)
            {
                a = data[x * channels + 3];
                qimg.setPixel(x, y, qRgba(r, g, b, a));
            }
            else
            {
                qimg.setPixel(x, y, qRgb(r, g, b));
            }
        }
    }

    return qimg;
}

} // namespace KFace
