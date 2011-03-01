/** ===========================================================
 * @file
 *
 * This file is a part of digiKam project
 * <a href="http://www.digikam.org">http://www.digikam.org</a>
 *
 * @date  2010-09-02
 * @brief Wrapper class for face recongition
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

#ifndef KFACE_RECOGNITIONDATABASE_H
#define KFACE_RECOGNITIONDATABASE_H

// Qt includes

#include <QtCore/QExplicitlySharedDataPointer>

// KDE includes

#include <kstandarddirs.h>

// Local includes

#include "libkface_export.h"
#include "face.h"
#include "image.h"

namespace KFaceIface
{

class KFACE_EXPORT RecognitionDatabase
{

public:

    /**
     * A wrapper around Database for face recongition.
     * Additionally provides the following guarantees:
     * - deferred creation: The backend is created only when used first.
     * - only one instance per configuration path is created
     * - an instance of this class is thread-safe
     *   (this class is also reentrant, for different objects and paths)
     */

    /**
     * Returns an instance of RecognitionDatabase for the given configuration path.
     * When called multiple times with the same path, will return the same database.
     * The database is closed and configuration written after the last object is destroyed.
     * @param configurationPath The path where the RecognitionDatabase configuration file will be stored.
     * If null, a default path is located by KStandardDirs.
     */
    static RecognitionDatabase addDatabase(const QString& configurationPath = QString());

    /// Constructs a null database
    RecognitionDatabase();

    RecognitionDatabase(const RecognitionDatabase& other);
    ~RecognitionDatabase();
    RecognitionDatabase& operator=(const RecognitionDatabase& other);

    bool isNull() const;

    /**
     * Explicitly save configuration. Automatically done in destructor.
     */
    void saveConfig();

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
     * @return A QList of "closeness" of recognized faces, in the same order as the argument;
     * or an empty list, if an error occurred or no recognition data is available.
     */
    QList<double> recognizeFaces(QList<Face>& faces);

    /**
     * Clear the training database for a single name or id.
     * Use with care, this can deleted carefully accumulated data!
     */
    void clearTraining(const QString& name);
    void clearTraining(int id);
    void clearAllTraining();

    /**
     * Returns the directory path of the config file
     *
     */
    QString configPath() const;

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
     * Returns the recommended size if you want to scale images for recognition.
     * Larger images can be passed, but may be downscaled.
     */
    QSize recommendedImageSize(const QSize& availableSize = QSize()) const;

private:

    class RecognitionDatabasePriv;

    RecognitionDatabase(QExplicitlySharedDataPointer<RecognitionDatabasePriv> d);

    QExplicitlySharedDataPointer<RecognitionDatabasePriv> d;

    friend class RecognitionDatabaseStaticPriv;
};

} // namespace KFaceIface

#endif // KFACE_RECOGNITIONDATABASE_H
