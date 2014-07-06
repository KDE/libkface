/** ===========================================================
 * @file
 *
 * This file is a part of digiKam project
 * <a href="http://www.digikam.org">http://www.digikam.org</a>
 *
 * @date   2010-06-16
 * @brief  Face Detection CLI tool
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
#include <QImage>
#include <QHBoxLayout>
#include <QLabel>
#include <QPixmap>
#include <QWidget>

// KDE includes

#include <kdebug.h>

// libkface includes

#include "libkface/facedetector.h"

using namespace KFaceIface;

void detectFaces(const QString& file)
{
    kDebug() << "Loading" << file;
    QImage img(file);
    kDebug() << "Detecting";
    FaceDetector detector;
    QList<QRectF> faces = detector.detectFaces(img);
    kDebug() << "Detected";

    if (faces.isEmpty())
    {
        kDebug() << "No faces found";
        return;
    }

    kDebug() << "Coordinates of detected faces : ";

    foreach(const QRectF& r, faces)
    {
        kDebug() << r;
    }

    QWidget* const mainWidget = new QWidget;
    mainWidget->setWindowTitle(file);
    QHBoxLayout* const layout = new QHBoxLayout(mainWidget);
    QLabel* const fullImage   = new QLabel;
    fullImage->setPixmap(QPixmap::fromImage(img.scaled(250, 250, Qt::KeepAspectRatio)));
    layout->addWidget(fullImage);

    foreach(const QRectF& rr, faces)
    {
        QLabel* const label = new QLabel;
        label->setScaledContents(false);
        QRect r             = FaceDetector::toAbsoluteRect(rr, img.size());
        QImage part         = img.copy(r);
        label->setPixmap(QPixmap::fromImage(part.scaled(200, 200, Qt::KeepAspectRatio)));
        layout->addWidget(label);
    }

    mainWidget->show();
    qApp->processEvents(); // dirty hack
}

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        kDebug() << "Bad Arguments!!!\nUsage: " << argv[0] << " <image1> <image2> ...";
        return 0;
    }

    QApplication app(argc, argv);

    for (int i = 1 ; i < argc ; i++)
    {
        detectFaces(QString::fromLocal8Bit(argv[i]));
    }

    app.exec();

    return 0;
}
