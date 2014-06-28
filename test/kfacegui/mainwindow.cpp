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
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QFileDialog>
#include <QLabel>

// KDE include

#include <kdebug.h>
#include <kfiledialog.h>

// libkface includes

#include "recognitiondatabase.h"
#include "facedetector.h"
#include "faceitem.h"

using namespace std;
using namespace KFaceIface;

class MainWindow::Private
{
public:

    Private()
    {
        ui            = 0;
        myScene       = 0;
        myView        = 0;
        lastPhotoItem = 0;
        detector      = 0;
        scale         = 0.0;
    }

    Ui::MainWindow*      ui;
    QGraphicsScene*      myScene;
    QGraphicsView*       myView;
    QGraphicsPixmapItem* lastPhotoItem;
    QList<FaceItem*>     faceitems;

    RecognitionDatabase  database;
    FaceDetector*        detector;
    QImage               currentPhoto;
    double               scale;
    QList<QRectF>        currentFaces;
    QString              lastFileOpenPath;
};

MainWindow::MainWindow(QWidget* const parent)
    : QMainWindow(parent), d(new Private)
{
    d->ui = new Ui::MainWindow;
    d->ui->setupUi(this);
    d->ui->recogniseBtn->setEnabled(false);
    d->ui->updateDatabaseBtn->setEnabled(false);
    d->ui->detectFacesBtn->setEnabled(false);
    d->ui->configLocation->setReadOnly(true);

    connect(d->ui->openImageBtn, SIGNAL(clicked()),
            this, SLOT(slotOpenImage()));

    connect(d->ui->horizontalSlider, SIGNAL(valueChanged(int)),
            this, SLOT(slotUpdateAccuracy()));

    connect(d->ui->detectFacesBtn, SIGNAL(clicked()),
            this, SLOT(slotDetectFaces()));

    connect(d->ui->openDatabaseBtn, SIGNAL(clicked()),
            this, SLOT(slotOpenDatabase()));

    connect(d->ui->recogniseBtn, SIGNAL(clicked()),
            this, SLOT(slotRecognise()));

    connect(d->ui->updateDatabaseBtn, SIGNAL(clicked()),
            this, SLOT(slotUpdateDatabase()));

    d->myScene                = new QGraphicsScene();
    QGridLayout* const layout = new QGridLayout;
    d->myView                 = new QGraphicsView(d->myScene);

    d->myView->setCacheMode(QGraphicsView::CacheBackground);
    d->myScene->setItemIndexMethod(QGraphicsScene::NoIndex);

    setMouseTracking(true);
    layout->addWidget(d->myView);

    d->ui->widget->setLayout(layout);

    d->myView->show();

    d->detector = new FaceDetector();

    d->ui->configLocation->setText(QDir::currentPath());
    d->ui->horizontalSlider->setValue(80);

    d->lastFileOpenPath = QDir::currentPath();
}

MainWindow::~MainWindow()
{
    delete d->ui;
    delete d->detector;
    delete d;
}

void MainWindow::changeEvent(QEvent* e)
{
    QMainWindow::changeEvent(e);

    switch (e->type())
    {
        case QEvent::LanguageChange:
            d->ui->retranslateUi(this);
            break;
        default:
            break;
    }
}

void MainWindow::clearScene()
{
    QList<QGraphicsItem*> list = d->myScene->items();

    for(int i=0; i<list.size(); i++)
    {
        d->myScene->removeItem(list.at(i));
    }
}

void MainWindow::slotOpenImage()
{
    QString file = KFileDialog::getOpenFileName(
            d->lastFileOpenPath,
            "Image Files (*.png *.jpg *.bmp *.pgm)",
            this,
            "Open Image");

    if (file.isEmpty())
        return;

    d->lastFileOpenPath = QFileInfo(file).absolutePath();

    clearScene();

    kDebug() << "Opened file " << file.toAscii().data();

    QPixmap* const photo = new QPixmap(file);
    d->lastPhotoItem     = new QGraphicsPixmapItem(*photo);
    d->currentPhoto      = photo->toImage();

    if (1.0 * d->ui->widget->width() / photo->width() < 1.0 * d->ui->widget->height() / photo->height())
    {
        d->scale = 1.0 * d->ui->widget->width() / photo->width();
    }
    else
    {
        d->scale = 1.0 * d->ui->widget->height() / photo->height();
    }

    d->lastPhotoItem->setScale(d->scale);

    d->myScene->addItem(d->lastPhotoItem);
    d->ui->detectFacesBtn->setEnabled(true);
}

void MainWindow::slotDetectFaces()
{
    d->currentFaces.clear();
    d->currentFaces = d->detector->detectFaces(d->currentPhoto);

    kDebug() << "libkface detected : " << d->currentFaces.size() << " faces.";
    kDebug() << "Coordinates of detected faces : ";

    foreach(const QRectF& r, d->currentFaces)
    {
        kDebug() << r;
    }

    foreach(FaceItem* const item, d->faceitems)
        item->setVisible(false);

    d->faceitems.clear();
    QRect face;

    for(int i = 0; i < d->currentFaces.size(); ++i)
    {
        face = d->detector->toAbsoluteRect(d->currentFaces[i], d->currentPhoto.size());
        d->faceitems.append(new FaceItem(0, d->myScene, face, d->scale));
        kDebug() << face;
    }
}

void MainWindow::slotUpdateAccuracy()
{
    int value = d->ui->horizontalSlider->value();
    d->ui->lcdNumber->display(value);
    d->detector->setParameter("accuracy", value/100.0);
}

void MainWindow::slotOpenDatabase()
{
    QString directory = KFileDialog::getExistingDirectory(
            QDir::currentPath(),
            this,
            "Select Database Directory");

    d->ui->configLocation->setText(directory);

    d->database = RecognitionDatabase::addDatabase(directory);
    d->ui->recogniseBtn->setEnabled(true);
    d->ui->updateDatabaseBtn->setEnabled(true);
}

void MainWindow::slotRecognise()
{
    setCursor(Qt::WaitCursor);

    int i = 0;

    foreach(FaceItem* const item, d->faceitems)
    {
        Identity identity = d->database.recognizeFace(d->currentPhoto.copy(item->originalRect()));

        if (!identity.isNull())
        {
            item->suggest(identity.attributes["fullName"]);

            kDebug() << "Face #"<< i+1 << " is closest to the person with ID " << identity.id
                     << " and name "<< identity.attributes["fullName"];
        }

        i++;
    }

    unsetCursor();
}

// TODO: port these method to FaceRecognitionDatabase API

void MainWindow::slotUpdateDatabase()
{
/*
    kDebug() << "Path of config directory = " << d->database->configPath();

    // Assign the text of the d->faceitems to the name of each face. When there is no text, drop that face from currentfaces.
    QList<QRectF> updateList;

    for(int i = 0 ; i <d->currentFaces.size(); ++i)
    {
        if (d->faceitems[i]->text() != "?")
        {
            d->currentFaces[i].setName(d->faceitems[i]->text());
            updateList.append(d->currentFaces.at(i));
        }
    }

    if( d->database->updateFaces(updateList) )
    {
        kDebug() << "Trained";
        d->database->saveConfig();
    }
    else
    {
        kDebug() << "No faces to train.";
    }
*/
}
