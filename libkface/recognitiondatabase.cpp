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

#include "recognitiondatabase.h"

// Qt includes

#include <QMutex>
#include <QMutexLocker>
#include <QSharedData>

// KDE includes

#include <kdebug.h>
#include <kglobal.h>

// Local includes

#include "database.h"

namespace KFaceIface
{

/*
 * What is all this code here about?
 * The RecognitionDatabaseStaticPriv holds a hash to all exising RecognitionDatabase data,
 * mutex protected.
 * When creating a RecognitionDatabase, either a new RecognitionDatabasePriv is created,
 * or an existing one is used.
 * When the last RecognitionDatabase referencing a RecognitionDatabasePriv is destroyed,
 * the RecognitionDatabasePriv is destroyed as well, removing itself from the static hash.
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

    QExplicitlySharedDataPointer<RecognitionDatabase::RecognitionDatabasePriv> database(const QString& key);
    void removeDatabase(const QString& key);

    QString                                                               defaultPath;
    QMutex                                                                mutex;

    // Important: Do not hold an QExplicitlySharedDataPointer here, or the objects will never be freed!
    typedef QHash<QString, RecognitionDatabase::RecognitionDatabasePriv*> DatabaseHash;
    DatabaseHash                                                          databases;
};

K_GLOBAL_STATIC(RecognitionDatabaseStaticPriv, static_d)

class RecognitionDatabase::RecognitionDatabasePriv : public QSharedData
{
public:

    ~RecognitionDatabasePriv()
    {
        static_d->removeDatabase(configPath);
        delete db;
    }

    const QString configPath;
    QMutex        mutex;

    // call under lock
    Database* database() const
    {
        if (!db)
            const_cast<RecognitionDatabasePriv*>(this)->db = new Database(Database::InitRecognition, configPath);
        return db;
    }

    const Database* constDatabase() const
    {
        return db;
    }

private:

    friend class RecognitionDatabaseStaticPriv;

    RecognitionDatabasePriv(const QString& configPath)
        : configPath(configPath), mutex(QMutex::Recursive), db(0)
    {
    }

private:

    Database* db;
};

QExplicitlySharedDataPointer<RecognitionDatabase::RecognitionDatabasePriv> RecognitionDatabaseStaticPriv::database(const QString& path)
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
            QExplicitlySharedDataPointer<RecognitionDatabase::RecognitionDatabasePriv> p(it.value());
            it.value()->ref.deref(); // We incremented above
            return p;
        }
        /* if the original value is 0, it is currently being deleted, but it must be
         * safe to access it, because the destructor has not yet completed - otherwise it'd not be in the hash.
         */
    }
    RecognitionDatabase::RecognitionDatabasePriv* d = new RecognitionDatabase::RecognitionDatabasePriv(configPath);
    databases[configPath]                           = d;
    return QExplicitlySharedDataPointer<RecognitionDatabase::RecognitionDatabasePriv>(d);
}

void RecognitionDatabaseStaticPriv::removeDatabase(const QString& key)
{
    QMutexLocker lock(&mutex);
    databases.remove(key);
}

RecognitionDatabase RecognitionDatabase::addDatabase(const QString& configurationPath)
{
    QExplicitlySharedDataPointer<RecognitionDatabasePriv> d = static_d->database(configurationPath);
    return RecognitionDatabase(d);
}

RecognitionDatabase::RecognitionDatabase()
{
}

RecognitionDatabase::RecognitionDatabase(QExplicitlySharedDataPointer<RecognitionDatabasePriv> d)
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

bool RecognitionDatabase::isNull() const
{
    return !d;
}

bool RecognitionDatabase::updateFaces(QList<Face>& faces)
{
    if (!d)
        return false;
    QMutexLocker lock(&d->mutex);
    return d->database()->updateFaces(faces);
}

QList<double> RecognitionDatabase::recognizeFaces(QList<Face>& faces)
{
    if (!d)
        return QList<double>();
    QMutexLocker lock(&d->mutex);
    return d->database()->recognizeFaces(faces);
}

void RecognitionDatabase::saveConfig()
{
    if (d && d->constDatabase())
    {
        QMutexLocker lock(&d->mutex);
        d->database()->saveConfig();
    }
}

QString RecognitionDatabase::configPath() const
{
    if (!d)
        return QString();
    return d->configPath;
}

int RecognitionDatabase::peopleCount() const
{
    if (!d)
        return 0;
    QMutexLocker lock(&d->mutex);
    return d->database()->peopleCount();
}

QSize RecognitionDatabase::recommendedImageSize(const QSize& availableSize) const
{
    if (!d)
        return QSize();
    QMutexLocker lock(&d->mutex);
    return d->database()->recommendedImageSizeForRecognition(availableSize);
}

void RecognitionDatabase::clearTraining(const QString& name)
{
    if (!d)
        return;
    QMutexLocker lock(&d->mutex);
    return d->database()->clearTraining(name);
}

void RecognitionDatabase::clearTraining(int id)
{
    if (!d)
        return;
    QMutexLocker lock(&d->mutex);
    return d->database()->clearTraining(id);
}

void RecognitionDatabase::clearAllTraining()
{
    if (!d)
        return;
    QMutexLocker lock(&d->mutex);
    return d->database()->clearAllTraining();
}

QList<int> RecognitionDatabase::allIds() const
{
    if (!d)
        return QList<int>();
    QMutexLocker lock(&d->mutex);
    return d->database()->allIds();
}

QStringList RecognitionDatabase::allNames() const
{
    if (!d)
        return QStringList();
    QMutexLocker lock(&d->mutex);
    return d->database()->allNames();
}

QString RecognitionDatabase::nameForId(int id) const
{
    if (!d)
        return QString();
    QMutexLocker lock(&d->mutex);
    return d->database()->nameForId(id);
}

int RecognitionDatabase::idForName(const QString& name) const
{
    if (!d)
        return -1;
    QMutexLocker lock(&d->mutex);
    return d->database()->idForName(name);
}

} // namespace KFaceIface
