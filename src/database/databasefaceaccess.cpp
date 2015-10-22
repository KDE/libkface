/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2007-03-18
 * Description : Face database access wrapper.
 *
 * Copyright (C) 2007-2008 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "databasefaceaccess.h"

// Qt includes

#include <QMutex>
#include <QSqlDatabase>

// Local includes

#include "libkface_debug.h"
#include "databasecorebackend.h"
#include "trainingdb.h"
#include "databasefaceschemaupdater.h"

namespace KFaceIface
{

class DatabaseFaceAccessData
{
public:

    DatabaseFaceAccessData()
        : backend(0),
          db(0),
          lastError(QSqlError::NoError),
          initializing(false)
    {
    }

    ~DatabaseFaceAccessData()
    {
    };

public:

    DatabaseCoreBackend*   backend;
    TrainingDB*            db;
    DatabaseFaceParameters parameters;
    DatabaseLocking        lock;
    int                    lastError;
    bool                   initializing;
};

// ----------------------------------------------------------------

class DatabaseFaceAccessMutexLocker : public QMutexLocker
{
public:

    DatabaseFaceAccessMutexLocker(DatabaseFaceAccessData* const d)
        : QMutexLocker(&d->lock.mutex), d(d)
    {
        d->lock.lockCount++;
    }

    ~DatabaseFaceAccessMutexLocker()
    {
        d->lock.lockCount--;
    }

    DatabaseFaceAccessData* const d;
};

// ----------------------------------------------------------------

DatabaseFaceAccessData* DatabaseFaceAccess::create()
{
    return new DatabaseFaceAccessData;
}

void DatabaseFaceAccess::destroy(DatabaseFaceAccessData* const d)
{
    if (d)
    {
        DatabaseFaceAccessMutexLocker locker(d);
        d->backend->close();
        delete d->db;
        delete d->backend;
    }

    delete d;
}

DatabaseFaceAccess::DatabaseFaceAccess(DatabaseFaceAccessData* const d)
    : d(d)
{
    d->lock.mutex.lock();
    d->lock.lockCount++;

    if (!d->backend->isOpen() && !d->initializing)
    {
        // avoid endless loops
        d->initializing = true;

        d->backend->open(d->parameters);

        d->initializing = false;
    }
}

DatabaseFaceAccess::~DatabaseFaceAccess()
{
    d->lock.lockCount--;
    d->lock.mutex.unlock();
}

DatabaseFaceAccess::DatabaseFaceAccess(bool, DatabaseFaceAccessData* const d)
    : d(d)
{
    // private constructor, when mutex is locked and
    // backend should not be checked
    d->lock.mutex.lock();
    d->lock.lockCount++;
}

TrainingDB* DatabaseFaceAccess::db() const
{
    return d->db;
}

DatabaseCoreBackend* DatabaseFaceAccess::backend() const
{
    return d->backend;
}

DatabaseFaceParameters DatabaseFaceAccess::parameters() const
{
    if (d)
    {
        return d->parameters;
    }

    return DatabaseFaceParameters();
}

void DatabaseFaceAccess::initDatabaseErrorHandler(DatabaseFaceAccessData* const d, DatabaseErrorHandler* const errorhandler)
{
    //DatabaseErrorHandler *errorhandler = new DatabaseGUIErrorHandler(d->parameters);
    d->backend->setDatabaseErrorHandler(errorhandler);
}

void DatabaseFaceAccess::setParameters(DatabaseFaceAccessData* const d, const DatabaseFaceParameters& parameters)
{
    DatabaseFaceAccessMutexLocker lock(d);

    if (d->parameters == parameters)
    {
        return;
    }

    if (d->backend && d->backend->isOpen())
    {
        d->backend->close();
    }

    // Kill the old database error handler
    if (d->backend)
    {
        d->backend->setDatabaseErrorHandler(0);
    }

    d->parameters = parameters;

    if (!d->backend || !d->backend->isCompatible(parameters))
    {
        delete d->db;
        delete d->backend;
        d->backend = new DatabaseCoreBackend(QString::fromLatin1("database-"), &d->lock);
        d->db      = new TrainingDB(d->backend);
    }
}

bool DatabaseFaceAccess::checkReadyForUse(DatabaseFaceAccessData* const d, DatabaseFaceInitObserver* const observer)
{
    QStringList drivers = QSqlDatabase::drivers();

    if (!drivers.contains(QString::fromLatin1("QSQLITE")))
    {
        qCWarning(LIBKFACE_LOG) << "No SQLite3 driver available. "
                                   "Libkface depends on the drivers provided by the SQL module of Qt. "
                                   "List of QSqlDatabase drivers: " << drivers;
        d->lastError = SQLite3DriverUnavialable;
        return false;
    }

    // create an object with private shortcut constructor
    DatabaseFaceAccess access(false, d);

    if (!d->backend)
    {
        qCWarning(LIBKFACE_LOG) << "No database backend available in checkReadyForUse. "
                                   "Did you call setParameters before?";
        return false;
    }

    if (d->backend->isReady())
    {
        return true;
    }

    if (!d->backend->isOpen())
    {
        if (!d->backend->open(d->parameters))
        {
            access.setLastError((int)d->backend->lastError().type());
            return false;
        }
    }

    // avoid endless loops (if called methods create new DatabaseFaceAccess objects)
    d->initializing = true;

    // update schema
    DatabaseFaceSchemaUpdater updater(&access);
    updater.setObserver(observer);

    if (!d->backend->initSchema(&updater))
    {
        d->initializing = false;
        return false;
    }

    d->initializing = false;

    return d->backend->isReady();
}

int DatabaseFaceAccess::lastError() const
{
    return d->lastError;
}

void DatabaseFaceAccess::setLastError(int error)
{
    d->lastError = error;
}

// ---------------------------------------------------------------------------------

DatabaseFaceAccessUnlock::DatabaseFaceAccessUnlock(DatabaseFaceAccessData* const d)
    : d(d)
{
    // acquire lock
    d->lock.mutex.lock();
    // store lock count
    count = d->lock.lockCount;
    // set lock count to 0
    d->lock.lockCount = 0;

    // unlock
    for (int i=0; i<count; ++i)
    {
        d->lock.mutex.unlock();
    }

    // drop lock acquired in first line. Mutex is now free.
    d->lock.mutex.unlock();
}

DatabaseFaceAccessUnlock::DatabaseFaceAccessUnlock(DatabaseFaceAccess* const access)
    : d(access->d)
{
    // With the passed pointer, we have assured that the mutex is acquired
    // Store lock count
    count = d->lock.lockCount;
    // set lock count to 0
    d->lock.lockCount = 0;

    // unlock
    for (int i=0; i<count; ++i)
    {
        d->lock.mutex.unlock();
    }

    // Mutex is now free
}

DatabaseFaceAccessUnlock::~DatabaseFaceAccessUnlock()
{
    // lock as often as it was locked before
    for (int i=0; i<count; ++i)
    {
        d->lock.mutex.lock();
    }

    // update lock count
    d->lock.lockCount += count;
}

} // namespace KFaceIface
