/** ===========================================================
 *
 * This file is a part of digiKam project
 * <a href="http://www.digikam.org">http://www.digikam.org</a>
 *
 * @date   2010-07-23
 * @brief  QGraphicsRectItem wrapper for libkface
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

// Qt includes

#include <Qt/QtGui>

// Local includes

#include "libkface_export.h"

namespace KFaceIface
{

class KFACE_EXPORT FancyRect : public QGraphicsRectItem
{

public:

    FancyRect(QGraphicsItem *parent = 0);
    FancyRect(const QRectF& rect, QGraphicsItem* parent = 0);
    FancyRect(qreal x, qreal y, qreal w, qreal h, QGraphicsItem* parent = 0);
    FancyRect(QGraphicsRectItem* other, QGraphicsItem* parent);

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0);
};

} // namespace KFaceIface
