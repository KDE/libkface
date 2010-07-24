/** ===========================================================
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
 * @author Copyright (C) 2010 by Gilles Caulier
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

#include <QGraphicsItemGroup>

// KDE includes

#include <kdebug.h>

// Local includes

#include "fancyrect.h"

#define HANDLESIZE 10

namespace KFaceIface
{

Marquee::Marquee(FancyRect* rect, QGraphicsItem* parent)
       : QObject(NULL), QGraphicsItemGroup(parent), m_rect(rect)
{
    m_Htr = m_Hbr = m_Hbl = 0;
    m_rectPen.setColor(Qt::white);
    m_rectPen.setWidth(2);
    m_outlinePen.setColor(Qt::black);
    addToGroup(m_rect);
    m_rect->setPen(m_rectPen);
    setPos(m_rect->scenePos());
    m_rect->setPos(0, 0);
    createHandles();

    m_label = new QGraphicsSimpleTextItem("", this);
    m_label->setBrush(QBrush(m_rectPen.color()));
    m_label->setPen(m_outlinePen);
    m_label->setZValue(2);
    QFont font = m_label->font();
    font.setBold(true);
    font.setPointSize(12);
    m_label->setFont(font);
    placeLabel();
    setFlag(QGraphicsItem::ItemIsSelectable);
    setSelected(true);
    emit selected(this);
}

QRectF Marquee::boundingRect() const
{
    return m_rect->rect();
}

void Marquee::setLabel(const QString& text)
{
    m_label->setText(text);
    placeLabel();
}

QString Marquee::id()
{
    return m_mid;
}

void Marquee::setId(const QString& id)
{
    m_mid = id;
}

QRectF Marquee::toRectF()
{
    return QRectF(x(), y(), m_rect->rect().width(), m_rect->rect().height());
}

void Marquee::setLabelMatrix(const QMatrix& matrix)
{
    m_label->setTransform(QTransform(matrix));
    placeLabel();
}

void Marquee::createHandles()
{
    m_Htr = new FancyRect(0, 0, HANDLESIZE, HANDLESIZE);
    m_Htr->setZValue(1);
    m_Hbl = new FancyRect(0, 0, HANDLESIZE, HANDLESIZE);
    m_Hbl->setZValue(1);
    m_Hbr = new FancyRect(0, 0, HANDLESIZE, HANDLESIZE);
    m_Hbr->setZValue(1);

    m_Htr->setPen(m_rectPen);
    m_Hbl->setPen(m_rectPen);
    m_Hbr->setPen(m_rectPen);

    addToGroup(m_Htr);
    addToGroup(m_Hbl);
    addToGroup(m_Hbr);
    placeHandles();
}

void Marquee::placeHandles()
{
    qreal rw = m_rect->rect().width();
    qreal rh = m_rect->rect().height();
    qreal ox = m_rect->rect().x();
    qreal oy = m_rect->rect().y();
    qreal hs = m_Hbr->boundingRect().width();

    m_Htr->setPos(ox+rw-hs, oy);
    m_Hbl->setPos(ox, oy+rh-hs);
    m_Hbr->setPos(ox+rw-hs, oy+rh-hs);
}

void Marquee::placeLabel()
{
    QRect regionRect = m_label->boundingRegion(m_label->transform()).boundingRect();
    m_label->setPos((boundingRect().width() - regionRect.width())/2, (boundingRect().height() - regionRect.height())/2);
}

void Marquee::mousePressEvent(QGraphicsSceneMouseEvent* e)
{
    emit selected(this);
    // Check for some resize handles under the mouse

    if (m_Htr->isUnderMouse())
    {
        m_resizing = true;
        m_resizeType = 1;
        return;
    }
    if (m_Hbl->isUnderMouse())
    {
        m_resizing = true;
        m_resizeType = 2;
        return;
    }
    if (m_Hbr->isUnderMouse())
    {
        m_resizing = true;
        m_resizeType = 3;
        return;
    }

    // If no handle is under the mouse, then we move the frame
    m_resizing = false;
    m_moving = true;
    m_moveOffset = e->pos();

    emit changed();
}

void Marquee::mouseMoveEvent(QGraphicsSceneMouseEvent* e)
{
    if (m_resizing)
    {
        QRectF r = m_rect->rect();
        switch(m_resizeType)
	{
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
        if (r.width() < 2*HANDLESIZE || r.height() < 2*HANDLESIZE) return;
        setPos(pos() + r.topLeft());
        r.moveTopLeft(QPointF(0, 0));
        m_rect->setRect(r);
        placeHandles();
        placeLabel();
    }
    if (m_moving)
    {
        setPos(e->scenePos() - m_moveOffset);
    }

    emit changed();
}

void Marquee::mouseReleaseEvent(QGraphicsSceneMouseEvent* e)
{
    Q_UNUSED(e);
    m_resizing = false;
    m_moving = false;

    emit changed();
}

} // namespace KFaceIface
