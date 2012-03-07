/** ===========================================================
 * @file
 *
 * This file is a part of digiKam project
 * <a href="http://www.digikam.org">http://www.digikam.org</a>
 *
 * @date   2010-07-23
 * @brief  face marquer widget for libkface
 *
 * @author Copyright (C) 2008 Adrien Bustany
 *         <a href="mailto:madcat at mymadcat dot com">madcat at mymadcat dot com</a>
 * @author Copyright (C) 2010 by Aditya Bhatt
 *         <a href="mailto:adityabhatt1991 at gmail dot com">adityabhatt1991 at gmail dot com</a>
 * @author Copyright (C) 2010-2012 by Gilles Caulier
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

#include "marquee.moc"

// Qt includes

#include <QPointF>
#include <QPen>
#include <QString>
#include <QGraphicsSimpleTextItem>

// KDE includes

#include <kdebug.h>

// Local includes

#include "fancyrect.h"

#define HANDLESIZE 10

namespace KFaceIface
{

class Marquee::MarqueePriv
{
public:

    MarqueePriv()
    {
        htr   = 0;
        hbr   = 0;
        hbl   = 0;
        label = 0;
    }

    QPen                     rectPen;    // The pen used to draw the frames
    QPen                     outlinePen; // Text outline pen

    // Handles
    FancyRect*               htl;
    FancyRect*               htr;
    FancyRect*               hbr;
    FancyRect*               hbl;

    FancyRect*               rect;       // Main frame
    QGraphicsSimpleTextItem* label;

    bool                     moving;     // Tells if we are moving the marquee
    QPointF                  moveOffset; // where the mouse was when the user began to drag the marquee
    bool                     resizing;   // Tells if we are resizing the marquee
    char                     resizeType; // 0 = from top left, 1 = top right, etc.
    QString                  mid;        // Nepomuk resource id, only set for previously existing regions
};

Marquee::Marquee(FancyRect* const rect, QGraphicsItem* const parent)
    : QObject(0), QGraphicsItemGroup(parent), d(new MarqueePriv)
{
    d->rect = rect;
    d->rectPen.setColor(Qt::red);
    d->rectPen.setWidth(2);
    d->outlinePen.setColor(Qt::red);
    addToGroup(d->rect);
    d->rect->setPen(d->rectPen);
    setPos(d->rect->scenePos());
    d->rect->setPos(0, 0);
    createHandles();

    d->label = new QGraphicsSimpleTextItem("", this);
    d->label->setBrush(QBrush(d->rectPen.color()));
    d->label->setPen(d->outlinePen);
    d->label->setZValue(2);
    QFont font = d->label->font();
    font.setBold(true);
    font.setPointSize(12);
    d->label->setFont(font);
    setFlag(QGraphicsItem::ItemIsSelectable);
    setSelected(true);
    emit selected(this);
}

Marquee::~Marquee()
{
    delete d;
}

QRectF Marquee::boundingRect() const
{
    return d->rect->rect();
}

QRectF Marquee::toRectF()
{
    return QRectF(x(), y(), d->rect->rect().width(), d->rect->rect().height());
}

void Marquee::createHandles()
{
    d->htl = new FancyRect(0, 0, HANDLESIZE, HANDLESIZE);
    d->htl->setZValue(1);
    d->htr = new FancyRect(0, 0, HANDLESIZE, HANDLESIZE);
    d->htr->setZValue(1);
    d->hbl = new FancyRect(0, 0, HANDLESIZE, HANDLESIZE);
    d->hbl->setZValue(1);
    d->hbr = new FancyRect(0, 0, HANDLESIZE, HANDLESIZE);
    d->hbr->setZValue(1);

    d->htl->setPen(d->rectPen);
    d->htr->setPen(d->rectPen);
    d->hbl->setPen(d->rectPen);
    d->hbr->setPen(d->rectPen);

    addToGroup(d->htl);
    addToGroup(d->htr);
    addToGroup(d->hbl);
    addToGroup(d->hbr);
    placeHandles();
}

void Marquee::placeHandles()
{
    qreal rw = d->rect->rect().width();
    qreal rh = d->rect->rect().height();
    qreal ox = d->rect->rect().x();
    qreal oy = d->rect->rect().y();
    qreal hs = d->hbr->boundingRect().width();

    d->htl->setPos(ox, oy);
    d->htr->setPos(ox+rw-hs, oy);
    d->hbl->setPos(ox, oy+rh-hs);
    d->hbr->setPos(ox+rw-hs, oy+rh-hs);
}

void Marquee::mousePressEvent(QGraphicsSceneMouseEvent* e)
{
    emit selected(this);
    // Check for some resize handles under the mouse

    if (d->htl->isUnderMouse())
    {
        d->resizing   = true;
        d->resizeType = 0;
        return;
    }

    if (d->htr->isUnderMouse())
    {
        d->resizing   = true;
        d->resizeType = 1;
        return;
    }
    if (d->hbl->isUnderMouse())
    {
        d->resizing   = true;
        d->resizeType = 2;
        return;
    }
    if (d->hbr->isUnderMouse())
    {
        d->resizing   = true;
        d->resizeType = 3;
        return;
    }

    // If no handle is under the mouse, then we move the frame
    d->resizing   = false;
    d->moving     = true;
    d->moveOffset = e->pos();

    emit changed();
}

void Marquee::mouseMoveEvent(QGraphicsSceneMouseEvent* e)
{
    if (d->resizing)
    {
        QRectF r = d->rect->rect();
        switch(d->resizeType)
        {
            case 0:
                r.setTopLeft(e->pos());
                break;
            case 1:
                r.setTopRight(e->pos());
                break;
            case 2:
                r.setBottomLeft(e->pos());
                break;
            case 3:
                r.setBottomRight(e->pos());
                break;
            default:
                break;
        }

        if (r.width() < 2*HANDLESIZE || r.height() < 2*HANDLESIZE)
            return;

        setPos(pos() + r.topLeft());
        r.moveTopLeft(QPointF(0, 0));
        d->rect->setRect(r);
        placeHandles();
    }

    if (d->moving)
    {
        setPos(e->scenePos() - d->moveOffset);
    }

    emit changed();
}

void Marquee::mouseReleaseEvent(QGraphicsSceneMouseEvent* e)
{
    Q_UNUSED(e);
    d->resizing = false;
    d->moving   = false;

    emit changed();
}

} // namespace KFaceIface
