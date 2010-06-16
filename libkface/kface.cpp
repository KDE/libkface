
#include <libface/Face.h>

#include "kface.h"
#include "kfaceutils.h"

namespace KFace {

class FacePriv
{
public:

    FacePriv()
    {
    }

    libface::Face face;
};

Face::Face()
            : d(new FacePriv)
{
}

Face::Face(const QRect& rect, const QImage& image)
            : d(new FacePriv)
{
    setRect(rect);
    setImage(image);
}

Face::Face(const libface::Face& other)
            : d(new FacePriv)
{
    operator=(other);
}

Face::Face(const Face& other)
            : d(new FacePriv)
{
    operator=(other);
}

Face::~Face()
{
    delete d;
}

libface::Face &Face::face() const
{
    return d->face;
}

int Face::id() const
{
    return d->face.getId();
}

void Face::setId(int id)
{
    d->face.setId(id);
}

void Face::setImage(const QImage& image)
{
    QImage greyImage = image.convertToFormat(QImage::Format_Indexed8);
    IplImage *faceImage;
    faceImage = KFaceUtils::QImage2IplImage(greyImage);
    d->face.setFace(faceImage);
}

QImage Face::getImage()
{
    return KFaceUtils::IplImage2QImage(d->face.getFace());
}

void Face::setFace(const libface::Face& face)
{
    d->face.setX1(face.getX1());
    d->face.setX2(face.getX2());
    d->face.setY1(face.getY1());
    d->face.setY2(face.getY2());
    d->face.setId(libface::Face(face).getId()); //FIXME: method must be const in libface
    d->face.setFace(libface::Face(face).getFace()); //FIXME: method must be const in libface
}

Face& Face::operator=(const Face& other)
{
    d->face.setX1(other.d->face.getX1());
    d->face.setX2(other.d->face.getX2());
    d->face.setY1(other.d->face.getY1());
    d->face.setY2(other.d->face.getY2());
    d->face.setId(libface::Face(other.d->face).getId()); //FIXME: method must be const in libface
    d->face.setFace(libface::Face(other.d->face).getFace()); //FIXME: method must be const in libface

    return *this;
}

Face& Face::operator=(const libface::Face& face)
{
    setFace(face);
    return *this;
}

void Face::setRect(const QRect& rect)
{
    QPoint topLeft = rect.topLeft();
    QPoint bottomRight = rect.bottomRight();
    d->face.setX1(topLeft.x());
    d->face.setY1(topLeft.y());
    d->face.setX2(bottomRight.x());
    d->face.setY2(bottomRight.y());
}

QRect Face::toRect() const
{
    QPoint topLeft(d->face.getX1(), d->face.getY1());
    QPoint bottomRight(d->face.getX2(), d->face.getY2());
    return QRect(topLeft, bottomRight);
}

};