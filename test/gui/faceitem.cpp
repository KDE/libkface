/** ===========================================================
 * @file
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

#include "faceitem.moc"

// C++ includes

#include <cmath>

// Qt includes

#include <QWidget>
#include <QPainter>
#include <QGraphicsOpacityEffect>
#include <QTextDocument>
#include <QGraphicsScene>
#include <QColor>
#include <QDebug>

// KDE include

#include <kdebug.h>
#include <kstandarddirs.h>
#include <kicon.h>

// Local includes

#include "marquee.h"
#include "fancyrect.h"

namespace KFaceIface
{

class FaceItem::FaceItemPriv
{
public:

    FaceItemPriv()
    {
        faceName               = 0;
        nameRect               = 0;
        rejectButton           = 0;
        acceptButton           = 0;
        faceMarquee            = 0;
        suggestionAcceptButton = 0;
        suggestionRejectButton = 0;
        suggestionMode         = false;
        name                   = QString();
    }

    bool               suggestionMode;

    int                sceneWidth, sceneHeight;
    int                x1, x2, y1, y2;

    QString            name;
    Marquee*           faceMarquee;

    QGraphicsTextItem* faceName;
    QGraphicsRectItem* nameRect;

    QRect              origRect;
    double             origScale;
    double             scale;

    Button*            rejectButton;
    Button*            acceptButton;

    Button*            suggestionRejectButton;
    Button*            suggestionAcceptButton;
};

FaceItem::FaceItem(QGraphicsItem* const parent, QGraphicsScene* const scene, const QRect& rect, 
                   double scale, const QString& name, double originalscale)
        : QGraphicsObject(parent), d(new FaceItemPriv)
{
    setAcceptHoverEvents(true);

    d->origScale     = originalscale;
    d->scale         = scale;
    d->origRect      = rect;
    FancyRect* fancy = 0;
    d->sceneWidth    = scene->width();
    d->sceneHeight   = scene->height();

    // Scale all coordinates to fit the initial size of the scene
    d->x1 = rect.topLeft().x()*scale;
    d->y1 = rect.topLeft().y()*scale;
    d->x2 = rect.bottomRight().x()*scale;
    d->y2 = rect.bottomRight().y()*scale;

    // A QRect containing coordinates for the face rectangle
    QRect scaledRect;
    scaledRect.setTopLeft(QPoint(d->x1, d->y1));
    scaledRect.setBottomRight(QPoint(d->x2, d->y2));

    // marquee
    fancy          = new FancyRect(scaledRect);
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
    QPen p(QColor(QString("white")));
    p.setWidth(3);
    d->nameRect->setPen(p);
    d->nameRect->setBrush(QBrush(QColor(QString("black"))));
    d->nameRect->setOpacity(0.6);
    d->nameRect->show();

    // Draw the name input item
    d->faceName->setDefaultTextColor(QColor(QString("white")));
    d->faceName->setFont(QFont("Helvetica", 9));
    d->faceName->setTextInteractionFlags(Qt::TextEditorInteraction);
    d->faceName->setOpacity(1);

    //---------------------

    QString s("dialog-close");
    KIcon* icon       = new KIcon(s);
    QPixmap rejectPix = icon->pixmap(QSize(16,16));

    d->rejectButton   = new Button( rejectPix, rejectPix);
    scene->addItem(d->rejectButton);
    d->rejectButton->show();

    QString s1("dialog-ok");
    KIcon* icon1      = new KIcon(s1);
    QPixmap acceptPix = icon1->pixmap(QSize(16,16));

    d->acceptButton   = new Button( acceptPix, acceptPix);
    scene->addItem(d->acceptButton);
    //d->acceptButton->show();

    d->suggestionRejectButton = new Button( rejectPix, rejectPix);
    scene->addItem(d->suggestionRejectButton);
    //d->suggestionAcceptButton->hide();

    d->suggestionAcceptButton = new Button( acceptPix, acceptPix);
    scene->addItem(d->suggestionAcceptButton);
    //d->suggestionRejectButton->hide();

    update();

    switchToEditMode();

    d->acceptButton->hide();

    connect(d->rejectButton, SIGNAL(clicked()),
            this, SLOT(reject()));

    connect(d->acceptButton, SIGNAL(clicked()),
            this, SLOT(accepted()));

    connect(d->suggestionAcceptButton, SIGNAL(clicked()),
            this, SLOT(slotSuggestionAccepted()) );

    connect(d->suggestionRejectButton, SIGNAL (clicked()),
            this, SLOT(slotSuggestionRejected()) );

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
                   36 + adjust,  60 + adjust);
}

void FaceItem::paint(QPainter* /*painter*/, const QStyleOptionGraphicsItem* /*option*/, QWidget* /*widget*/)
{
}

void FaceItem::setText(const QString& newName)
{
    d->faceName->setHtml(newName);
}

QString FaceItem::text() const
{
    return d->faceName->toPlainText().remove(QChar('?'));
}

void FaceItem::update()
{
    if(text().isEmpty())
    {
        d->faceName->setDefaultTextColor(QColor("white"));
        d->nameRect->setPen(QPen(QColor("white")));
        d->acceptButton->hide();
        d->name.clear();
    }
    else
    {
        d->nameRect->setPen(QPen(QColor("yellow")));
        d->faceName->setDefaultTextColor(QColor("yellow"));
        if(!d->suggestionMode)
        {
            d->acceptButton->show();
            d->name = text();
        }
    }

    QPointF bl = d->faceMarquee->mapRectToScene(d->faceMarquee->boundingRect()).bottomLeft();
    QPointF br = d->nameRect->mapRectToScene(d->nameRect->boundingRect()).bottomRight();
    d->faceName->setPos(bl.x() + 5, bl.y() + 5);

    d->rejectButton->setPos(bl.x() - 16, bl.y() + 9);
    d->acceptButton->setPos(br.x() + 4, bl.y() + 11);

    d->suggestionAcceptButton->setPos(br.x() + 4, bl.y() + 11);
    d->suggestionRejectButton->setPos(br.x() + 20, bl.y() + 11);

    QRectF r      = d->faceName->mapRectToScene(d->faceName->boundingRect());
    d->nameRect->setRect(r);
    QRect newRect = this->d->faceMarquee->mapRectToScene(d->faceMarquee->boundingRect()).toRect();
    kDebug() << "Origscale is : " << d->origScale << " and scale is " << d->scale;

    QSize s(newRect.size());
    s.scale(newRect.width()*std::sqrt(d->origScale), newRect.height()*std::sqrt(d->origScale), Qt::KeepAspectRatio);
    newRect.setSize(s);

    //newRect.setRect(x,y,w,h);
    kDebug() << "Orig before" << d->origRect;

    //d->origRect = newRect;
    kDebug() << "Orig after" << d->origRect;
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

    if(d->suggestionMode)
    {
        d->suggestionAcceptButton->setVisible(visible);
        d->suggestionRejectButton->setVisible(visible);
    }
    else
    {
        d->rejectButton->setVisible(visible);
        d->acceptButton->setVisible(visible);
    }
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

void FaceItem::accepted()
{
    d->acceptButton->hide();
    d->faceName->setDefaultTextColor(QColor("white"));
    d->nameRect->setPen(QPen(QColor("white")));
    emit acceptButtonClicked(this->text(), this->originalRect());
}

QRect FaceItem::originalRect()
{
    return d->origRect;
}

double FaceItem::originalScale()
{
    return d->origScale;
}

void FaceItem::reject()
{
    emit rejectButtonClicked(this->text(), this->originalRect());
    clearAndHide();
}

void FaceItem::suggest(const QString& name)
{
    kDebug() << "suggested name is " << name;
    d->name = name;
    this->switchToSuggestionMode();
}

void FaceItem::switchToEditMode()
{
    d->suggestionMode = false;
    d->faceName->setEnabled(true);
    d->faceName->setHtml("<b>" + d->name + "</b>");
    d->acceptButton->show();
    d->suggestionAcceptButton->hide();
    d->suggestionRejectButton->hide();
}

void FaceItem::switchToSuggestionMode()
{
    d->suggestionMode = true;
    d->faceName->setEnabled(false);
    d->faceName->setHtml("Is this <b>" + d->name + "</b>?");
    d->acceptButton->hide();
    d->suggestionAcceptButton->show();
    d->suggestionRejectButton->show();
}

void FaceItem::slotSuggestionAccepted()
{
    switchToEditMode();
    d->faceName->setHtml("<b>" + d->name + "</b>");
    accepted();
    emit suggestionAcceptButtonClicked(this->text(), this->originalRect());
}

void FaceItem::slotSuggestionRejected()
{
    switchToEditMode();
    d->faceName->setHtml("<b>" + QString() + "</b>");
    emit suggestionRejectButtonClicked(this->text(), this->originalRect());
}

} // namespace KFaceIface
