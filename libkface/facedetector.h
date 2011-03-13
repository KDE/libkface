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

#include <QtCore/QExplicitlySharedDataPointer>
#include <QtCore/QPair>
#include <QtCore/QHash>

// Local includes

#include "libkface_export.h"
#include "face.h"
#include "image.h"

namespace KFaceIface
{

class KFACE_EXPORT FaceDetector
{

public:

    /** This class is a wrapper around the Database object
     *  providing the following additional guarantees:
     *  - deferred creation: The detection backend is created only
     *    when detectFaces is called for the first time
     *  - reentrancy: This class is reentrant.
     *    (Note: An object of this class is not thread-safe)
     */

    FaceDetector();
    FaceDetector(const FaceDetector& other);
    ~FaceDetector();

    FaceDetector& operator=(const FaceDetector& other);

    /**
     * Scan an image for faces. Return a list with regions possibly
     * containing faces.
     * @param image The image in which faces are to be detected
     * @return A QList of detected Face's, with the extracted face images loaded into them.
     */
    QList<Face> detectFaces(const Image& image);

    /**
     * Set the accuracy and specificity of Face Detection.
     */
    void   setAccuracy(double value);
    double accuracy() const;
    void   setSpecificity(double value);
    double specificity() const;

    /**
     * Returns the recommended size if you want to scale images for detection.
     * Larger images can be passed, but may be downscaled.
     */
    int recommendedImageSize(const QSize& availableSize = QSize()) const;

private:

    class FaceDetectorPriv;
    QExplicitlySharedDataPointer<FaceDetectorPriv> d;
};

} // namespace KFaceIface

#endif // KFACE_FACEDETECTOR_H
