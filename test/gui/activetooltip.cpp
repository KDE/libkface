/* This file is part of the KDE project
   Copyright 2007 Vladimir Prus

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "activetooltip.h"

#include <QPoint>
#include <QPalette>
#include <QApplication>
#include <QEvent>
#include <QMouseEvent>
#include <limits>
#include <kdebug.h>
#include <QPointer>
#include <qdesktopwidget.h>
#include <qmenu.h>

class ActiveToolTipPrivate
{
public:
    uint previousDistance_;
    QRect rect_;
    QRegion rectExtensions_;
    QList<QPointer<QObject> > friendWidgets_;
    int mouseOut_;
};

ActiveToolTip::ActiveToolTip(QWidget *parent, const QPoint& position)
    : QWidget(parent, Qt::ToolTip), d(new ActiveToolTipPrivate)
{
    d->mouseOut_ = 0;
    d->previousDistance_ = std::numeric_limits<uint>::max();
    setMouseTracking(true);
    d->rect_ = QRect(position, position);
    d->rect_.adjust(-10, -10, 10, 10);
    move(position);

    QPalette p;
    p.setColor(backgroundRole(), p.color(QPalette::ToolTipBase));
    p.setColor(QPalette::Base, p.color(QPalette::ToolTipBase));
    setPalette(p);

    qApp->installEventFilter(this);
}

ActiveToolTip::~ActiveToolTip()
{
    delete d;
}

bool ActiveToolTip::eventFilter(QObject *object, QEvent *e)
{
    switch (e->type()) {

    case QEvent::WindowActivate:
    case QEvent::WindowDeactivate:
    {
        if(insideThis(object))
            return false;
        kDebug() << "closing because of window activation";
        close();
    }
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
    case QEvent::MouseButtonDblClick:
    case QEvent::Wheel:
        /* If the click is within tooltip, it's fine.
           Clicks outside close it.  */
        {
            //Do not close the widget when NoFocus widgets are used
            QWidget* widget = dynamic_cast<QWidget*>(object);
            if(widget && widget->focusPolicy() == Qt::NoFocus)
                return false;
        }
        if (!insideThis(object)) {
            kDebug() << "closing because of click into" << object;
            close();
        }

    // FIXME: revisit this code later.
#if 0
    case QEvent::FocusIn:
    case QEvent::FocusOut:
        close();
        break;
#endif
    case QEvent::MouseMove:
        {
            QPoint globalPos = static_cast<QMouseEvent*>(e)->globalPos();
            if (!d->rect_.isNull() 
                && !d->rect_.contains(globalPos) && !d->rectExtensions_.contains(globalPos) && !insideThis(object)) {
                
                int distance = (d->rect_.center() - static_cast<QMouseEvent*>(e)->globalPos()).manhattanLength();
                
                // On X, when the cursor leaves the tooltip and enters
                // the parent, we sometimes get some wrong Y coordinate.
                // Don't know why, so wait for two out-of-range mouse
                // positions before closing.
                
                //Additional test: When the cursor has been moved towards the tooltip, don't close it.
                if(distance > (int)d->previousDistance_)
                    ++d->mouseOut_;
                else
                    d->previousDistance_ = distance;
            } else               
                d->mouseOut_ = 0;
            if (d->mouseOut_ == 2) {
                kDebug() << "closing because of mouse move";
                close();
            }
        }
    default:
        break;
    }
    return false;
}

void ActiveToolTip::addFriendWidget(QWidget* widget)
{
    d->friendWidgets_.append((QObject*)widget);
}

bool ActiveToolTip::insideThis(QObject* object)
{
    while (object)
    {
        if(dynamic_cast<QMenu*>(object))
            return true;
        if (object == this || d->friendWidgets_.contains(object))
        {
            return true;
        }
        object = object->parent();
    }
    return false;
}

void ActiveToolTip::showEvent(QShowEvent*)
{        
    adjustRect();
}

void ActiveToolTip::updateMouseDistance()
{
    d->previousDistance_ = (d->rect_.center() - QCursor::pos()).manhattanLength();
}

void ActiveToolTip::moveEvent(QMoveEvent* ev)
{
    QWidget::moveEvent(ev);
    
    updateMouseDistance();
}

void ActiveToolTip::resizeEvent(QResizeEvent*)
{
    adjustRect();
    
    emit resized();

    updateMouseDistance();
}

void ActiveToolTip::addExtendRect(QRect rect)
{
    d->rectExtensions_ += rect;
}

void ActiveToolTip::adjustRect()
{
    // For tooltip widget, geometry() returns global coordinates.
    QRect r = geometry();
    r.adjust(-10, -10, 10, 10);
    d->rect_ = r;
    updateMouseDistance();
}

void ActiveToolTip::setBoundingGeometry(QRect r) {
    r.adjust(-10, -10, 10, 10);
    d->rect_ = r;
}

namespace {
    typedef QMultiMap<float, QPair<QPointer<ActiveToolTip>, QString> > ToolTipPriorityMap;
    static ToolTipPriorityMap registeredToolTips;
    ActiveToolTipManager manager;
    
    QWidget* masterWidget(QWidget* w) {
    while(w && w->parent() && qobject_cast<QWidget*>(w->parent()))
        w = qobject_cast<QWidget*>(w->parent());
    return w;
    }
}

void ActiveToolTipManager::doVisibility() {
    bool hideAll = false;
    int lastBottomPosition = -1;
    int lastLeftPosition = -1;
    QRect fullGeometry; //Geometry of all visible tooltips together
    
    for(ToolTipPriorityMap::const_iterator it = registeredToolTips.constBegin(); it != registeredToolTips.constEnd(); ++it) {
        QPointer< ActiveToolTip > w = (*it).first;
        if(w) {
            if(hideAll) {
                (w)->hide();
            }else{
                QRect geom = (w)->geometry();
                if((w)->geometry().top() < lastBottomPosition) {
                    geom.moveTop(lastBottomPosition);
                }
                if(lastLeftPosition != -1)
                    geom.moveLeft(lastLeftPosition);
                
                (w)->setGeometry(geom);
//                 (w)->show();
                    
                lastBottomPosition = (w)->geometry().bottom();
                lastLeftPosition = (w)->geometry().left();
                
                if(it == registeredToolTips.constBegin())
                    fullGeometry = (w)->geometry();
                else
                    fullGeometry = fullGeometry.united((w)->geometry());
            }
            if(it.key() == 0) {
                hideAll = true;
            }
        }
    }
    if(!fullGeometry.isEmpty()) {
        QRect oldFullGeometry = fullGeometry;
        QRect screenGeometry = QApplication::desktop()->screenGeometry(fullGeometry.topLeft());
        if(fullGeometry.bottom() > screenGeometry.bottom()) {
            //Move up, avoiding the mouse-cursor
            fullGeometry.moveBottom(fullGeometry.top()-10);
            if(fullGeometry.bottom() > QCursor::pos().y() - 20)
                fullGeometry.moveBottom(QCursor::pos().y() - 20);
        }
        if(fullGeometry.right() > screenGeometry.right()) {
            //Move to left, avoiding the mouse-cursor
            fullGeometry.moveRight(fullGeometry.left()-10);
            if(fullGeometry.right() > QCursor::pos().x() - 20)
                fullGeometry.moveRight(QCursor::pos().x() - 20);
        }
        
        QPoint offset = fullGeometry.topLeft() - oldFullGeometry.topLeft();
        if(!offset.isNull()) {
            for(ToolTipPriorityMap::const_iterator it = registeredToolTips.constBegin(); it != registeredToolTips.constEnd(); ++it)
                if((*it).first) {
                    (*it).first->move((*it).first->pos() + offset);
                }
        }
    }

    //Set bounding geometry, and remove old tooltips
    for(ToolTipPriorityMap::iterator it = registeredToolTips.begin(); it != registeredToolTips.end(); ) {
        if(!(*it).first) {
            it = registeredToolTips.erase(it);
        }else{
            (*it).first->setBoundingGeometry(fullGeometry);
            ++it;
        }
    }

    //Final step: Show all tooltips
    if(!hideAll) {
        for(ToolTipPriorityMap::const_iterator it = registeredToolTips.constBegin(); it != registeredToolTips.constEnd(); ++it)
            if(it->first && masterWidget(it->first)->isActiveWindow())
                (*it).first->show();
    }
}

void ActiveToolTip::showToolTip(ActiveToolTip* tooltip, float priority, QString uniqueId) {
    
    if(!uniqueId.isEmpty()) {
        for(QMap< float, QPair< QPointer< ActiveToolTip >, QString > >::const_iterator it = registeredToolTips.constBegin(); it != registeredToolTips.constEnd(); ++it) {
            if((*it).second == uniqueId)
                delete (*it).first;
        }
    }

    registeredToolTips.insert(priority, qMakePair(QPointer<ActiveToolTip>(tooltip), uniqueId));

    connect(tooltip, SIGNAL(resized()), &manager, SLOT(doVisibility()));
    QMetaObject::invokeMethod(&manager, "doVisibility", Qt::QueuedConnection);
}


void ActiveToolTip::closeEvent(QCloseEvent* event)
{
    QWidget::closeEvent(event);
    deleteLater();
}




#include "activetooltip.moc"
