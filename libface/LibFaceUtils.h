/** ===========================================================
 * @file
 *
 * This file is a part of libface project
 * <a href="http://libface.sourceforge.net">http://libface.sourceforge.net</a>
 *
 * @date    2010-04-07
 * @brief   Libface utility methods.
 * @section DESCRIPTION
 *
 * @author Copyright (C) 2010 by Alex Jironkin
 *         <a href="alexjironkin at gmail dot com">alexjironkin at gmail dot com</a>
 * @author Copyright (C) 2010 by Aditya Bhatt
 *         <a href="adityabhatt at gmail dot com">adityabhatt at gmail dot com</a>
 * @author Copyright (C) 2010-2013 by Gilles Caulier
 *         <a href="mailto:caulier dot gilles at gmail dot com">caulier dot gilles at gmail dot com</a>
 *
 * @section LICENSE
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

#ifndef _LIBFACEUTILS_H_
#define _LIBFACEUTILS_H_

#include "libopencv.h"

#include <cstdio>
#include <cstdlib>
#include <string>

#include "Face.h"

namespace libface
{

class FACEAPI LibFaceUtils
{
public:

    static IplImage*   resizeToArea(const IplImage* const img, int area, double& ratio);
    static CvPoint     center(const Face&);
    static int         distance(const CvPoint&, const CvPoint&);
    static int         distance(const Face&, const Face&);

    static CvMat*      addScalar(CvMat* const src, const CvScalar& value);
    static CvMat*      combine(CvMat* const src, CvMat* const vector);
    static void        divVec(CvMat* const src, double value);
    static CvMat*      getColoumn(CvMat* const src, int col);
    static CvMat*      mean(CvMat* const src);
    static CvMat*      multScalar(CvMat* const src, double value);
    static void        printMatrix(CvMat* const src);
    static CvMat*      reshape(CvMat* const src);
    static CvMat*      reverseOrder(CvMat* const src);
    static void        showImage(CvArr* const src, const std::string& title = "Image");
    static void        showImage(const IplImage* const img, const std::vector<Face>& faces, double scale = 1, const std::string& title = "Image");
    static void        sqrVec(CvMat* const src);
    static IplImage*   stringToImage(const std::string& data, int depth, int channels);
    static CvMat*      stringToMatrix(const std::string& data, int type);
    static CvMat*      subtract(CvMat* const src1, CvMat* const src2);
    static double      sumVecToDouble(CvMat* const src);
    static CvMat*      transpose(CvMat* const src);
    static IplImage*   charToIplImage(const char* const img, int width, int height, int step, int depth, int channels);
    static IplImage*   copyRect(const IplImage* const src, const CvRect& rect);
    static IplImage*   scaledSection(const IplImage* const src, const CvRect& sourceRect, double scaleFactor);
    static IplImage*   scaledSection(const IplImage* const src, const CvRect& sourceRect, const CvSize& destSize);

    static std::string imageToString(IplImage* const src);
    static std::string matrixToString(CvMat* const src);
};

} // namespace libface

#endif // _LIBFACEUTILS_H_
