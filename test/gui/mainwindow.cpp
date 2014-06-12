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
 *         Copyright (C) 2010 by Aditya Bhatt
 *         <a href="mailto:adityabhatt1991 at gmail dot com">adityabhatt1991 at gmail dot com</a>
 *         Copyright (C) 2010-2014 by Gilles Caulier
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

#include "mainwindow.moc"
#include "ui_mainwindow.h"

// Qt includes

#include <QLayout>
#include <QFormLayout>

// KDE include

#include <kdebug.h>
#include <kfiledialog.h>

using namespace KFaceIface;

MainWindow::MainWindow(QWidget* const parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow)
{
    lastPhotoItem = 0;
    scale         = 0.0;

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

    connect(ui->horizontalSlider, SIGNAL(valueChanged(int)),
            this, SLOT(updateAccuracy()));


    myScene                   = new QGraphicsScene();
    QGridLayout* const layout = new QGridLayout;
    myView                    = new QGraphicsView(myScene);

    myView->setCacheMode(QGraphicsView::CacheBackground);
    myScene->setItemIndexMethod(QGraphicsScene::NoIndex);

    setMouseTracking(true);
    layout->addWidget(myView);

    ui->widget->setLayout(layout);

    myView->show();

    detector = new FaceDetector();
    database = RecognitionDatabase::addDatabase(QDir::currentPath());

    ui->configLocation->setText(QDir::currentPath());
    ui->horizontalSlider->setValue(80);

    lastFileOpenPath = QDir::currentPath();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete detector;
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
    QString file = KFileDialog::getOpenFileName(
            lastFileOpenPath,
            "Image Files (*.png *.jpg *.bmp *.pgm)",
            this,
            "Open Image");

    if (file.isEmpty())
        return;

    lastFileOpenPath = QFileInfo(file).absolutePath();

    clearScene();

    kDebug() << "Opened file " << file.toAscii().data();

    QPixmap* const photo = new QPixmap(file);
    lastPhotoItem        = new QGraphicsPixmapItem(*photo);
    currentPhoto         = photo->toImage();

    if (1.0 * ui->widget->width() / photo->width() < 1.0 * ui->widget->height() / photo->height())
    {
        scale = 1.0 * ui->widget->width() / photo->width();
    }
    else
    {
        scale = 1.0 * ui->widget->height() / photo->height();
    }

    lastPhotoItem->setScale(scale);

    myScene->addItem(lastPhotoItem);
}

void MainWindow::detectFaces()
{
    currentFaces.clear();
    currentFaces = detector->detectFaces(currentPhoto);

    kDebug() << "libkface detected : " << currentFaces.size() << " faces.";
    kDebug() << "Coordinates of detected faces : ";

    foreach(const QRectF& r, currentFaces)
    {
        kDebug() << r;
    }

    foreach(FaceItem* const item, faceitems)
        item->setVisible(false);

    faceitems.clear();
    QRect face;

    for(int i = 0; i < currentFaces.size(); ++i)
    {
        face = detector->toAbsoluteRect(currentFaces[i], currentPhoto.size());
        faceitems.append(new FaceItem(0, myScene, face, scale));
        kDebug() << face;
    }
}

void MainWindow::updateAccuracy()
{
    int value = ui->horizontalSlider->value();
    ui->lcdNumber->display(value);
    detector->setParameter("accuracy", value/100.0);
}

void MainWindow::clearScene()
{
    QList<QGraphicsItem*> list = myScene->items();

    for(int i=0; i<list.size(); i++)
    {
        myScene->removeItem(list.at(i));
    }
}

void MainWindow::openConfig()
{
/*
    QString directory = KFileDialog::getExistingDirectory(
            QDir::currentPath(),
            this,
            "Select Config Directory");

    ui->configLocation->setText(directory);

    database = new Database(directory);
*/
}

void MainWindow::updateConfig()
{
/*
    kDebug() << "Path of config directory = " << database->configPath();

    // Assign the text of the faceitems to the name of each face. When there is no text, drop that face from currentfaces.
    QList<QRectF> updateList;

    for(int i = 0 ; i <currentFaces.size(); ++i)
    {
        if (faceitems[i]->text() != "?")
        {
            currentFaces[i].setName(faceitems[i]->text());
            updateList.append(currentFaces.at(i));
        }
    }

    if( database->updateFaces(updateList) )
    {
        kDebug() << "Trained";
        database->saveConfig();
    }
    else
    {
        kDebug() << "No faces to train.";
    }
*/
}

void MainWindow::recognise()
{
/*
    QList<double> closeness = database->recognizeFaces(currentFaces);

    if (closeness.isEmpty())
        return;

    for(int i = 0; i < currentFaces.size(); ++i)
    {
        faceitems[i]->suggest(currentFaces[i].name());
        kDebug() << "Face #"<< i+1 << " is closest to the person with ID " << currentFaces[i].id()
                 << " and name "<< currentFaces[i].name()
                 << " with a distance of "<< closeness[i];
    }
*/
}
