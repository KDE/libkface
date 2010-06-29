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

#ifndef KFACE_IMAGE_P_H
#define KFACE_IMAGE_P_H

// OpenCV includes

#include <opencv/cv.h>

namespace KFaceIface
{

// INTERNAL

class ImageData
{
public:

    ImageData() : image(0) {}

    ImageData(IplImage* image) : image(image) {}
    ImageData& operator=(IplImage* im) { image = im; return *this; }
    operator IplImage *() const { return image; }

    IplImage* image;
};

} // namespace KFaceIface

#endif // KFACE_IMAGE_P_H
