/** ===========================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * @date  : 2010-06-21
 * @note  : GUI test program for libkface
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

#include "faceitem.h"

// Qt includes

#include <QDebug>

FaceItem::FaceItem(QGraphicsItem* parent, QGraphicsScene* scene, QRect rect, double scale, QString /*name*/)
#if QT_VERSION >= 0x040600
        : QGraphicsObject(parent)
#else
        : QGraphicsItem(parent)
#endif
{
    faceRect = new QGraphicsRectItem( 0, scene);

    int x1, y1, x2, y2;
    x1 = rect.topLeft().x();
    y1 = rect.topLeft().y();
    x2 = rect.bottomRight().x();
    y2 = rect.bottomRight().y();

    QRect scaledRect;
    scaledRect.setTopLeft(QPoint(x1*scale, y1*scale));
    scaledRect.setBottomRight(QPoint(x2*scale, y2*scale));

    faceRect->setRect(scaledRect);

    faceRect->setBrush(QBrush(QColor(QString("red"))));

    faceRect->setOpacity(0.5);

    //faceName = new QGraphicsTextItem (name, 0, scene);
    //faceName->setPos(x-10,y-10);
    //faceName->setFont(QFont("Helvetica"));
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

    //Update?
}
