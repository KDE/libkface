/** ===========================================================
 *
 * This file is a part of digiKam project
 * <a href="http://www.digikam.org">http://www.digikam.org</a>
 *
 * @date   2010-06-21
 * @brief  GUI test program for libkface
 *
 * @author Copyright (C) 2010 by Alex Jironkin
 *         <a href="mailto:alexjironkin at gmail dot com">alexjironkin at gmail dot com</a>
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

#include "faceitem.moc"

// Qt includes

#include <QWidget>
#include <QPainter>
#include <QGraphicsOpacityEffect>
#include <QTextDocument>
#include <QGraphicsScene>
#include <QColor>

// KDE include

#include <kdebug.h>
#include <kstandarddirs.h>
#include <kicon.h>

#include "marquee.h"
#include "fancyrect.h"

namespace KFaceIface
{

class FaceItem::FaceItemPriv
{
public:

    FaceItemPriv()
    {
        faceName      = 0;
        nameRect      = 0;
        rejectButton  = 0;
        faceMarquee   = 0;
    }

    int                sceneWidth, sceneHeight;
    int                x1, x2, y1, y2;
    Marquee*           faceMarquee;
    QGraphicsTextItem* faceName;
    QGraphicsRectItem* nameRect;
    Button*            rejectButton;
};

FaceItem::FaceItem(QGraphicsItem* parent, QGraphicsScene* scene, const QRect& rect, double scale, const QString& name)
        : QGraphicsObject(parent), d(new FaceItemPriv)
{
    setAcceptHoverEvents(true);

    FancyRect* fancy;
    
    d->sceneWidth  = scene->width();
    d->sceneHeight = scene->height();

    // Scale all coordinates to fit the initial size of the scene
    d->x1 = rect.topLeft().x()*scale;
    d->y1 = rect.topLeft().y()*scale;
    d->x2 = rect.bottomRight().x()*scale;
    d->y2 = rect.bottomRight().y()*scale;

    // Top-left Coordinates for the name
    int x = d->x1 + 20;
    int y = d->y2 + 10;

    // A QRect containing coordinates for the face rectangle
    QRect scaledRect;
    scaledRect.setTopLeft(QPoint(d->x1, d->y1));
    scaledRect.setBottomRight(QPoint(d->x2, d->y2));
    
    ////// marquee
    
    fancy = new FancyRect(scaledRect);
    d->faceMarquee = new Marquee(fancy);
    scene->addItem(d->faceMarquee);
    
    // Make a new QGraphicsTextItem for writing the name text, and a new QGraphicsRectItem to draw a good-looking, semi-transparent bounding box.
    d->nameRect = new QGraphicsRectItem( 0, scene);
    d->faceName = new QGraphicsTextItem (name, 0, scene);

    // Make the bounding box for the name update itself to cover all the text whenever contents are changed
    QTextDocument* doc = d->faceName->document();
    QTextOption o;
    o.setAlignment(Qt::AlignCenter);
    doc->setDefaultTextOption(o);

    // Get coordinates of the name relative the the scene
    QRectF r = d->faceName->mapRectToScene(d->faceName->boundingRect());

    // Draw the bounding name rectangle with the scene coordinates
    d->nameRect->setRect(r);
    QPen p(QColor(QString("black")));
    p.setWidth(3);
    d->nameRect->setPen(p);
    d->nameRect->setBrush(QBrush(QColor(QString("black"))));
    d->nameRect->setOpacity(0.8);
    d->nameRect->show();

    // Draw the name input item
    d->faceName->setDefaultTextColor(QColor(QString("white")));
    d->faceName->setFont(QFont("Helvetica", 8));
    d->faceName->setTextInteractionFlags(Qt::TextEditorInteraction);
    d->faceName->setOpacity(1);

    //---------------------

    QString s("dialog-close");
    KIcon* icon       = new KIcon(s);
    QPixmap rejectPix = icon->pixmap(QSize(16,16));

    d->rejectButton   = new Button( rejectPix, rejectPix);
    d->rejectButton->hide();
    scene->addItem(d->rejectButton);
    d->rejectButton->show();
    
    update();
    
    connect(d->rejectButton, SIGNAL(clicked()),
            this, SLOT(clearAndHide()));

    connect(doc, SIGNAL(contentsChanged()),
            this, SLOT(update()));
    
    connect(d->faceMarquee, SIGNAL(changed()),
            this, SLOT(update()));
}

FaceItem::~FaceItem()
{
    delete d;
}

QRectF FaceItem::boundingRect() const
{
    qreal adjust = 0.5;
    return QRectF(-18 - adjust, -22 - adjust,
                  36 + adjust, 60 + adjust);
}

void FaceItem::paint(QPainter* /*painter*/, const QStyleOptionGraphicsItem* /*option*/, QWidget* /*widget*/)
{
}

void FaceItem::setText(const QString& newName)
{
    d->faceName->setPlainText(newName);
}

QString FaceItem::text() const
{
    return d->faceName->toPlainText();
}

void FaceItem::update()
{
    QPointF tl     = d->faceMarquee->mapRectToScene(d->faceMarquee->boundingRect()).topLeft();
    d->rejectButton->setPos(tl.x() - 8, tl.y() - 8);
    QPointF bl     = d->faceMarquee->mapRectToScene(d->faceMarquee->boundingRect()).bottomLeft();
    d->faceName->setPos(bl.x(), bl.y() + 5);
    QRectF r = d->faceName->mapRectToScene(d->faceName->boundingRect());
    d->nameRect->setRect(r);
}

void FaceItem::setVisible(bool visible)
{
    d->faceMarquee->setVisible(visible);
    setControlsVisible(visible);
}

void FaceItem::setControlsVisible(bool visible)
{
    d->nameRect->setVisible(visible);
    d->faceName->setVisible(visible);
    d->rejectButton->setVisible(visible);
}

void FaceItem::clearText()
{
    d->faceName->setPlainText(QString());
}

void FaceItem::hoverEnterEvent(QGraphicsSceneHoverEvent* /*event*/)
{

}

void FaceItem::hoverMoveEvent(QGraphicsSceneHoverEvent* /*event*/)
{
    
}

void FaceItem::hoverLeaveEvent(QGraphicsSceneHoverEvent* /*event*/)
{
    
}

void FaceItem::clearAndHide()
{
    clearText();
    setVisible(false);
}

} // namespace KFaceIface
