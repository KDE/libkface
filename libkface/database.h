/** ===========================================================
 * @file
 *
 * This file is a part of digiKam project
 * <a href="http://www.digikam.org">http://www.digikam.org</a>
 *
 * @date  2010-06-16
 * @brief The Database class wraps the libface database
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

#ifndef KFACE_DATABASE_H
#define KFACE_DATABASE_H

// Qt includes

#include <QtCore/QExplicitlySharedDataPointer>
#include <QtCore/QString>
#include <QtCore/QSharedDataPointer>
#include <QtCore/QPair>
#include <QtCore/QHash>
#include <QtGui/QImage>

// Local includes

#include "libkface_export.h"
#include "face.h"
#include "image.h"

namespace KFaceIface
{

class KFACE_EXPORT Database
{

public:

    enum InitFlag
    {
        InitDetection   = 0x01,
        InitRecognition = 0x02,
        InitAll         = InitDetection | InitRecognition
    };
    Q_DECLARE_FLAGS(InitFlags, InitFlag)

    /**
     * Constructor for Database
     * @param flags Flags that specify type of usage for the Database instance.
     * For Detection : InitDetection
     * For Recognition : InitRecognition
     * For All : InitAll
     * @param configurationPath The path where the Database configuration file will be stored.
     * The default configuration path will be located by KStandardDirs by "data", "libkface/database/".
     */
    explicit Database(InitFlags flags = InitAll, const QString& configurationPath = QString());

    /**
     * Copy constructor for database
     * @param other A reference to another database
     */
    Database(const Database& other);

    /**
     * Descructor
     */
    ~Database();

    /**
     * Assignment Operator.
     * @param other The database whose contents are to be assigned.
     */
    Database& operator=(const Database& other);

    /**
     * Explicitly save configuration. Automatically done in destructor.
     */
    void saveConfig();

    /**
     * Scan an image for faces. Return a list with regions possibly
     * containing faces.
     * Note that you can construct a KFaceIface::Image from a QImage, raw data or a file path.
     * @param image The image in which faces are to be detected
     * @return A QList of detected Face's, with the extracted face images loaded into them.
     */
    QList<Face> detectFaces(const Image& image);

    /**
     * Update the training database with a QList of Faces which hold the face images
     * Faces that have not been given any ID by the caller will automatically be given the next available ID,
     * and this ID will be updated in the Face objects.
     * @param faces A QList of Face's, which hold the face image too, for updating the DB.
     * @return False if no faces were updated, due to an argument of size zero
     */
    bool updateFaces(QList<Face>& faces);

    /**
     * Clear the training database for a single name or id.
     * Use with care, this can deleted carefully accumulated data!
     */
    void clearTraining(const QString& name);
    void clearTraining(int id);
    void clearAllTraining();

    /**
     * Function to recognize faces in a QList of Faces which hold the face images.
     * Recognized faces will have their ID's changed in the Face objects
     * @param faces A QList of Face's, which hold the face image too, for recongition.
     * @return A QList of "closeness" of recognized faces, in the same order as the argument;
     * or an empty list, if an error occurred or no recognition data is available.
     */
    QList<double> recognizeFaces(QList<Face>& faces);

    /**
     * Returns the directory path of the config file
     *
     */
    QString configPath() const;

    /**
     * Set the accuracy of Face Detection, in an interval between 0 and 1.
     * 0 means least accuracy, but very high speed. 1 means highest accuracy but very low speed.
     * The default value in libface is automatically set as 0.8.
     * @param value The degree of accuracy in [0;1]
     */
    void setDetectionAccuracy(double value);

    /**
     * Set the detection specificity between 0 and 1.
     * Trades sensitivity vs. specificity:
     * 0 means many faces will be found, but also many false positives.
     * 1 means some faces will not be found, but most returned faces are
     * true positives.
     * This parameter is orthogonal to the speed vs accuracy dimension.
     * Default is 0.8.
     */
    void setDetectionSpecificity(double value);

    /**
     * Get the currently set accuracy and specificity of Face Detection.
     */
    double detectionAccuracy() const;
    double detectionSpecificity() const;

    /**
     * Get the number of people in the database
     * @return Number of unique ID's in the database
     */
    int peopleCount() const;

    /**
     * Returns all ids in the database
     */
    QList<int> allIds() const;

    /**
     * Returns all names in the database
     */
    QStringList allNames() const;

    /**
     * Returns the first name associated with the given id, or
     * a null string. Note there may be multiple names per id.
     */
    QString nameForId(int id) const;

    /**
     * Returns the id for the given name, or -1 if this name is not know.
     */
    int idForName(const QString& name) const;

    /**
     * Returns the recommended size if you want to scale images for detection.
     * Larger images can be passed, but may be downscaled.
     */
    int recommendedImageSizeForDetection(const QSize& availableSize = QSize()) const;

    /**
     * Returns the recommended size if you want to scale images for recognition.
     * Larger images can be passed, but may be downscaled.
     */
    QSize recommendedImageSizeForRecognition(const QSize& availableSize = QSize()) const;

private:

    class DatabasePriv;
    QExplicitlySharedDataPointer<DatabasePriv> d;
};

} // namespace KFaceIface

Q_DECLARE_OPERATORS_FOR_FLAGS(KFaceIface::Database::InitFlags)

#endif // KFACE_DATABASE_H
