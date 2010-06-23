/** ===========================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * @date  : 2010-06-21
 * @brief : GUI test program for libkface
 *
 * @author: Copyright (C) 2010 by Aditya Bhatt <adityabhatt1991 at gmail dot com>
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

#include "faceitem.h"

// Qt includes

#include <QDebug>
#include <QWidget>
#include <QPainter>
#include <QGraphicsOpacityEffect>
#include <QTextDocument>

FaceItem::FaceItem(QGraphicsItem* parent, QGraphicsScene* scene, const QRect& rect, double scale, const QString& name)
#if QT_VERSION >= 0x040600
        : QGraphicsObject(parent)
#else
        : QGraphicsItem(parent)
#endif
{
    faceRect = new QGraphicsRectItem( 0, scene);

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
    int x = x1 - 20;
    int y = y2 + 10;

    // Make a new QGraphicsTextItem for writing the name text, and a new QGraphicsRectItem to draw a good-looking, semi-transparent bounding box.
    nameRect = new QGraphicsRectItem( 0, scene);
    faceName = new QGraphicsTextItem (name, 0, scene);

    // Make the bounding box for the name update itself to cover all the text whenever contents are changed
    QTextDocument* doc;
    doc = faceName->document();
    connect(doc, SIGNAL(contentsChanged()), 
            this, SLOT(update()));

    // Set coordinates of the name
    faceName->setPos(x,y);
    faceName->setTextWidth((x2-x1)>140?(x2-x1):140);

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
    faceName->setFont(QFont("Helvetica", 10));
    faceName->setTextInteractionFlags(Qt::TextEditorInteraction);
    // If the width of the name rect is greater than 140, then let the string extend upto the width. Else keep the length to a max of 140 px.
    faceName->setOpacity(1);
    
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

QString FaceItem::getText()
{
    return faceName->toPlainText();
}

void FaceItem::update()
{
    QRectF r = faceName->mapRectToScene(faceName->boundingRect());
    nameRect->setRect(r);
}
