/** ===========================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * @date  : 2010-06-21
 * @note  : GUI test program for libkface
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

FaceItem::FaceItem(QGraphicsItem* parent, QGraphicsScene* scene, QRect rect, double scale, QString name)
#if QT_VERSION >= 0x040600
        : QGraphicsObject(parent)
#else
        : QGraphicsItem(parent)
#endif
{
    faceRect = new QGraphicsRectItem( 0, scene);

    x1 = rect.topLeft().x()*scale;
    y1 = rect.topLeft().y()*scale;
    x2 = rect.bottomRight().x()*scale;
    y2 = rect.bottomRight().y()*scale;

    QRect scaledRect;
    scaledRect.setTopLeft(QPoint(x1, y1));
    scaledRect.setBottomRight(QPoint(x2, y2));

    faceRect->setRect(scaledRect);
    QPen pen(QColor(QString("red")));
    pen.setWidth(3);
    faceRect->setPen(pen);
    faceRect->setOpacity(0.5);

    int x = x1 + 10;
    int y = y2 + 10;
    
    nameRect = new QGraphicsRectItem( 0, scene);
    faceName = new QGraphicsTextItem (name, 0, scene);
    
    
    QTextDocument *doc;
    doc = faceName->document();
    connect(doc, SIGNAL(contentsChanged()), this, SLOT(update()));
    
    faceName->setPos(x,y);
    
    QRectF r = faceName->mapRectToScene(faceName->boundingRect());
    nameRect->setRect(r);
    QPen p(QColor(QString("black")));
    p.setWidth(3);
    nameRect->setPen(p);
    nameRect->setBrush(QBrush(QColor(QString("black"))));
    nameRect->setOpacity(0.8);

    
    faceName->setDefaultTextColor(QColor(QString("white")));
    faceName->setFont(QFont("Helvetica", 10));
    faceName->setTextInteractionFlags(Qt::TextEditorInteraction);
    faceName->setTextWidth((x2-x1)>300?(x2-x1):140);
    r = faceName->mapRectToScene(faceName->boundingRect());
    
    //faceName->setGraphicsEffect();
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

void FaceItem::setText(QString newName)
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
