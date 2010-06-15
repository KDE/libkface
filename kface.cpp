#include "kface.h"

namespace kface {
    
void KFace::setImage(const QImage& image)
{
    QImage greyImage = image.convertToFormat(QImage::Format_Indexed8);
    IplImage *faceImage;
    faceImage = KFaceUtils::QImage2IplImage(&greyImage);
    this->setFace(faceImage);
}

QImage* KFace::getImage()
{
    return KFaceUtils::IplImage2QImage(this->getFace());
}

KFace& KFace::operator=( KFace& other)
{
    this->setX1(other.getX1());
    this->setX2(other.getX2());
    this->setY1(other.getY1());
    this->setY2(other.getY2());
    this->setId(other.getId());
    this->setFace(other.getFace());
    
    return *this;
}

KFace& KFace::operator=( Face& other)
{
    this->setX1(other.getX1());
    this->setX2(other.getX2());
    this->setY1(other.getY1());
    this->setY2(other.getY2());
    this->setId(other.getId());
    this->setFace(other.getFace());
    
    return *this;
}

void KFace::setRect(const QRect& rect)
{
    QPoint topLeft = rect.topLeft();
    QPoint bottomRight = rect.bottomRight();
    this->setX1(topLeft.x());
    this->setY1(topLeft.y());
    this->setX2(bottomRight.x());
    this->setY2(bottomRight.y());
}

QRect KFace::getRect()
{
    QPoint topLeft(this->getX1(), this->getY1());
    QPoint bottomRight(this->getX2(), this->getY2());
    return QRect(topLeft, bottomRight);
}

};