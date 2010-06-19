/** ===========================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * @date  : 2010-06-16
 * @note  : The Database class wraps the libface database
 *
 * @author: Copyright (C) 2010 by Aditya Bhatt <adityabhatt1991 at gmail dot com>
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
#include <QDebug>
#include <QGraphicsPixmapItem>

// Local includes

#include "libkface/database.h"
#include "libkface/kface.h"

using namespace KFace;

int main(int argc, char** argv)
{
    if (argc<2)
    {
        qDebug() << "Bad Args!!!\nUsage: " << argv[0] << " <image1> <image2> ...";
        return 0;
    }

    QImage img(argv[1]);

    // Make a new instance of Database and then detect faces from the image
    qDebug() << "Making DB";
    Database* d        = new Database(Database::InitDetection, QString("."));
    qDebug() << "Detecting";
    QList<Face> result = d->detectFaces(img);
    qDebug() << "Detected";


    qDebug() << "Coordinates of detected faces : ";
    Face f;
    foreach(f, result)
    {
	QRect r = f.toRect();
	qDebug() << "(" << r.topLeft().x() << "," << r.topLeft().y() << ");" 
	         << "(" << r.bottomRight().x() << "," << r.bottomRight().y() << ")";
    }

    return 0;
}
