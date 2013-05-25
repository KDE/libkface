/** ===========================================================
 * @file
 *
 * This file is a part of digiKam project
 * <a href="http://www.digikam.org">http://www.digikam.org</a>
 *
 * @date  2010-09-02
 * @brief Wrapper class for face recongition
 *
 * @author Copyright (C) 2010-2013 by Marcel Wiesweg
 *         <a href="mailto:marcel dot wiesweg at gmx dot de">marcel dot wiesweg at gmx dot de</a>
 * @author Copyright (C) 2010 by Aditya Bhatt
 *         <a href="mailto:adityabhatt1991 at gmail dot com">adityabhatt1991 at gmail dot com</a>
 * @author Copyright (C) 2010-2013 by Gilles Caulier
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

#include <QExplicitlySharedDataPointer>
#include <QImage>
#include <QList>
#include <QMap>
#include <QVariant>

// Local includes

#include "libkface_export.h"
#include "identity.h"
#include "dataproviders.h"

namespace KFaceIface
{

class KFACE_EXPORT RecognitionDatabase
{

public:

    /**
     * Performs face recogition.
     * Persistent data about identities and training data will be stored
     * under a given or the default configuration path.
     *
     * The class guarantees
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

    // ------------ Identity management --------------

    /**
     * Returns all identities known to the database
     */
    QList<Identity> allIdentities();
    Identity identity(int id);

    /**
     * Finds the first identity with matching attribute - value.
     */
    Identity findIdentity(const QString& attribute, const QString& value);

    /**
     * Adds a new identity with the specified attributes.
     */
    Identity addIdentity(const QMap<QString, QString>& attributes);

    /**
     * Adds or sets, resp., the attributes of an identity.
     */
    void addIdentityAttributes(int id, const QMap<QString, QString>& attributes);
    void addIdentityAttribute(int id, const QString& attribute, const QString& value);
    void setIdentityAttributes(int id, const QMap<QString, QString>& attributes);

    // ------------ backend parameters --------------

    /// A textual, informative identifier of the backend in use.
    QString backendIdentifier() const;

    /**
     * Tunes backend parameters.
     */
    void setParameter(const QString& parameter, const QVariant& value);
    void setParameters(const QVariantMap& parameters);
    QVariantMap parameters() const;

    // ------------ Recognition, clustering and training --------------

    /**
     * Returns the recommended size if you want to scale face images for recognition.
     * Larger images can be passed, but may be downscaled.
     */
    int recommendedImageSize(const QSize& availableSize = QSize()) const;

    /**
     * Performs recogition.
     * The face details to be recognized are passed by the provider.
     * For each entry in the provider, in 1-to-1 mapping,
     * a recognized identity or the null identity is returned.
     */
    QList<Identity> recognizeFaces(ImageListProvider* images);
    QList<Identity> recognizeFaces(const QList<QImage>& images);
    Identity recognizeFace(const QImage& image);

    enum TrainingCostHint
    {
        /// Training is so cheap that new photos for training can be passed any time
        TrainingIsCheap,
        /// Training is significantly optimized if new images are received in batches
        /// instead training single images multiple times
        TrainingLikesBatches,
        /// Training is a computing intensive operation.
        /// By choice of the application, it may be manually triggered by the user.
        TrainingIsExpensive
    };

    /// Gives a hint about the complexity of training for the current backend.
    TrainingCostHint trainingCostHint() const;

    /**
     * Performs training.
     * The identities which have new images to be trained are given.
     * An empty list means that all identities are checked.
     *
     * All needed data will be queried from the provider.
     *
     * An identifier for the current training context is given,
     * which can identify the application or group of collections.
     * (It is assumed that training from different contexts is based on
     * non-overlapping collections of images. Keep is always constant for your app.)
     */

    void train(const QList<Identity>& identitiesToBeTrained, TrainingDataProvider* data,
               const QString& trainingContext);
    void train(const Identity& identityToBeTrained, TrainingDataProvider* data,
               const QString& trainingContext);

    void clearAllTraining(const QString& trainingContext = QString());

private:

    class Private;

    explicit RecognitionDatabase(QExplicitlySharedDataPointer<Private> d);

    QExplicitlySharedDataPointer<Private> d;

    friend class RecognitionDatabaseStaticPriv;
};

/**
 * Return a string version of LibOpenCV release in format "major.minor.patch"
 */
KFACE_EXPORT QString LibOpenCVVersion();

/**
 * Return a string version of libkface release
 */
KFACE_EXPORT QString version();

} // namespace KFaceIface

#endif // KFACE_RECOGNITIONDATABASE_H
