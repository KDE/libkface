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

#include "kfaceutils.h"

// C++ includes

#include <cassert>

// Qt includes

#include <QFile>
#include <QDataStream>

// KDE include

#include <kdebug.h>

// OpenCV includes

#include "libopencv.h"

namespace KFaceIface
{

QImage KFaceUtils::QImage2Grayscale(const QImage& qimg)
{
    QImage img = qimg;
    if (img.width() == 0 || img.height() == 0)
      return img;

    int pixels         = img.width() * img.height() ;
    unsigned int* data = (unsigned int*)img.bits();

    int val;
    for(int i=0; i < pixels; ++i)
    {
        val     = qGray(data[i]);
        data[i] = qRgba(val, val, val, qAlpha(data[i]));
    }
    return img;
}

IplImage* KFaceUtils::QImage2GrayscaleIplImage(const QImage& qimg)
{
    QImage localImage;
    switch (qimg.format())
    {
        case QImage::Format_RGB32:
        case QImage::Format_ARGB32:
        case QImage::Format_ARGB32_Premultiplied:
            // I think we can ignore premultiplication when converting to grayscale
            localImage = qimg;
            break;
        default:
            localImage = qimg.convertToFormat(QImage::Format_RGB32);
            break;
    }
    // I'm a bit paranoid not to cause a deep copy when calling bits()
    const QImage& image = localImage;
    const int width     = image.width();
    const int height    = image.height();
    IplImage* iplImg    = 0;

    try
    {
        iplImg = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 1);
    }
    catch (cv::Exception& e)
    {
        kError() << "Cannot allocate IplImage:" << e.what();
        return 0;
    }
    catch(...)
    {
        kError() << "Cannot allocate IplImage";
        return 0;
    }

    const quint32* sptr = (const quint32*)image.bits();
    const int imageStep = image.bytesPerLine() / sizeof(quint32);
    uchar* dptr         = (uchar*)iplImg->imageData;
    const int iplStep   = iplImg->widthStep / sizeof(uchar);

    for (int y=0; y<height; ++y)
    {
        for (int x=0; x<width; ++x)
        {
            dptr[x] = qGray(sptr[x]);
        }
        sptr += imageStep;
        dptr += iplStep;
    }

    return iplImg;
}

IplImage* KFaceUtils::Data2GrayscaleIplImage(uint width, uint height, bool sixteenBit, bool alpha, const uchar* const data)
{
    Q_UNUSED(alpha);

    IplImage* img = 0;
    try
    {
        img = cvCreateImage( cvSize(width, height), IPL_DEPTH_8U, 1);
    }
    catch (cv::Exception& e)
    {
        kError() << "Cannot allocate IplImage:" << e.what();
        return 0;
    }
    catch(...)
    {
        kError() << "Cannot allocate IplImage";
        return 0;
    }

    uchar* dptr       = (uchar*)img->imageData;
    const int iplStep = img->widthStep / sizeof(uchar);

    if (sixteenBit)
    {
        const ushort* sptr = (const ushort*)data;

        for (uint y=0; y<height; ++y)
        {
            for (uint x=0; x<width; ++x)
            {
                dptr[x] = qGray((sptr[2] * 255UL) / 65535UL,    // R
                                (sptr[1] * 255UL) / 65535UL,    // G
                                (sptr[0] * 255UL) / 65535UL);   // B
                sptr += 4;
            }
            dptr += iplStep;
        }
    }
    else
    {
        const uchar* sptr = data;

        for (uint y=0; y<height; ++y)
        {
            for (uint x=0; x<width; ++x)
            {
                dptr[x] = qGray(sptr[2], sptr[1], sptr[0]);
                sptr += 4;
            }
            dptr += iplStep;
        }
    }

    return img;
}

QImage KFaceUtils::IplImage2QImage(const IplImage* const iplImg)
{
    try
    {
        int h        = iplImg->height;
        int w        = iplImg->width;
        int channels = iplImg->nChannels;
        QImage qimg(w, h, QImage::Format_ARGB32);
        char* data   = iplImg->imageData;

        if (channels != 1 && channels != 3 && channels != 4)
        {
            kError(51005) << "Unsupported number of channels in IplImage:" << channels;
            return QImage();
        }

        char r=0, g=0, b=0, a = 0;

        for (int y = 0; y < h; ++y, data += iplImg->widthStep)
        {
            for (int x = 0; x < w; ++x)
            {
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
                    b = data[x * channels    ];
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
    catch (cv::Exception& e)
    {
        kError() << "Cannot convert OpenCV Image to QImage:" << e.what();
    }
    catch(...)
    {
        kDebug() << "Cannot convert OpenCV Image to QImage";
    }

    return QImage();
}

QHash<QString, int> KFaceUtils::hashFromFile(const QString& fileName)
{
    QFile file(fileName);
    file.open(QIODevice::ReadOnly|QIODevice::Text);

    QHash<QString, int> tempHash;
    QPair<QString, int> namePair;
    QDataStream in(&file);

    while( !in.atEnd() )
    {
        in >> namePair;
        tempHash[namePair.first] = namePair.second;
    }

    file.close();

    return tempHash;
}

void KFaceUtils::addNameToFile(const QString& fileName, const QString& name, const int id)
{
    QFile file(fileName);
    file.open(QIODevice::Append|QIODevice::WriteOnly|QIODevice::Text);

    QDataStream out(&file);
    out << qMakePair<QString, int>(name,id);

    file.close();
}

QSize KFaceUtils::fromCvSize(const CvSize& size)
{
    return QSize(size.width, size.height);
}

CvSize KFaceUtils::toCvSize(const QSize& size)
{
    return cvSize(size.width(), size.height());
}

} // namespace KFaceIface
