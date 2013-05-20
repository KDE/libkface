/** ===========================================================
 * @file
 *
 * This file is a part of digiKam project
 * <a href="http://www.digikam.org">http://www.digikam.org</a>
 *
 * @date   2010-06-16
 * @brief  The RecognitionDatabase class wraps the libface database
 *
 * @author Copyright (C) 2010 by Marcel Wiesweg
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

#include "recognitiondatabase.h"

// Qt includes

#include <QMutex>
#include <QMutexLocker>
#include <QSharedData>
#include <QUuid>

// KDE includes

#include <kdebug.h>
#include <kglobal.h>
#include <kstandarddirs.h>

// Local includes

#include "databaseaccess.h"
#include "databaseoperationgroup.h"
#include "databaseparameters.h"
#include "recognition-opentld/opentldfacerecognizer.h"
#include "trainingdb.h"
#include "version.h"

namespace KFaceIface
{


Identity::Identity()
    : id(-1)
{
}

bool Identity::isNull() const
{
    return id == -1;
}

bool Identity::operator==(const Identity& other) const
{
    return id == other.id;
}

/**
 * The RecognitionDatabaseStaticPriv holds a hash to all exising RecognitionDatabase data,
 * mutex protected.
 * When creating a RecognitionDatabase, either a new Private is created,
 * or an existing one is used.
 * When the last RecognitionDatabase referencing a Private is destroyed,
 * the Private is destroyed as well, removing itself from the static hash.
 */
class RecognitionDatabaseStaticPriv
{
public:

    RecognitionDatabaseStaticPriv()
        : mutex(QMutex::Recursive)
    {
        // Note: same line in Database. Keep in sync.
        defaultPath = KStandardDirs::locateLocal("data", "libkface/database/", true);
    }

    QExplicitlySharedDataPointer<RecognitionDatabase::Private> database(const QString& key);
    void removeDatabase(const QString& key);

public:

    QString                                               defaultPath;
    QMutex                                                mutex;

    // Important: Do not hold an QExplicitlySharedDataPointer here, or the objects will never be freed!
    typedef QHash<QString, RecognitionDatabase::Private*> DatabaseHash;
    DatabaseHash                                          databases;
};

K_GLOBAL_STATIC(RecognitionDatabaseStaticPriv, static_d)

// -------------------------------------------------------------------------------------------------

class RecognitionDatabase::Private : public QSharedData
{
public:

    ~Private();

    const QString       configPath;
    QMutex              mutex;
    DatabaseAccessData *db;

    QVariantMap         parameters;
    QList<Identity>     identityCache;

public:

    template <class T>
    T* getObjectOrCreate(T* &ptr)
    {
        if (!ptr)
        {
            ptr = new T(db);
        }
        return ptr;
    }

    OpenTLDFaceRecognizer* openTLD() { return getObjectOrCreate(opentld); }
    OpenTLDFaceRecognizer* openTLDConst() const { return opentld; }

    void applyParameters();

private:

    friend class RecognitionDatabaseStaticPriv;
    // Protected creation by StaticPriv only
    Private(const QString& configPath);

private:

    OpenTLDFaceRecognizer* opentld;
};

QExplicitlySharedDataPointer<RecognitionDatabase::Private> RecognitionDatabaseStaticPriv::database(const QString& path)
{
    QMutexLocker lock(&mutex);
    QString configPath        = path.isNull() ? defaultPath : path;
    DatabaseHash::iterator it = databases.find(configPath);

    if (it != databases.end())
    {
        /* There is a race condition: The last Priv is dereferenced, the destructor called.
         * Now database() has been called and locks the mutex after this dereferencing, but before removeDatabase is executed.
         * So we only can use this cached data if its reference count is non-zero.
         * Atomically to testing, we increase the reference count to reserve it for our usage.
         */
        if (it.value()->ref.fetchAndAddOrdered(1) != 0)
        {
            QExplicitlySharedDataPointer<RecognitionDatabase::Private> p(it.value());
            it.value()->ref.deref(); // We incremented above
            return p;
        }
        /* if the original value is 0, it is currently being deleted, but it must be
         * safe to access it, because the destructor has not yet completed - otherwise it'd not be in the hash.
         */
    }

    RecognitionDatabase::Private* const d = new RecognitionDatabase::Private(configPath);
    databases[configPath]                 = d;

    return QExplicitlySharedDataPointer<RecognitionDatabase::Private>(d);
}

void RecognitionDatabaseStaticPriv::removeDatabase(const QString& key)
{
    QMutexLocker lock(&mutex);
    databases.remove(key);
}

// ----------------------------------------------------------------------------------------------

RecognitionDatabase::Private::Private(const QString& configPath)
    : configPath(configPath),
      mutex(QMutex::Recursive),
      db(DatabaseAccess::create()),
      opentld(0)
{
    DatabaseParameters params = DatabaseParameters::parametersForSQLite(configPath + "/" + "recognition.db");
    DatabaseAccess::setParameters(db, params);
    if (DatabaseAccess::checkReadyForUse(db))
    {
        identityCache = DatabaseAccess(db).db()->identities();
    }
}

RecognitionDatabase::Private::~Private()
{
    static_d->removeDatabase(configPath);
    DatabaseAccess::destroy(db);
}


RecognitionDatabase::RecognitionDatabase()
{
}

RecognitionDatabase::RecognitionDatabase(QExplicitlySharedDataPointer<Private> d)
    : d(d)
{
}

RecognitionDatabase::RecognitionDatabase(const RecognitionDatabase& other)
{
    d = other.d;
}

RecognitionDatabase& RecognitionDatabase::operator=(const KFaceIface::RecognitionDatabase& other)
{
    d = other.d;
    return *this;
}

RecognitionDatabase::~RecognitionDatabase()
{
    // saveConfig() called from KFace::Database Priv destructor
}

RecognitionDatabase RecognitionDatabase::addDatabase(const QString& configurationPath)
{
    QExplicitlySharedDataPointer<Private> d = static_d->database(configurationPath);
    return RecognitionDatabase(d);
}

bool RecognitionDatabase::isNull() const
{
    return !d;
}

QList<Identity> RecognitionDatabase::allIdentities()
{
    if (!d)
        return QList<Identity>();

    QMutexLocker lock(&d->mutex);
    return d->identityCache;
}

Identity RecognitionDatabase::identity(int id)
{
    if (!d)
        return Identity();

    QMutexLocker lock(&d->mutex);

    foreach (const Identity& identity, d->identityCache)
    {
        if (identity.id == id)
        {
            return identity;
        }
    }
    return Identity();
}

Identity RecognitionDatabase::findIdentity(const QString& attribute, const QString& value)
{
    if (!d)
        return Identity();

    QMutexLocker lock(&d->mutex);

    foreach (const Identity& identity, d->identityCache)
    {
        if (identity.attributes.value(attribute) == value)
        {
            return identity;
        }
    }
    return Identity();
}

Identity RecognitionDatabase::addIdentity(const QMap<QString, QString>& attributes)
{
    if (!d)
        return Identity();

    QMutexLocker lock(&d->mutex);

    Identity identity;
    {
        DatabaseOperationGroup group(d->db);
        int id = DatabaseAccess(d->db).db()->addIdentity();
        identity.id = id;
        identity.attributes = attributes;
        identity.attributes["uuid"] = QUuid::createUuid().toString();
        DatabaseAccess(d->db).db()->updateIdentity(identity);
    }

    d->identityCache << identity;
    return identity;
}

void RecognitionDatabase::addIdentityAttributes(int id, const QMap<QString, QString>& attributes)
{
    if (!d)
        return;

    QMutexLocker lock(&d->mutex);

    for (QList<Identity>::iterator it = d->identityCache.begin(); it != d->identityCache.end(); ++it)
    {
        if (it->id == id)
        {
            it->attributes.unite(attributes);
            DatabaseAccess(d->db).db()->updateIdentity(*it);
            return;
       }
    }
}

void RecognitionDatabase::addIdentityAttribute(int id, const QString& attribute, const QString& value)
{
    if (!d)
        return;

    QMutexLocker lock(&d->mutex);

    for (QList<Identity>::iterator it = d->identityCache.begin(); it != d->identityCache.end(); ++it)
    {
        if (it->id == id)
        {
            it->attributes.insertMulti(attribute, value);
            DatabaseAccess(d->db).db()->updateIdentity(*it);
            return;
       }
    }
}

void RecognitionDatabase::setIdentityAttributes(int id, const QMap<QString, QString>& attributes)
{
   if (!d)
        return;

    QMutexLocker lock(&d->mutex);

    for (QList<Identity>::iterator it = d->identityCache.begin(); it != d->identityCache.end(); ++it)
    {
        if (it->id == id)
        {
            it->attributes = attributes;
            DatabaseAccess(d->db).db()->updateIdentity(*it);
            return;
       }
    }
}

QString RecognitionDatabase::backendIdentifier() const
{
    return "opentld";
}

void RecognitionDatabase::Private::applyParameters()
{
    if (opentld)
    {
        for (QVariantMap::const_iterator it = parameters.begin(); it != parameters.end(); ++it)
        {
            if (it.key() == "threshold" || it.key() == "accuracy")
            {
                openTLD()->setThreshold(it.value().toFloat());
            }
        }
    }
}

void RecognitionDatabase::setParameter(const QString& parameter, const QVariant& value)
{
   if (!d)
        return;

    QMutexLocker lock(&d->mutex);

    d->parameters.insert(parameter, value);
    d->applyParameters();
}

void RecognitionDatabase::setParameters(const QVariantMap& parameters)
{
   if (!d)
        return;

    QMutexLocker lock(&d->mutex);

    for (QVariantMap::const_iterator it = parameters.begin(); it != parameters.end(); ++it)
    {
        d->parameters.insert(it.key(), it.value());
    }
    d->applyParameters();
}

QVariantMap RecognitionDatabase::parameters() const
{
   if (!d)
        return QVariantMap();

    QMutexLocker lock(&d->mutex);

    return d->parameters;
}

int RecognitionDatabase::recommendedImageSize(const QSize& availableSize) const
{
    // hardcoded for now, change when we know better.
    Q_UNUSED(availableSize)
    return 256;
}

Identity RecognitionDatabase::recognizeFace(const QImage& image)
{
    QList<Identity> result = recognizeFaces(QList<QImage>() << image);
    return result.first();
}

QList<Identity> RecognitionDatabase::recognizeFaces(const QList<QImage>& images)
{
    QListImageListProvider provider(images);
    return recognizeFaces(&provider);
}

QList<Identity> RecognitionDatabase::recognizeFaces(ImageListProvider* images)
{
   if (!d)
        return QList<Identity>();

    QMutexLocker lock(&d->mutex);

    QList<Identity> result;
    for (; images->hasNext(); images->proceed())
    {
        cv::Mat cvImage = d->openTLD()->prepareForRecognition(images->image());
        int id = d->openTLD()->recognize(cvImage);
        if (id == -1)
        {
            result << Identity();
        }
        else
        {
            result << d->identityCache.value(id);
        }
    }

    return result;
}

RecognitionDatabase::TrainingCostHint RecognitionDatabase::trainingCostHint() const
{
    return TrainingIsCheap;
}

void RecognitionDatabase::train(const Identity& identityToBeTrained, TrainingDataProvider* data,
                                const QString& trainingContext)
{
    train(QList<Identity>() << identityToBeTrained, data, trainingContext);
}

void RecognitionDatabase::train(const QList<Identity>& identitiesToBeTrained, TrainingDataProvider* data,
                                const QString& trainingContext)
{
   if (!d)
        return;

   QMutexLocker lock(&d->mutex);

   foreach (const Identity& identity, identitiesToBeTrained)
   {
       ImageListProvider* images = data->newImages(identity);
       for (; images->hasNext(); images->proceed())
       {
           cv::Mat cvImage = d->openTLD()->prepareForRecognition(images->image());
           d->openTLD()->train(identity.id, cvImage);
       }
   }
}

void RecognitionDatabase::clearAllTraining(const QString& trainingContext)
{
   if (!d)
        return;

   QMutexLocker lock(&d->mutex);
   //TODO
}

// --- Runtime version info static methods (declared in version.h)

QString LibOpenCVVersion()
{
    return QString("%1").arg(CV_VERSION);
}

QString version()
{
    return QString(kface_version);
}


} // namespace KFaceIface
