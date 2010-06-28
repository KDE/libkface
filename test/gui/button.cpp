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

#include "button.h"

Button::Button(QGraphicsItem *parent)
        : QGraphicsItem(parent), m_isPressed(false)
{
}

Button::Button(const QString normal, const QString pressed,
               QGraphicsItem *parent)
        : QGraphicsItem(parent), m_isPressed(false)
{
    setPixmap(normal, pressed);
}

QRectF Button::boundingRect() const
{
    return m_normal.rect();
}

void Button::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                   QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    if (m_isPressed)
        painter->drawPixmap(0, 0, m_pressed);
    else
        painter->drawPixmap(0, 0, m_normal);
}

void Button::setPixmap(QString normal, QString pressed)
{
    if (! m_normal.isNull()) {
        m_normal.detach();
        m_pressed.detach();
    }

    m_normal.load(normal);
    m_pressed.load(pressed);
}

void Button::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        if (contains(event->pos()))
            m_isPressed = true;

        update();
    }
}

void Button::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        if (contains(event->pos())) {
            m_isPressed = true;
        } else {
            m_isPressed = false;
        }

        update();
    }
}

void Button::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_isPressed = false;

        update();

        if (contains(event->pos())) {
            emit clicked();
        }
    }
}

