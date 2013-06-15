/** ===========================================================
 * @file
 *
 * This file is a part of digiKam project
 * <a href="http://www.digikam.org">http://www.digikam.org</a>
 *
 * @date   2010-06-16
 * @brief  The Database class wraps the libface database
 *
 * @author Copyright (C) 2010 by Aditya Bhatt
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

// Qt includes

#include <QApplication>
#include <QDir>
#include <QImage>
#include <QTime>

// OpenCV includes

#include "libopencv.h"

// KDE includes

#include <kdebug.h>

// libkface includes

#include "libkface/alignment-congealing/funnelreal.h"

// it's not exported...
#include "libkface/alignment-congealing/funnelreal.cpp"

using namespace KFaceIface;

QStringList toPaths(char** argv, int startIndex, int argc)
{
    QStringList files;

    for (int i=startIndex; i<argc; i++)
    {
        files << QString::fromLocal8Bit(argv[i]);
    }

    return files;
}

QList<cv::Mat> toImages(const QStringList& paths)
{
    QList<cv::Mat> images;

    foreach (const QString& path, paths)
    {
        QByteArray s = path.toLocal8Bit();
        images << cv::imread(std::string(s));
    }

    return images;
}


// --------------------------------------------------------------------------------------------------

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        kDebug() << "Bad Args!!!\nUsage: " << argv[0] << " align <image1> <image2> ... ";
        return 0;
    }

    QApplication app(argc, argv);

    QStringList paths     = toPaths(argv, 1, argc);
    QList<cv::Mat> images = toImages(paths);

    QTime time;
    time.start();
    FunnelReal funnel;
    kDebug() << "Setup of FunnelReal took" << time.restart();

    const int uiSize = 200;
    cv::Mat bigImage = cv::Mat::zeros(uiSize*images.size(), 2*uiSize, CV_8UC3);
    qDebug() << bigImage.cols << bigImage.rows;
    for (int i=0; i<images.size(); ++i)
    {
        const cv::Mat& image = images[i];
        cv::Mat aligned = funnel.align(image);

        QSize size(image.cols, image.rows);
        size.scale(uiSize, uiSize, Qt::KeepAspectRatio);
        qDebug() << size;
        cv::Size scaleSize(size.height(), size.width());

        const int top = i*uiSize;
        cv::Mat scaledOriginal, scaledResult;
        cv::resize(image, scaledOriginal, scaleSize);
        cv::resize(aligned, scaledResult, scaleSize);

        scaledOriginal.copyTo(bigImage.colRange(0, scaledOriginal.cols).rowRange(top, top+scaledOriginal.rows));
        scaledResult.copyTo  (bigImage.colRange(uiSize, uiSize+scaledResult.cols).rowRange(top, top+scaledResult.rows));
    }

    int elapsed = time.elapsed();
    kDebug() << "Alignment took" << elapsed << "for" << images.size() << "," << ((float)elapsed/images.size()) << "per image";

    cv::namedWindow("images");
    cv::imshow("images", bigImage);
    app.exec();

    return 0;
}
