/** ===========================================================
 * @file
 *
 * This file is a part of digiKam project
 * <a href="http://www.digikam.org">http://www.digikam.org</a>
 *
 * @date   2010-06-21
 * @brief  GUI test program for libkface
 *
 * @author Copyright (C) 2010 by Alex Jironkin
 *         <a href="mailto:alexjironkin at gmail dot com">alexjironkin at gmail dot com</a>
 * @author Copyright (C) 2010 by Aditya Bhatt
 *         <a href="mailto:adityabhatt1991 at gmail dot com">adityabhatt1991 at gmail dot com</a>
 * @author Copyright (C) 2010-2012 by Gilles Caulier
 *         <a href="mailto:caulier dot gilles at gmail dot com">caulier dot gilles at gmail dot com</a>
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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// Qt includes

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QFileDialog>
#include <QLabel>

// libkface includes

#include "database.h"
#include "face.h"
#include "faceitem.h"

namespace Ui
{
    class MainWindow;
}

using namespace KFaceIface;
using namespace std;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    MainWindow(QWidget* const parent = 0);
    ~MainWindow();

public Q_SLOTS:

    void openImage();
    void openConfig();
    void detectFaces();
    void updateConfig();
    void recognise();
    void updateAccuracy();

protected:

    void changeEvent(QEvent* e);

private:

    void clearScene();

private:

    Ui::MainWindow*      ui;
    QGraphicsScene*      myScene;
    QGraphicsView*       myView;
    QGraphicsPixmapItem* lastPhotoItem;
    QList<FaceItem*>     faceitems;

    Database*            database;
    QImage               currentPhoto;
    double               scale;
    QList<Face>          currentFaces;
    QString              lastFileOpenPath;
};

#endif // MAINWINDOW_H
