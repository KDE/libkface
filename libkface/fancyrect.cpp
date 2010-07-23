/*
 * This file is part of the Nepomuk KDE project.
 * Copyright (C) 2008 Adrien Bustany <madcat@mymadcat.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "fancyrect.h"

#include <QGraphicsItem>
#include <QRectF>
#include <QPainter>

namespace KFaceIface
{
    
FancyRect::FancyRect(QGraphicsItem* parent) : QGraphicsRectItem(parent)
{
}

FancyRect::FancyRect(const QRectF& rect, QGraphicsItem* parent) : QGraphicsRectItem(rect, parent)
{
}

FancyRect::FancyRect(qreal x, qreal y, qreal w, qreal h, QGraphicsItem* parent) : QGraphicsRectItem(x, y, w, h, parent)
{
}

FancyRect::FancyRect(QGraphicsRectItem* other, QGraphicsItem* parent) : QGraphicsRectItem(parent)
{
    setPos(other->pos());
    setRect(other->rect());
    setPen(other->pen());
}

void FancyRect::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    if (option->state.testFlag(QStyle::State_Selected)) {
        QPen selectedPen = pen();
        selectedPen.setColor(Qt::red);
        painter->setPen(selectedPen);
        painter->drawRect(rect());
    } else {
        QGraphicsRectItem::paint(painter, option, widget);
    }
}

}   // Namespace KFaceIface