/** ===========================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * @date  : 2010-06-16
 * @note  : This is a derivative of Face class
 *
 * @author: Copyright (C) 2010 by Marcel Wiesweg <marcel.wiesweg at gmx dot de>
 *          Copyright (C) 2010 by Aditya Bhatt <adityabhatt1991 at gmail dot com>
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

#ifndef KFACE_FACE_H
#define KFACE_FACE_H

// Qt includes

#include <QImage>

// Local includes

#include "libkface_export.h"

namespace libface
{
    class Face;
}

namespace KFace
{

class FacePriv;

class KFACE_EXPORT Face
{

public:

    Face();
    Face(const QRect& rect, const QImage& image = QImage());
    Face(const libface::Face& other);

    Face(const Face& other);
    ~Face();

    /** Will convert given QImage to an internal IplImage.
     * @param image The QImage to be set as the face image for the KFace object
     */
    void setImage(const QImage& image);

    /** Will return a QImage version of the internal face image stored in the KFace object
     * @return The QImage version of the internal face image
     */
    QImage getImage();

    /** Will set the co-ordinates of KFace object to the specified rectangle
     * @param rect The QRect rectangle which is to be set as the rectangle for KFace instance
     */
    void setRect(const QRect& rect);

    void setFace(const libface::Face& face);

    int id() const;
    void setId(int id);

    /** Will return a QRect of the KFace object, for better interop with functions that don't want to directly use KFace
     * @return A QRect version of the bounding box for a face
     */
    QRect toRect() const;
    operator QRect() const { return toRect(); }

    libface::Face &face() const;
    operator libface::Face&() const { return face(); }

    /** Assignment operator that assigns a KFace's data to another KFace
     * @param other A reference to a KFace object
     * @return A reference to the copied KFace object
     */
    Face& operator=(const Face& other);

    /** Assignment operator that assigns a Face's data to another KFace
     * @param other A reference to a Face object
     * @return A reference to the copied KFace object
     */
    Face& operator=(const libface::Face& other);

public:

    /** Return a string version of LibOpenCV release in format "major.minor.patch"
     */
    static QString LibOpenCVVersion();

    /** Return a string version of LibFace release in format "major.minor.patch"
     */
    static QString LibFaceVersion();

    /** Return a string version of libkface release
     */
    static QString version();

private:

    FacePriv* const d;
};

}; // namespace KFace

#endif // KFACE_FACE_H
