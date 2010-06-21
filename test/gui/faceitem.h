#ifndef FACEITEM_H
#define FACEITEM_H

#include <QtGlobal>
#if QT_VERSION >= 0x040600
#include <QGraphicsObject>
#endif

#include <QGraphicsItem>
#include <QObject>
#include <QGraphicsRectItem>
#include <QBrush>
#include <QFont>

class FaceItem
#if QT_VERSION >= 0x040600
    : public QGraphicsObject
    #else
    : public QObject, public QGraphicsItem
#endif
{
    Q_OBJECT
//    Q_INTERFACES(QGraphicsItem)

public:

    FaceItem(QGraphicsItem* parent = 0, QGraphicsScene* scene = 0, QRect rect = QRect(0, 0, 0, 0),
             double scale = 1, QString name = QString("unknown"));

    QRectF boundingRect() const;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
               QWidget* widget);
    void setText(QString newName);

Q_SIGNALS:

public Q_SLOTS:

private:

    QGraphicsRectItem* faceRect;
    QGraphicsTextItem* faceName;
};

#endif // FACEITEM_H
