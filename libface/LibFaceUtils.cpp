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
 * @author Copyright (C) 2010 by Gilles Caulier
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

#include <iostream>
#include <cmath>

#include "LibFaceUtils.h"

using namespace std;

namespace libface
{

/**
 * This takes the input image and returns a new image of a specified pixel count (area),
 * while preserving the aspect ratio.
 *
 * @param img The input image
 * @param area The desired area of the resized image
 * @param ratio The scaling factor, to be passed as reference
 * @return The resized image
 */
IplImage* LibFaceUtils::resizeToArea(const IplImage* img, int area, double& ratio)
{
    // Area of input image
    int W = img->width;
    int H = img->height;

    /*
     We want an area of A pixels in the output image - that should be analyzable
     Suppose width and height of input image are W and H. And we must divide both by same factor z.
     Then, (W/z) * (H/z) = A is what we want.
     So, z = sqrt(W*H/A);
     */

    CvSize s;
    double z      = sqrt((double)(W) * (double)(H) / (double)(area));
    s.width       = (int)(W/z);
    s.height      = (int)(H/z);
    ratio         = z;
    IplImage* out = cvCreateImage(s, img->depth, img->nChannels);
    cvResize(img, out);

    return out;
}

/**
 * Returns a CvPoint that is the center coordinate of a face object.
 *
 * @param f The face object whose center will be returned
 * @return The center of the face
 */
CvPoint LibFaceUtils::center(const Face& f)
{
    CvPoint p;    // Center of face 1
    p.x = (f.getX1() + f.getX2())/2;
    p.y = (f.getY1() + f.getY2())/2;

    return p;
}

/**
 * Returns the integer-rounded distance between two points.
 *
 * @param p1 The first CvPoint
 * @param p2 The second CvPoint
 * @return The distance between the two points
 */
int LibFaceUtils::distance(CvPoint p1, CvPoint p2)
{
    return (int)sqrt( (double)(((p1.x - p2.x)*(p1.x - p2.x)) + ((p1.y - p2.y)*(p1.y - p2.y))) );    // Euclidean distance
}

/**
 * Returns the integer-rounded distance between the centers of two faces.
 *
 * @param f1 The first Face object
 * @param f2 The second Face object
 * @return The distance between the two Faces
 */
int LibFaceUtils::distance(const Face& f1, const Face& f2)
{
    CvPoint p1 = center(f1);
    CvPoint p2 = center(f2);

    return distance(p1,p2);
}

/**
 * Method for reshaping a matrix into a single coloumn vector.
 *
 * @param src A pointer to the matrix to be reshaped.
 *
 * @return Returns the pointer to the reshaped vector.
 */
CvMat* LibFaceUtils::reshape(CvMat* src)
{
    int i, j, t = 0;
    CvMat* row  = cvCreateMat(src->rows*src->cols, 1, src->type);

    for (i=0; i < src->rows; ++i)
    {
        for (j=0; j < src->cols; ++j)
        {
            cvSet1D(row, t, cvGet2D(src, i, j));
            t++;
        }
    }

    return row;
}

/**
 * Method for transposing a matrix.
 *
 * @param src A Pointer to the matrix.
 *
 * @return Returns a pointer to the transposed matrix.
 */
CvMat* LibFaceUtils::transpose(CvMat* src)
{
    CvMat* result = cvCreateMat(src->cols, src->rows, src->type);
    cvTranspose(src, result);

    return result;
}

/**
 * Method for adding an extra column to a matrix. This method makes no checks on validity
 * of the data.
 *
 * @param src A pointer to a matrix to be added to.
 * @param vector A pointer to a vector to be added to the matrix. Needs to have the same number of rows.
 *
 * @return Returns a pointer to the new matrix with added column.
 */
CvMat* LibFaceUtils::combine(CvMat* src, CvMat* vector)
{
    int i, j;
    CvMat* result = cvCreateMat(src->rows, src->cols + 1, src->type);

    for (i = 0; i < src->rows; ++i)
    {
        for (j = 0; j <= src->cols; ++j)
        {
            if (j != src->cols)
                cvSet2D(result, i, j, cvGet2D(src, i, j));
            else
                cvSet2D(result, i, j, cvGet1D(vector, i));
        }
    }

    return result;
}

/**
 * Method for reversing order of the elements in a matrix. Only columns are reversed, rows are left in tact.
 *
 * @param src A pointer to a matrix to have columns reversed.
 *
 * @return Returns a pointer to the new matrix.
 */
CvMat* LibFaceUtils::reverseOrder(CvMat* src)
{
    int j, i;
    CvMat* result = cvCreateMat(src->rows, src->cols, src->type);

    for (j = src->cols - 1; j >= 0; j--)
    {
        for (i = 0; i < src->rows; ++i)
        {
            cvSet2D(result, i, src->cols - j - 1, cvGet2D(src, i, j));
        }
    }

    return result;
}

/**
 * A method for squaring every element in a matrix. The matrix will be modified in the process.
 *
 * @param src A pointer to the matrix where every element will be squared.
 *
 */
void LibFaceUtils::sqrVec(CvMat* src)
{
    int i, j;

    for (i = 0; i < src->rows; ++i)
    {
        for (j = 0; j < src->cols; ++j)
        {
            double value = cvGet2D(src, i, j).val[0];
            double sqr   = value * value;

            cvSet2D(src, i, j, cvScalarAll(sqr));
        }
    }
}

/**
 * Method for summing all the rows in a vector to a value of type double.
 *
 * @param src A pointer to a vector to be summed.
 *
 * @return Returns sum of all rows in a vector as a double.
 */
double LibFaceUtils::sumVecToDouble(CvMat* src)
{
    double sum = 0;
    int i;

    for (i = 0; i < src->rows; ++i)
    {
        sum = sum + cvGet1D(src, i).val[0];
    }

    return sum;
}

/**
 *   Method for dividing every element by some value. Matrix is modified in process.
 *
 *  @param src A pointer to a matrix.
 *  @param value A value to be divided by.
 */
void LibFaceUtils::divVec(CvMat* src, double value)
{
    int i, j;

    for (i = 0; i < src->rows; ++i)
    {
        for (j = 0; j < src->cols; ++j)
        {
            double val = cvGet2D(src, i, j).val[0];
            double res = val / value;

            cvSet2D(src, i, j, cvScalarAll(res));
        }
    }
}

/**
 *  Method for printing matrix.
 *
 *  @param src A pointer to a matrix.
 */
void LibFaceUtils::printMatrix(CvMat* src)
{
    int i, j;

    for (i = 0; i < src->rows; ++i)
    {
        for (j = 0; j < src->cols; ++j)
        {
            printf("%f \t", cvGet2D(src, i, j).val[0]);
        }
        printf("\n");
    }
}

/**
 * Method for multiplying every element by a single value. New matrix is created and returned.
 */
CvMat* LibFaceUtils::multScalar(CvMat* src, double value)
{
    CvMat* result = cvCreateMat(src->rows, src->cols, src->type);

    int i, j;

    for (i = 0; i < src->rows; ++i)
    {
        for (j = 0; j < src->cols; ++j)
        {
            double value1 = cvGet2D(src, i, j).val[0];
            double res    = value1 * value;

            cvSet2D(result,i,j,cvScalarAll(res));
        }
    }

    return result;
}

/**
 *   Method for calculating mean value for every row.
 *
 *  @param src A pointer to a matrix.
 *
 *  @return Returns a pointer to the matrix containing means for every row.
 */
CvMat* LibFaceUtils::mean(CvMat* src)
{
    int    i, j;
    double mean;
    CvMat* result = cvCreateMat(src->rows, 1, src->type);

    for (i = 0; i < src->rows; ++i)
    {
        mean = 0;
        for (j = 0; j < src->cols; ++j)
        {
            mean = mean + cvGet2D(src, i, j).val[0];
        }
        mean = mean / src->cols;

        cvSet1D(result, i, cvScalarAll(mean));
    }

    return result;
}

/**
 *  Method for converting from string to image format. The data in the columns is
 *  separated by "\t" rows are separated by "\n".
 *
 *  @param data - String that contains the delimited data.
 *  @param depth - depth for the image to be.
 *  @param channels - Numbers of channels in the image.
 *
 *  @return Returns the pointer to IplImage with the values contained in the string.
 */
IplImage* LibFaceUtils::stringToImage(const string& data, int depth, int channels)
{
    std::string::size_type start, stop;
    start    = 0;
    stop     = data.find('\n', start);
    int cols = 0, rows = 0;
    vector<double> values;

    //First figure out how many rows and cols there are and store data in vector
    while (stop != std::string::npos)
    {
        string copy(data, start, stop);
        int extract = 1;

        cols = 0;

        while(extract)
        {
            std::string::size_type startIndex = copy.find("\t");

            string value = copy.substr(0, startIndex);

            copy         = copy.substr(startIndex+1);

            values.insert(values.end(),atof(value.c_str()));

            if(copy == "")
                    extract = 0;

            cols++;
        }

        start = stop + 1;
        stop  = data.find('\n', start);
        rows++;
    }

    IplImage* img = cvCreateImage(cvSize(rows, cols), depth, channels);
    int i, j;

    //Create matrix and transfer data to the matrix.
    for(i=0 ; i < rows ; ++i)
    {
        for(j=0;j<cols;++j)
        {
            cvSetAt(img,cvScalarAll(values.at(cols*i+j)),i,j);
        }
    }

    return img;
}

/**
 *  Method for converting from string to matrix format. The data in the columns is
 *  separated by "\t" rows are separated by "\n".
 *
 *  @param data - String that contains the delimited data.
 *  @param type - type for the matrix to be.
 *
 *  @return Returns the pointer matrix with the values contained in the string.
 */
CvMat* LibFaceUtils::stringToMatrix(const string & data, int type)
{
    std::string::size_type start, stop;
    start    = 0;
    stop     = data.find('\n', start);
    int cols = 0, rows = 0;
    vector<double> values;

    //First figure out how many rows and cols there are and store data in vector
    while (stop != std::string::npos)
    {
        string copy(data, start, stop);
        int extract = 1;
        cols        = 0;

        while(extract)
        {
            std::string::size_type startIndex = copy.find("\t");
            string value                      = copy.substr(0, startIndex);
            copy                              = copy.substr(startIndex+1);

            values.insert(values.end(), atof(value.c_str()));

            if(copy == "")
                extract = 0;

            cols++;
        }

        start = stop + 1;
        stop  = data.find('\n', start);
        rows++;
    }

    CvMat* matrix = cvCreateMat(rows, cols, type);
    int i, j;

    //Create matrix and transfer data to the matrix.
    for(i=0 ; i < rows ; ++i)
    {
        for(j=0 ; j<cols ; ++j)
        {
            cvSetAt(matrix, cvScalarAll(values.at(cols*i+j)), i, j);
        }
    }

    return matrix;
}

/**
 * Simple utility method for displaying matrices. Only useful if matrix represents an image
 *
 * @param src A pointer to the matrix to be displayed.
 */
void LibFaceUtils::showImage(CvArr* src, const string& title)
{
    cvNamedWindow(title.data(),CV_WINDOW_AUTOSIZE);
    cvShowImage(title.data(), src);
    cvWaitKey(0);
    cvDestroyWindow(title.data());
}

/**
 * Simple utility method for displaying images and drawing faces on them.
 *
 * @param img Pointer to IplImage to be displayed.
 * @param faces vector of Face's to be drawn on the image.
 */
void LibFaceUtils::showImage(const IplImage* img, const vector<Face>& faces, double scale, const string& title)
{
    IplImage* copy = cvCloneImage(img);

    for (unsigned int i = 0; i < faces.size(); ++i)    // Draw squares over detected faces
    {
        cvRectangle( copy,
                     cvPoint((int)(faces[i].getX1()/scale),
                             (int)(faces[i].getY1()/scale)),
                     cvPoint((int)(faces[i].getX2()/scale),
                             (int)(faces[i].getY2()/scale)),
                     CV_RGB(255, 0, 0), 3, 1, 0);
    }

    cvNamedWindow(title.data(), CV_WINDOW_AUTOSIZE);
    cvShowImage(title.data(), copy);
    cvWaitKey(0);
    cvDestroyWindow(title.data());
}

/**
 *  Method for converting image(greyscale, 1 channel) into a string. The data in coloumns is separated by "\t"
 *  and rows are separated by "\n".
 *
 *  @param img - A pointer to the IplImage to be converted to string.
 *
 *  @return String is returned containing the data of the image.
 */
string LibFaceUtils::imageToString(IplImage* img)
{
    stringstream matrix;
    int          i, j;

    for(i=0 ; i < img->height ; ++i)
    {
        for(j=0 ; j < img->width ; ++j)
        {
            string data;
            matrix << cvGet2D(img, i, j).val[0] << "\t";
        }
        matrix << "\n";
    }

    return matrix.str();
}

/**
 *  Method for converting matrix into a string. The data in coloumns is separated by "\t"
 *  and rows are separated by "\n".
 *
 *  @param src - A pointer to the matrix to be converted to string.
 *
 *  @return String is returned containing the data of the matrix.
 */
string LibFaceUtils::matrixToString(CvMat* src)
{
    stringstream matrix;
    int          i, j;

    for(i=0 ; i < src->rows ; ++i)
    {
        for(j=0 ; j < src->cols ; ++j)
        {
            string data;
            matrix << cvmGet(src, i, j) << "\t";
        }
        matrix << "\n";
    }

    return matrix.str();
}

/**
 *  A method for subtrating a vector from a matrix. New matrix is created in process.
 *
 *  @param src1 A pointer to a matrix to be subtracted from.
 *  @param src2 A pointer to a vector to be subtracted.
 *
 *  @return Returns a pointer to the new matrix.
 */
CvMat* LibFaceUtils::subtract(CvMat* src1, CvMat* src2)
{
    int    i, j;
    CvMat* result = cvCreateMat(src1->rows, src1->cols, src1->type);

    for (i = 0 ; i < src1->rows ; ++i)
    {
        for (j=0 ; j < src1->cols ; ++j)
        {
            double value1 = cvGet2D(src1, i, j).val[0];
            double value2 = cvGet1D(src2, i).val[0];
            double res    = value1 - value2;

            cvSet2D(result, i, j, cvScalarAll(res));
        }
    }
    return result;
}

/**
 *  Method for accessing a coloumn from a matrix.
 *
 *  @param src A pointer to a matrix.
 *  @param id of coloumn to be returned.
 *
 *  @return Returns the pointer to the coloumn.
 */
CvMat* LibFaceUtils::getColoumn(CvMat* src, int col)
{
    int i;
    CvMat* result = cvCreateMat(src->rows, 1, src->type);

    for (i = 0 ; i < src->rows ; ++i)
    {
        cvSet1D(result, i, cvGet2D(src, i, col));
    }

    return result;
}

/**
 * A method for adding another element to the end of a vector.
 *
 * @param src A pointer to a matrix.
 * @param value Value to be appended to the end.
 *
 * @return Returns a pointer to the new vector.
 */
CvMat* LibFaceUtils::addScalar(CvMat* src, CvScalar value)
{
    CvMat* result = cvCreateMat(src->rows+1,1,src->type);
    int i;

    for (i=0 ; i < src->rows ; ++i)
    {
        cvSet1D(result, i, cvGet1D(src, i));
    }

    cvSet1D(result, src->rows,value);

    return result;
}

IplImage* LibFaceUtils::charToIplImage(const char* img, int width, int height, int step, int depth, int channels)
{
    cout << "Inside charToIplImage" << endl;

    IplImage* imgHeader  = cvCreateImageHeader(cvSize(width, height), depth, channels);
    imgHeader->widthStep = step;

    cout << "made header" << endl;

    imgHeader->imageData = (char *)img;

    cout << "loaded data" << endl;
    //cvNamedWindow("a");
    //cvShowImage("a", imgHeader);
    //cvWaitKey(0);
    //cvDestroyWindow("a");

    cout << "returning" << endl;
    return imgHeader;
}

IplImage* LibFaceUtils::copyRect(const IplImage* src, const CvRect& rect)
{
    // Create a non-const header to set the ROI
    IplImage* srcHeader  = cvCreateImageHeader(cvSize(src->width, src->height), src->depth, src->nChannels);
    srcHeader->imageData = const_cast<char*>(src->imageData);
    cvSetImageROI(srcHeader, rect);

    IplImage* result = cvCreateImage(cvSize(rect.width, rect.height), src->depth, src->nChannels);
    cvCopy(srcHeader, result, NULL);

    cvReleaseImageHeader(&srcHeader);
    return result;
}

IplImage* LibFaceUtils::scaledSection(const IplImage* src, const CvRect& sourceRect, double scaleFactor)
{
    if (scaleFactor == 1.0)
        return copyRect(src, sourceRect);

    return scaledSection(src, sourceRect, cvSize(
                         lround(double(sourceRect.width) * scaleFactor),
                         lround(double(sourceRect.height) * scaleFactor) )
                        );
}

IplImage* LibFaceUtils::scaledSection(const IplImage* src, const CvRect& sourceRect, const CvSize& destSize)
{
    if (destSize.width == sourceRect.width && destSize.height == sourceRect.height)
        return copyRect(src, sourceRect);

    // Create a non-const header to set the ROI
    IplImage* srcHeader  = cvCreateImageHeader(cvSize(src->width, src->height), src->depth, src->nChannels);
    srcHeader->imageData = const_cast<char*>(src->imageData);
    cvSetImageROI(srcHeader, sourceRect);

    IplImage* result = cvCreateImage(cvSize(destSize.width, destSize.height), src->depth, src->nChannels);
    cvResize(srcHeader, result);

    cvReleaseImageHeader(&srcHeader);
    return result;
}

} // namespace libface
