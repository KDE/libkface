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

#ifndef FACEITEM_H
#define FACEITEM_H

// Qt includes

#include <QtGlobal>
#if QT_VERSION >= 0x040600
#include <QGraphicsObject>
#endif

#include <QGraphicsItem>
#include <QObject>
#include <QGraphicsRectItem>
#include <QBrush>
#include <QFont>

class FaceItem
#if QT_VERSION >= 0x040600
    : public QGraphicsObject
#else
    : public QObject, public QGraphicsItem
#endif
{
    Q_OBJECT

public:

    FaceItem(QGraphicsItem* parent = 0, QGraphicsScene* scene = 0, QRect rect = QRect(0, 0, 0, 0),
             double scale = 1, QString name = QString("unknown"));

    QRectF boundingRect() const;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
               QWidget* widget);
    void setText(QString newName);

Q_SIGNALS:

public Q_SLOTS:

private:

    QGraphicsRectItem* faceRect;
    QGraphicsTextItem* faceName;
};

#endif // FACEITEM_H
