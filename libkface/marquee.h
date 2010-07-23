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

#ifndef MARQUEE_H
#define MARQUEE_H

#include <QtGui>

#include "libkface_export.h"

namespace KFaceIface
{

class FancyRect;

class KFACE_EXPORT Marquee : public QObject, public QGraphicsItemGroup
{

    Q_OBJECT

public:
    Marquee(FancyRect* rect, QGraphicsItem* parent = 0);
    QRectF boundingRect() const;
    void setLabel(const QString &text);
    QString id();
    void setId(const QString& id);
    QRectF toRectF();
    void setLabelMatrix(const QMatrix& matrix);

signals:
    void selected(Marquee *m);
    void changed();

private:
    void                        createHandles();
    void                        placeHandles();
    void                        placeLabel();
    QPen                        m_rectPen; // The pen used to draw the frames
    QPen                        m_outlinePen; // Text outline pen

    // Handles
    FancyRect*                  m_Htr;
    FancyRect*                  m_Hbr;
    FancyRect*                  m_Hbl;

    FancyRect*                  m_rect; // Main frame
    QGraphicsSimpleTextItem*    m_label;

    bool                        m_moving; // Tells if we are moving the marquee
    QPointF                     m_moveOffset; // where the mouse was when the user began to drag the marquee
    bool                        m_resizing; // Tells if we are resizing the marquee
    char                        m_resizeType; // 0 = from top left, 1 = top right, etc.
    QString                     m_mid; // Nepomuk resource id, only set for previously existing regions

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *e);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *e);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *e);

};

} // End namespace KFaceIface

#endif
