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
#include <QString>

// Local includes

#include "libkface_export.h"

namespace libface
{
    class Face;
}

namespace KFaceIface
{

class FacePriv;

class KFACE_EXPORT Face
{

public:

    /** 
     * Face constructor
     */
    Face();

    /** 
     * Face constructor which constructs a face object out of a QRect and optionally a QImage.
     * If no image is passed, it sets a blank image made by QImage()
     * @param rect The QRect for the Face object
     * @param image The QImage for the face image
     */
    Face(const QRect& rect, const QImage& image = QImage());

    /** 
     * Face constructor which constructs a face object from a libface::Face.
     * @param other The libface::Face object.
     */
    Face(const libface::Face& other);

    /** 
     * Face constructor which constructs a face object from another Face.
     * @param other The Face object
     */
    Face(const Face& other);

    /** 
     * Destructor
     */
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

    /** Will set a face object from a libface::Face.
     * @param face The libface::Face object.
     */
    void setFace(const libface::Face& face);

    /**
     * Returns integer ID of the Face
     * @return ID
     */
    int id() const;

    /**
     * Returns the name of the Face
     * @return A QString name of the Face
     */
    QString name() const;

    /**
     * Set the integer ID of the Face
     * @param id The ID
     */
    void setId(int id);

    /**
     * Set the name of the Face
     * @param name The name (QString)
     */
    void setName(const QString& name);

    /** 
     * Will return a QRect of the Face object, for better interop with functions that don't want to directly use Face
     * @return A QRect version of the bounding box for a face
     */
    QRect toRect() const;
    operator QRect() const { return toRect(); }

    /** 
     * Will return a libface::Face version of the Face object
     * @return libface::Face version
     */
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

}; // namespace KFaceIface

#endif // KFACE_FACE_H
