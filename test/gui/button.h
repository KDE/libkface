/********************************************************************************
**
**  GNU General Public License Usage
**
**  This program is free software: you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation, either version 3 of the License, or
**  (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program. If not, see <http://www.gnu.org/licenses/>.
**
********************************************************************************/

#ifndef BUTTON_H
#define BUTTON_H

#include <QGraphicsItem>
#include <QObject>
#include <QPainter>
#include <QGraphicsSceneMouseEvent>

class Button : public QObject, public QGraphicsItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)

public:
    Button(QGraphicsItem *parent = 0);
    Button(const QString normal, const QString pressed = "",
           QGraphicsItem *parent = 0);

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget = 0);
    void setPixmap(const QString normal, const QString pressed = "");

signals:
    void clicked();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

private:
    QPixmap m_normal;
    QPixmap m_pressed;
    bool m_isPressed;
};

#endif
