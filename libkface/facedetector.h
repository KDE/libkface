/** ===========================================================
 * @file
 *
 * This file is a part of digiKam project
 * <a href="http://www.digikam.org">http://www.digikam.org</a>
 *
 * @date  2010-09-02
 * @brief A convenience class for a standalone face detector
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

#ifndef KFACE_FACEDETECTOR_H
#define KFACE_FACEDETECTOR_H

// Qt includes

#include <QExplicitlySharedDataPointer>
#include <QImage>
#include <QVariant>

// Local includes

#include "libkface_export.h"

namespace KFaceIface
{

class KFACE_EXPORT FaceDetector
{

public:

    /**
     * Provides face detection, that means the process of selecting
     * those regions of a full image which contain face.
     *
     * This class provides shallow copying
     * The class is fully reentrant (a single object and its copies are not thread-safe).
     * Deferred creation is guaranteed, that means creation of a FaceDetector
     * object is cheap, the expensive creation of the detection backend
     * is performed when detectFaces is called for the first time.
     */

    FaceDetector();
    FaceDetector(const FaceDetector& other);
    ~FaceDetector();

    QString backendIdentifier() const;

    FaceDetector& operator=(const FaceDetector& other);

    /**
     * Scan an image for faces. Return a list with regions possibly
     * containing faces.
     * If the image has been downscaled anywhere in the process,
     * provide the original size of the image as this may be of importance in the detection process.
     *
     * Found faces are returned in relative coordinates.
     */
    QList<QRectF> detectFaces(const QImage& image, const QSize& originalSize = QSize());

    /**
     * Tunes backend parameters.
     */
    void setParameter(const QString& parameter, const QVariant& value);
    void setParameters(const QVariantMap& parameters);
    QVariantMap parameters() const;

    /**
     * Returns the recommended size if you want to scale images for detection.
     * Larger images can be passed, but may be downscaled.
     */
    int recommendedImageSize(const QSize& availableSize = QSize()) const;

    static QRectF toRelativeRect(const QRect& absoluteRect, const QSize& size);
    static QRect toAbsoluteRect(const QRectF& relativeRect, const QSize& size);
    static QList<QRectF> toRelativeRects(const QList<QRect>& absoluteRects, const QSize& size);
    static QList<QRect> toAbsoluteRects(const QList<QRectF>& relativeRects, const QSize& size);

private:

    class Private;
    QExplicitlySharedDataPointer<Private> d;
};

} // namespace KFaceIface

#endif // KFACE_FACEDETECTOR_H
