/** ===========================================================
 *
 * This file is a part of digiKam project
 * <a href="http://www.digikam.org">http://www.digikam.org</a>
 *
 * @date   2010-06-16
 * @brief  This is a derivative of Face class
 *
 * @author Copyright (C) 2010 by Marcel Wiesweg
 *         <a href="mailto:marcel dot wiesweg at gmx dot de">marcel dot wiesweg at gmx dot de</a>
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

// Libface includes

#include <libface/Face.h>
#include <libface/LibFaceConfig.h>

// Local includes

#include "kface.h"
#include "kfaceutils.h"
#include "image.h"
#include "image_p.h"
#include "version.h"

namespace KFaceIface
{

class Face::FacePriv : public QSharedData
{
public:

    FacePriv()
    {
    }

    ~FacePriv()
    {
        // we handle releasing the image
        face.setFace(0);
    }

    libface::Face face;
    QString       name;
    Image         image;
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
    : d(other.d)
{
}

Face::~Face()
{
}

Face& Face::operator=(const Face& other)
{
    d = other.d;
    return *this;
}

libface::Face &Face::face() const
{
    return d->face;
}

int Face::id() const
{
    return d->face.getId();
}

QString Face::name() const
{
    return d->name;
}

void Face::setName(const QString& name)
{
    d->name = name;
}

void Face::setId(int id)
{
    d->face.setId(id);
}

void Face::setImage(const Image& image)
{
    d->image = image;
    d->face.setFace(image.imageData());
}

QImage Face::getImage() const
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

Face& Face::operator=(const libface::Face& face)
{
    setFace(face);
    return *this;
}

void Face::setRect(const QRect& rect)
{
    QPoint topLeft     = rect.topLeft();
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

QPoint Face::center() const
{
    return this->toRect().center();
}

void Face::clearRecognition()
{
    setId(-1);
    d->name = QString();
}

// Static methods ---------------------------------------------------------

QString LibOpenCVVersion()
{
    return QString("%1").arg(CV_VERSION);
}

QString LibFaceVersion()
{
    return QString("%1.%2").arg(LibFace_VERSION_MAJOR)
                           .arg(LibFace_VERSION_MINOR);
}

QString version()
{
    return QString(kface_version);
}

} // namespace KFaceIface
