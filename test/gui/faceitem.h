#ifndef FACEITEM_H
#define FACEITEM_H

#include <QGraphicsItem>
#include <QObject>
#include <QGraphicsRectItem>
#include <QBrush>
#include <QFont>

class FaceItem : public QObject, public QGraphicsItem

{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)

private:
    QGraphicsRectItem *faceRect;
    QGraphicsTextItem *faceName;
public:
    FaceItem(QGraphicsItem *parent = 0, QGraphicsScene *scene = 0, QRect rect = QRect(0,0,0,0), double scale = 1, QString name = QString("unknown"));

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget);
    void setText(QString newName);


signals:

public slots:

};

#endif // FACEITEM_H
