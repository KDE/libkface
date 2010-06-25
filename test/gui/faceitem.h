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

#ifndef FACEITEM_H
#define FACEITEM_H

// Qt includes

#include <QtGlobal>
#include <QGraphicsObject>
#include <QObject>
#include <QGraphicsRectItem>
#include <QBrush>
#include <QFont>

class FaceItem
    : public QGraphicsObject
{
    Q_OBJECT

public:

    FaceItem(QGraphicsItem* parent = 0, QGraphicsScene* scene = 0, const QRect& rect = QRect(0, 0, 0, 0),
             double scale = 1, const QString& name = QString("?"));

    QRectF boundingRect() const;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);

    void setText(const QString& newName);
    QString getText();

public Q_SLOTS:

    void update();

private:

    int                sceneWidth, sceneHeight;
    int                x1, x2, y1, y2;
    QGraphicsRectItem* faceRect;
    QGraphicsTextItem* faceName;
    QGraphicsRectItem* nameRect;
};

#endif // FACEITEM_H
