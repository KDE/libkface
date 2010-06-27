/** ===========================================================
 *
 * This file is a part of digiKam project
 * <a href="http://www.digikam.org">http://www.digikam.org</a>
 *
 * @date  2010-06-16
 * @brief The Database class wraps the libface database
 *
 * @author Copyright (C) 2010 by Marcel Wiesweg
 *         <a href="mailto:marcel dot wiesweg at gmx dot de">marcel dot wiesweg at gmx dot de</a>
 *         Copyright (C) 2010 by Aditya Bhatt
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

#ifndef KFACE_DATABASE_H
#define KFACE_DATABASE_H

// Qt includes

#include <QExplicitlySharedDataPointer>
#include <QImage>
#include <QString>
#include <QSharedDataPointer>
#include <QPair>
#include <QHash>

// Local includes

#include "libkface_export.h"
#include "kface.h"
#include "image.h"

namespace KFace
{

class DatabasePriv;

class KFACE_EXPORT Database
{
private:

    QExplicitlySharedDataPointer<DatabasePriv> d;

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
     */
    Database(InitFlags flags = InitAll, const QString& configurationPath = QString());

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
     * Function to recognize faces in a QList of Faces which hold the face images.
     * Recognized faces will have their ID's changed in the Face objects
     * @param faces A QList of Face's, which hold the face image too, for recongition.
     * @return A QList of "closeness" of recognized faces, in the same order as the argument
     */
    QList<double> recognizeFaces(QList<Face>& faces);

    /**
     * Returns the directory path of the config file
     *
     */
    QString configPath();

    /**
     * Set the accuracy of Face Detection. This is measured on a 5-point scale, from 1 to 5.
     * 1 means least accuracy, but very high speed. 5 means highest accuracy but very low speed.
     * The default value in libface is automatically set as 3.
     * @param value The degree of accuracy (1 to 5)
     */
    void setDetectionAccuracy(int value);

    /**
     * Get the currently set accuracy of Face Detection on a 5-point scale.
     * 1 means least accuracy, but very high speed. 5 means highest accuracy but very low speed.
     * @return The degree of accuracy (1 to 5)
     */
    int detectionAccuracy();
};

}; // namespace KFace

Q_DECLARE_OPERATORS_FOR_FLAGS(KFace::Database::InitFlags)

#endif // KFACE_DATABASE_H
