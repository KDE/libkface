/** ===========================================================
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

#include "faceitem.moc"

// Qt includes

#include <QWidget>
#include <QPainter>
#include <QGraphicsOpacityEffect>
#include <QTextDocument>
#include <QGraphicsScene>

// KDE include

#include <kdebug.h>
#include <kstandarddirs.h>


FaceItem::FaceItem(QGraphicsItem* parent, QGraphicsScene* scene, const QRect& rect, double scale, const QString& name)
        : QGraphicsObject(parent)
{
    faceRect    = new QGraphicsRectItem( 0, scene);
    sceneWidth  = scene->width();
    sceneHeight = scene->height();

    // Scale all coordinates to fit the initial size of the scene
    x1 = rect.topLeft().x()*scale;
    y1 = rect.topLeft().y()*scale;
    x2 = rect.bottomRight().x()*scale;
    y2 = rect.bottomRight().y()*scale;

    // A QRect containing coordinates for the face rectangle
    QRect scaledRect;
    scaledRect.setTopLeft(QPoint(x1, y1));
    scaledRect.setBottomRight(QPoint(x2, y2));

    // Draw the Face Rectangle as a QGraphicsRectItem using the dimensions of the scaledRect
    faceRect->setRect(scaledRect);
    QPen pen(QColor(QString("red")));
    pen.setWidth(3);
    faceRect->setPen(pen);
    faceRect->setOpacity(1);

    // Top-left Coordinates for the name
    int x = x1 + 20;
    int y = y2 + 10;

    // Make a new QGraphicsTextItem for writing the name text, and a new QGraphicsRectItem to draw a good-looking, semi-transparent bounding box.
    nameRect = new QGraphicsRectItem( 0, scene);
    faceName = new QGraphicsTextItem (name, 0, scene);

    // Make the bounding box for the name update itself to cover all the text whenever contents are changed
    QTextDocument* doc;
    doc = faceName->document();
    QTextOption o; 
    o.setAlignment(Qt::AlignCenter);
    doc->setDefaultTextOption(o);
    connect(doc, SIGNAL(contentsChanged()), 
            this, SLOT(update()));

    // Set coordinates of the name
    faceName->setPos(x,y);

    // Get coordinates of the name relative the the scene
    QRectF r = faceName->mapRectToScene(faceName->boundingRect());

    // Draw the bounding name rectangle with the scene coordinates
    nameRect->setRect(r);
    QPen p(QColor(QString("black")));
    p.setWidth(3);
    nameRect->setPen(p);
    nameRect->setBrush(QBrush(QColor(QString("black"))));
    nameRect->setOpacity(0.8);

    // Draw the name input item
    faceName->setDefaultTextColor(QColor(QString("white")));
    faceName->setFont(QFont("Helvetica", 8));
    faceName->setTextInteractionFlags(Qt::TextEditorInteraction);
    faceName->setOpacity(1);
    
    approveButton = new Button( KStandardDirs::locate("data", "regiontaggingwidget/icons/button-approve-normal.png"), 
                                KStandardDirs::locate("data", "regiontaggingwidget/icons/button-approve-pressed.png") );
                                
    rejectButton = new Button( KStandardDirs::locate("data", "regiontaggingwidget/icons/button-reject-normal.png"), 
                                KStandardDirs::locate("data", "regiontaggingwidget/icons/button-reject-pressed.png") );
    
    approveButton->hide();
    rejectButton->hide();
    
    scene->addItem(approveButton);
    scene->addItem(rejectButton);
    
    approveButton->setPos(x-40, y );
    rejectButton->setPos(x-20, y );
    
    connect(rejectButton, SIGNAL(clicked()), this, SLOT(clearText()) );
}

QRectF FaceItem::boundingRect() const
{
    qreal adjust = 0.5;
    return QRectF(-18 - adjust, -22 - adjust,
                  36 + adjust, 60 + adjust);
}

void FaceItem::paint(QPainter* /*painter*/, const QStyleOptionGraphicsItem* /*option*/, QWidget* /*widget*/)
{
}

void FaceItem::setText(const QString& newName)
{
    faceName->setPlainText(newName);
}

QString FaceItem::text() const
{
    return faceName->toPlainText();
}

void FaceItem::update()
{
    if(faceName->toPlainText() == "")
    {
        approveButton->hide();
        rejectButton->hide();
    }
    else
    {
        approveButton->show();
        rejectButton->show();
    }
    
    QRectF r = faceName->mapRectToScene(faceName->boundingRect());
    nameRect->setRect(r);
}

void FaceItem::setVisible(bool visible)
{
    this->faceRect->setVisible(visible);
    this->setControlsVisible(visible);
}

void FaceItem::setControlsVisible(bool visible)
{
    this->nameRect->setVisible(visible);
    this->faceName->setVisible(visible);
    this->approveButton->setVisible(visible);
    this->rejectButton->setVisible(visible);
}

void FaceItem::clearText()
{
    faceName->setPlainText("");
}

