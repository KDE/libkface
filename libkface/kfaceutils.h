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

#ifndef KFACEUTILS_H
#define KFACEUTILS_H

#include <opencv/cv.h>
#include <QImage>

namespace KFace
{

namespace KFaceUtils
{
    IplImage* QImage2IplImage(const QImage& img);
    QImage IplImage2QImage(const IplImage *img);
};

}

#endif // KFACEUTILS_H
