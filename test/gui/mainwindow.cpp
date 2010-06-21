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

#include "mainwindow.h"
#include "ui_mainwindow.h"

// Qt includes

#include <QDebug>

using namespace KFace;

MainWindow::MainWindow(QWidget* parent)
          : QMainWindow(parent),
            ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->openImageBtn, SIGNAL(clicked()),
            this, SLOT(openImage()));

    connect(ui->openConfigBtn, SIGNAL(clicked()),
            this, SLOT(openConfig()));

    connect(ui->detectFacesBtn, SIGNAL(clicked()),
            this, SLOT(detectFaces()));

    connect(ui->recogniseBtn, SIGNAL(clicked()),
            this, SLOT(recognise()));

    connect(ui->updateDatabaseBtn, SIGNAL(clicked()),
            this, SLOT(updateConfig()));

    myScene             = new QGraphicsScene();
    QHBoxLayout* layout = new QHBoxLayout;
    myView              = new QGraphicsView(myScene);
    layout->addWidget(myView);

    ui->widget->setLayout(layout);

    myView->setRenderHints(QPainter::Antialiasing);
    myView->show();

    d = new Database(Database::InitAll, QDir::currentPath());

    ui->configLocation->setText(QDir::currentPath());
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::changeEvent(QEvent* e)
{
    QMainWindow::changeEvent(e);
    switch (e->type())
    {
        case QEvent::LanguageChange:
            ui->retranslateUi(this);
            break;
        default:
            break;
    }
}

void MainWindow::openImage()
{
    QString file = QFileDialog::getOpenFileName(this,
            tr("Open Image"), QDir::currentPath(), tr("Image Files (*.png *.jpg *.bmp)"));

    clearScene();

    printf("Opened file - %s\n",file.toAscii().data());

    QPixmap* photo = new QPixmap(file);
    lastPhotoItem  = new QGraphicsPixmapItem(*photo);
    currentPhoto   = photo->toImage();

    if(1.*ui->widget->width()/photo->width() < 1.*ui->widget->height()/photo->height())
    {
        scale = 1.*ui->widget->width()/photo->width();
    }
    else
    {
        scale = 1.*ui->widget->height()/photo->height();
    }

#if QT_VERSION > 0x040503
    lastPhotoItem->setScale(scale);
#else
    lastPhotoItem->scale(scale, scale);
#endif

    myScene->addItem(lastPhotoItem);
}

void MainWindow::openConfig()
{
    QString directory = QFileDialog::getExistingDirectory(this, tr("Select Config Directory"), QDir::currentPath());

    ui->configLocation->setText(directory);

    d = new Database(Database::InitAll, directory);
}

void MainWindow::detectFaces()
{
    currentFaces = d->detectFaces(currentPhoto);
    Face face;

    foreach(face, currentFaces)
    {
        new FaceItem(0, myScene, face.toRect(), scale);
        qDebug() << face.toRect() << " and scale is " << scale << endl;
    }
}

void MainWindow::updateConfig()
{
    d->updateFaces(currentFaces);
    d->saveConfig();
}

void MainWindow::clearScene()
{
    QList<QGraphicsItem*> list = myScene->items();

    int i;

    for(i=0; i<list.size(); i++)
    {
        myScene->removeItem(list.at(i));
    }
}

void MainWindow::recognise()
{
    qDebug() << "Will run MainWindow::recognise()";

    d->recognizeFaces(currentFaces);

    //TODO: create mapping to items.
}
