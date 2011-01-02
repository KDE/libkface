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

#ifndef MARQUEE_H
#define MARQUEE_H

// Qt includes

#include <QtCore/QObject>
#include <QtCore/QRectF>
#include <QtGui/QGraphicsItemGroup>
#include <QtGui/QGraphicsItem>
#include <QtGui/QGraphicsSceneMouseEvent>

// Local includes

#include "libkface_export.h"

namespace KFaceIface
{

class FancyRect;

class KFACE_EXPORT Marquee : public QObject, public QGraphicsItemGroup
{
    Q_OBJECT

public:

    Marquee(FancyRect* rect, QGraphicsItem* parent = 0);
    ~Marquee();

    QRectF boundingRect() const;
    QRectF toRectF();

Q_SIGNALS:

    void selected(Marquee* m);
    void changed();
    void entered();
    void left();

protected:

    void mousePressEvent(QGraphicsSceneMouseEvent* e);
    void mouseMoveEvent(QGraphicsSceneMouseEvent* e);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* e);

private:

    void createHandles();
    void placeHandles();
    void placeLabel();

private:

    class MarqueePriv;
    MarqueePriv* const d;
};

} // namespace KFaceIface

#endif // MARQUEE_H
