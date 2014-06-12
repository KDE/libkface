/** ===========================================================
 * @file
 *
 * This file is a part of digiKam project
 * <a href="http://www.digikam.org">http://www.digikam.org</a>
 *
 * @date    2012-01-03
 * @brief   Calculates the TanTriggs Preprocessing as described in:
 *          Tan, X., and Triggs, B. "Enhanced local texture feature sets for face
 *          recognition under difficult lighting conditions.". IEEE Transactions
 *          on Image Processing 19 (2010), 1635–650.
 *          Default parameters are taken from the paper.
 * @section DESCRIPTION
 *
 * @author Copyright (C) 2012-2013 by Marcel Wiesweg
 *         <a href="mailto:marcel dot wiesweg at gmx dot de">marcel dot wiesweg at gmx dot de</a>
 * @author Copyright (c) 2012 Philipp Wagner
 *         <a href="mailto:bytefish at gmx dot de">bytefish at gmx dot de</a>
 *
 * @section LICENSE
 *
 * Released to public domain under terms of the BSD Simplified license.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of the organization nor the names of its contributors
 *     may be used to endorse or promote products derived from this software
 *     without specific prior written permission.
 *
 *   See <http://www.opensource.org/licenses/bsd-license>
 *
 * ============================================================ */

#include "tantriggspreprocessor.h"
 
// OpenCV includes

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

namespace KFaceIface
{

TanTriggsPreprocessor::TanTriggsPreprocessor()
    : alpha(0.1f),
      tau(10.0f),
      gamma(0.2f),
      sigma0(1),
      sigma1(2)
{
}

cv::Mat TanTriggsPreprocessor::preprocess(const cv::Mat& inputImage)
{
    return normalize(preprocessRaw(inputImage));
}

cv::Mat TanTriggsPreprocessor::preprocessRaw(const cv::Mat& inputImage)
{
    cv::Mat X = inputImage;

    // ensure it's grayscale
    if (X.channels() > 1)
    {
        cvtColor(X, X, CV_RGB2GRAY);
    }

    // Convert to floating point:
    X.convertTo(X, CV_32FC1);

    // Start preprocessing:
    cv::Mat I;

    // Gamma correction
    cv::pow(X, gamma, I);

    // Calculate the DOG (Difference of Gaussian) Image:
    {
        cv::Mat gaussian0, gaussian1;

        // Kernel Size:
        int kernel_sz0 = (int)(3*sigma0);
        int kernel_sz1 = (int)(3*sigma1);

        // Make them odd for OpenCV:
        kernel_sz0    += ((kernel_sz0 % 2) == 0) ? 1 : 0;
        kernel_sz1    += ((kernel_sz1 % 2) == 0) ? 1 : 0;
        cv::GaussianBlur(I, gaussian0, cv::Size(kernel_sz0,kernel_sz0), sigma0, sigma0, cv::BORDER_CONSTANT);
        cv::GaussianBlur(I, gaussian1, cv::Size(kernel_sz1,kernel_sz1), sigma1, sigma1, cv::BORDER_CONSTANT);
        cv::subtract(gaussian0, gaussian1, I);
    }

    {
        double meanI = 0.0;

        {
            cv::Mat tmp;
            cv::pow(cv::abs(I), alpha, tmp);
            meanI = cv::mean(tmp).val[0];

        }

        I = I / cv::pow(meanI, 1.0/alpha);
    }

    {
        double meanI = 0.0;

        {
            cv::Mat tmp;
            cv::pow(min(abs(I), tau), alpha, tmp);
            meanI = cv::mean(tmp).val[0];
        }

        I = I / cv::pow(meanI, 1.0/alpha);
    }

    // Squash into the tanh:
    {
        for(int r = 0; r < I.rows; r++)
        {
            for(int c = 0; c < I.cols; c++)
            {
                I.at<float>(r,c) = tanh(I.at<float>(r,c) / tau);
            }
        }

        I = tau * I;
    }

    return I;
}

/** Normalizes a given image into a value range between 0 and 255.
 */
cv::Mat TanTriggsPreprocessor::normalize(const cv::Mat& src)
{
    // Create and return normalized image:
    cv::Mat dst;

    switch(src.channels())
    {
        case 1:
            cv::normalize(src, dst, 0, 255, cv::NORM_MINMAX, CV_8UC1);
            break;
        case 3:
            cv::normalize(src, dst, 0, 255, cv::NORM_MINMAX, CV_8UC3);
            break;
        default:
            src.copyTo(dst);
            break;
    }

    return dst;
}

} // namespace KFaceIface
