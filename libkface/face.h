/** ===========================================================
 * @file
 *
 * This file is a part of digiKam project
 * <a href="http://www.digikam.org">http://www.digikam.org</a>
 *
 * @date   2010-06-16
 * @brief  This is a derivative of Face class
 *
 * @author Copyright (C) 2010-2011 by Marcel Wiesweg
 *         <a href="mailto:marcel dot wiesweg at gmx dot de">marcel dot wiesweg at gmx dot de</a>
 * @author Copyright (C) 2010-2011 by Aditya Bhatt
 *         <a href="mailto:adityabhatt1991 at gmail dot com">adityabhatt1991 at gmail dot com</a>
 * @author Copyright (C) 2010-2012 by Gilles Caulier
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

#ifndef KFACE_FACE_H
#define KFACE_FACE_H

// Qt includes

#include <QtCore/QExplicitlySharedDataPointer>
#include <QtGui/QImage>
#include <QtCore/QString>

// Local includes

#include "image.h"
#include "libkface_export.h"

namespace libface
{
    class Face;
}

namespace KFaceIface
{

/** Return a string version of LibOpenCV release in format "major.minor.patch"
 */
KFACE_EXPORT QString LibOpenCVVersion();

/** Return a string version of LibFace release in format "major.minor.patch"
 */
KFACE_EXPORT QString LibFaceVersion();

/** Return a string version of libkface release
 */
KFACE_EXPORT QString version();

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
    explicit Face(const QRect& rect, const Image& image = Image());

    /**
     * Face constructor which constructs a face object from another Face.
     * @param other The Face object
     */
    Face(const Face& other);

    /** Assignment operator that assigns a KFace's data to another KFace
     * @param other A reference to a KFace object
     * @return A reference to the copied KFace object
     */
    Face& operator=(const Face& other);

    /**
     * Destructor
     */
    ~Face();

    /**
     * This mode determines how the image data stored in the face is treated.
     */
    enum ImageOwnershipMode
    {
        /**
         * Constructing from libface::Face:
         *  Takes ownership of the image data in the face. Not appropriate
         *  if there is another KFaceIface::Face object also holding the same image data.
         *  (typically, if the libface::Face was created from this other KFaceIface::Face).
         *  Note appropriate if any other entity possibly references the image data.
         * Returning a libface::Face:
         *  As the image data stored in libface::Face is const, this mode is almost always
         *  appropriate. Ensure that the KFaceIface object lives longer than the libface::Face.
         */
        ShallowCopy,

        /**
         * Constructing from libface::Face:
         *  If another entity has ownership of the image data.
         * Returning a libface::Face:
         *  Usually no use case
         */
        DeepCopy,

        /**
         * Constructing from libface::Face:
         *  Does not touch the image data in the face. Image is null.
         * Returning a libface::Face:
         *  Returned face has no image data set.
         */
        IgnoreData
    };

   /**
     * Face constructor which constructs a face object from a libface::Face.
     * @param other The libface::Face object.
     */
    static Face fromFace(const libface::Face& other, ImageOwnershipMode mode);
    const ImageData imageData() const;

    /**
     * Will return a libface::Face version of the Face object
     * @return libface::Face version
     */
    libface::Face toFace(ImageOwnershipMode mode = ShallowCopy) const;

    /** Will convert given QImage to an internal IplImage.
     * @param image The QImage to be set as the face image for the KFace object
     */
    void setImage(const Image& image);

    /** Will return a QImage version of the internal face image stored in the KFace object
     * @return The QImage version of the internal face image
     */
    Image image() const;

    /** Will set the co-ordinates of KFace object to the specified rectangle
     * @param rect The QRect rectangle which is to be set as the rectangle for KFace instance
     */
    void setRect(const QRect& rect);

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

    /**
     * Will return the center coordinates of the Face object
     * @return A QPoint version of the center for a face
     */
    QPoint center() const;

    operator QRect() const { return toRect(); }

    /**
     * Clears id and name
     */
    void clearRecognition();

private:

    class FacePriv;
    QExplicitlySharedDataPointer<FacePriv> d;
};

} // namespace KFaceIface

#endif // KFACE_FACE_H
