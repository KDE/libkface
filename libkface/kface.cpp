/**
* This file is part of libkface.
*
* libkface is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 2 of the License, or
* (at your option) any later version.
*
* libkface is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with libkface.  If not, see <http://www.gnu.org/licenses/>.
*
* @note: This is a derivative of Face class
* @author: Aditya Bhatt, Marcel Wiesweg
*/

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
    d->face.setId(face.getId());
    d->face.setFace(face.getFace());
}

Face& Face::operator=(const Face& other)
{
    d->face.setX1(other.d->face.getX1());
    d->face.setX2(other.d->face.getX2());
    d->face.setY1(other.d->face.getY1());
    d->face.setY2(other.d->face.getY2());
    d->face.setId(other.d->face.getId());
    d->face.setFace(other.d->face.getFace());

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