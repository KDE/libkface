/** ===========================================================
 *
 * This file is a part of digiKam project
 * <a href="http://www.digikam.org">http://www.digikam.org</a>
 *
 * @date   2010-06-29
 * @brief  Pressable Button class using QGraphicsItem
 *
 * @author Copyright (C) 2010 by Frederico Duarte
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

#include <QGraphicsItem>
#include <QObject>
#include <QPainter>
#include <QGraphicsSceneMouseEvent>

class Button : public QObject, public QGraphicsItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)

public:

    Button(QGraphicsItem* parent = 0);
    Button(const QString& normal, const QString& pressed = QString(), QGraphicsItem* parent = 0);

    QRectF boundingRect() const;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
               QWidget* widget = 0);
    void setPixmap(const QString& normal, const QString& pressed = QString());

Q_SIGNALS:

    void clicked();

protected:

    void mousePressEvent(QGraphicsSceneMouseEvent*);
    void mouseMoveEvent(QGraphicsSceneMouseEvent*);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent*);

private:

    bool    m_isPressed;
    QPixmap m_normal;
    QPixmap m_pressed;
};

#endif // BUTTON_H
