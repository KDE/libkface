/**
* This file is part of libkface.
*
* libkface is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 2 of the License, or
* (at your option) any later version.
*
* libkface is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with libkface.  If not, see <http://www.gnu.org/licenses/>.
*
* @note: This is a collection of useful functions
* @author: Aditya Bhatt, Marcel Wiesweg
*/

#include <kdebug.h>

#include "kfaceutils.h"

namespace KFace
{

IplImage* KFaceUtils::QImage2IplImage(const QImage &qimg)
{

    IplImage *imgHeader = cvCreateImageHeader( cvSize(qimg.width(), qimg.width()), IPL_DEPTH_8U, 4);
    imgHeader->imageData = (char*) qimg.bits();

    //uchar* newdata = (uchar*) malloc(sizeof(uchar) * qimg->byteCount());
    //memcpy(newdata, qimg->bits(), qimg->byteCount());
    //imgHeader->imageData = (char*) newdata;
    imgHeader->imageData = (char *)qimg.bits();

    return imgHeader;
}

QImage KFaceUtils::IplImage2QImage(const IplImage *iplImg)
{
    int h = iplImg->height;
    int w = iplImg->width;
    int channels = iplImg->nChannels;
    QImage qimg(w, h, QImage::Format_ARGB32);
    char *data = iplImg->imageData;

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

}

