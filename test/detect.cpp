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
#include <QImage>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QHBoxLayout>
#include <QLabel>
#include <QPixmap>
#include <QWidget>

// KDE includes

#include <kdebug.h>

// libkface includes

#include "libkface/database.h"
#include "libkface/face.h"

using namespace KFaceIface;

void detectFaces(Database* const d, const QString& file)
{
    kDebug() << "Loading" << file;
    QImage img(file);
    kDebug() << "Detecting";
    QList<Face> result = d->detectFaces(img);//QString::fromLocal8Bit(argv[1]));
    kDebug() << "Detected";

    if (result.isEmpty())
    {
        kDebug() << "No faces found";
        return;
    }

    kDebug() << "Coordinates of detected faces : ";
    foreach(const Face& f, result)
    {
        QRect r = f.toRect();
        kDebug() << r;
    }

    QWidget* mainWidget = new QWidget;
    mainWidget->setWindowTitle(file);
    QHBoxLayout* layout = new QHBoxLayout(mainWidget);
    QLabel* fullImage   = new QLabel;
    fullImage->setPixmap(QPixmap::fromImage(img.scaled(250, 250, Qt::KeepAspectRatio)));
    layout->addWidget(fullImage);

    foreach(const Face& f, result)
    {
        QLabel* label = new QLabel;
        label->setScaledContents(false);
        QImage part   = img.copy(f.toRect());
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
        kDebug() << "Bad Args!!!\nUsage: " << argv[0] << " <image1> <image2> ...";
        return 0;
    }

    // Make a new instance of Database and then detect faces from the image
    kDebug() << "Making DB";
    Database* d = new Database(Database::InitDetection, QString("."));

    QApplication app(argc, argv);
    for (int i=1; i<argc; i++)
    {
        detectFaces(d, QString::fromLocal8Bit(argv[i]));
    }
    app.exec();

    return 0;
}
