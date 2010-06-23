/** ===========================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * @date  : 2010-06-21
 * @note  : GUI test program for libkface
 *
 * @author: Copyright (C) 2010 by Aditya Bhatt <adityabhatt1991 at gmail dot com>
 *          Copyright (C) 2010 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *          Copyright (C) 2010 by Alex Jironkin <alexjironkin at gmail dot com>
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

// libkface includes

#include <libkface/database.h>
#include <libkface/kface.h>

// local includes

#include "faceitem.h"

namespace Ui
{
    class MainWindow;
}

using namespace KFace;
using namespace std;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    MainWindow(QWidget* parent = 0);
    ~MainWindow();

public Q_SLOTS:

    void openImage();
    void openConfig();
    void detectFaces();
    void updateConfig();
    void recognise();

protected:

    void changeEvent(QEvent* e);

private:

    void clearScene();

private:

    Ui::MainWindow*      ui;
    QGraphicsScene*      myScene;
    QGraphicsView*       myView;
    QGraphicsPixmapItem* lastPhotoItem;
    Database*            d;

    QImage               currentPhoto;
    double               scale;
    QList<Face>          currentFaces;
};

#endif // MAINWINDOW_H
