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
#ifndef ACTIVE_TOOLTIP_H
#define ACTIVE_TOOLTIP_H

#include <QtGui/QWidget>

///Internal
class ActiveToolTipManager : public QObject {
    Q_OBJECT
    friend class ActiveToolTip;
    private Q_SLOTS:
        void doVisibility();
};

/** This class implements a tooltip that can contain arbitrary
    widgets that the user can interact with.

    Usage example:
    KDevelop::ActiveToolTip* tooltip = new KDevelop::ActiveToolTip(mainWindow, QCursor::pos());
    QVBoxLayout* layout = new QVBoxLayout(tooltip);
    layout->addWidget(widget);
    tooltip->resize( tooltip->sizeHint() );
    ActiveToolTip::showToolTip(tooltip);
 */
class ActiveToolTip : public QWidget
{
Q_OBJECT
public:
    /* position must be in global coordinates.  */
    ActiveToolTip(QWidget *parent, const QPoint& position);
    ~ActiveToolTip();

    ///Shows and registers the given tool-tip.
    ///This should be used instead of just calling show() to make multiple different
    ///tooltips work together.
    ///The tooltip is owned by the manager after this is called. It will delete itself.
    ///@param tooltip  The tooltip to show. It should not be visible yet, show() will eventually be called from here, with some delay.
    ///                The ownership stays with the caller.
    ///@param priority The priority of this tooltip. Lower is better. Multiple tooltips will be stacked down in the given order.
    ///                If it is zero, the given tooltip will be shown exclusively.
    ///@param uniqueId If this is nonempty, ActiveTooltip will make sure that only one tooltip with the given id is shown at a time
    static void showToolTip(ActiveToolTip* tooltip, float priority = 100, QString uniqueId = QString());
    
    bool eventFilter(QObject *object, QEvent *e);
    
    bool insideThis(QObject* object);

    void showEvent(QShowEvent*);

    void resizeEvent(QResizeEvent*);
    
    void moveEvent(QMoveEvent*);

    void adjustRect();
    
    ///Clicks within the friend widget are allowed
    void addFriendWidget(QWidget* widget);
    
    ///Add a rectangle to the area in which the mouse can be moved freely without hiding the tooltip
    void addExtendRect(QRect rect);
    
    ///Set the area within which the mouse can be moved freely without hiding the tooltip
    void setBoundingGeometry(QRect geometry);
Q_SIGNALS:
    void resized();    
private:
    virtual void closeEvent(QCloseEvent* );
    void updateMouseDistance();
    
    class ActiveToolTipPrivate* const d;
};


#endif
