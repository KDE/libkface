/** ===========================================================
 * @file
 *
 * This file is a part of digiKam project
 * <a href="http://www.digikam.org">http://www.digikam.org</a>
 *
 * @date   2010-06-29
 * @brief  Pressable Button class using QGraphicsItem
 *
 * @author Copyright (C) 2010 by Frederico Duarte
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

#ifndef BUTTON_H
#define BUTTON_H

// Qt includes

#include <QtCore/QObject>
#include <QtGui/QPainter>
#include <QtGui/QGraphicsItem>
#include <QtGui/QGraphicsSceneMouseEvent>

// Local includes

#include "libkface_export.h"

namespace KFaceIface
{

class Button : public QObject, public QGraphicsItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)

public:

    Button(QGraphicsItem* const parent = 0);
    explicit Button(const QString& normal, const QString& pressed = QString(), QGraphicsItem* const parent = 0);
    ~Button();

    Button(const QPixmap& normal, const QPixmap& pressed, QGraphicsItem* const parent = 0);

    QRectF boundingRect() const;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0);
    void setPixmap(const QString& normal, const QString& pressed = QString());
    void setPixmap(const QPixmap& normal, const QPixmap& pressed);

Q_SIGNALS:

    void clicked();

protected:

    void mousePressEvent(QGraphicsSceneMouseEvent*);
    void mouseMoveEvent(QGraphicsSceneMouseEvent*);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent*);

private:

    class ButtonPriv;
    ButtonPriv* const d;
};

} // namespace KFaceIface

#endif // BUTTON_H
