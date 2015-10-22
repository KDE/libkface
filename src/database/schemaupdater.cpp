/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2007-04-16
 * Description : Schema update
 *
 * Copyright (C) 2007-2009 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
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

#include "schemaupdater.h"

// Local includes

#include "libkface_debug.h"
#include "databasecorebackend.h"
#include "databaseaccess.h"
#include "databasefaceinitobserver.h"
#include "trainingdb.h"

namespace KFaceIface
{

class SchemaUpdater::Private
{
public:

    Private()
        : setError(false),
          currentVersion(0),
          currentRequiredVersion(0),
          access(0),
          observer(0)
    {
    }

    bool                      setError;

    int                       currentVersion;
    int                       currentRequiredVersion;

    DatabaseAccess*           access;

    DatabaseFaceInitObserver* observer;
};

SchemaUpdater::SchemaUpdater(DatabaseAccess* const access)
    : d(new Private)
{
    d->access = access;
}

SchemaUpdater::~SchemaUpdater()
{
    delete d;
}

int SchemaUpdater::schemaVersion()
{
    return 2;
}

void SchemaUpdater::setObserver(DatabaseFaceInitObserver* const observer)
{
    d->observer = observer;
}

bool SchemaUpdater::update()
{
    bool success = startUpdates();

    // even on failure, try to set current version - it may have incremented
    if (d->currentVersion)
    {
        d->access->db()->setSetting(QString::fromLatin1("DBVersion"), QString::number(d->currentVersion));
    }

    if (d->currentRequiredVersion)
    {
        d->access->db()->setSetting(QString::fromLatin1("DBVersionRequired"), QString::number(d->currentRequiredVersion));
    }

    return success;
}

bool SchemaUpdater::startUpdates()
{
    // First step: do we have an empty database?
    QStringList tables = d->access->backend()->tables();

    if (tables.contains(QString::fromLatin1("Settings"), Qt::CaseInsensitive))
    {
        // Find out schema version of db file
        QString version         = d->access->db()->setting(QString::fromLatin1("DBVersion"));
        QString versionRequired = d->access->db()->setting(QString::fromLatin1("DBVersionRequired"));
        qCDebug(LIBKFACE_LOG) << "Have a database structure version " << version;

        // mini schema update
        if (version.isEmpty() && d->access->parameters().isSQLite())
        {
            version = d->access->db()->setting(QString::fromLatin1("DBVersion"));
        }

        // We absolutely require the DBVersion setting
        if (version.isEmpty())
        {
            // Something is damaged. Give up.
            qCWarning(LIBKFACE_LOG) << "The database is not valid: "
                                       "the \"DBVersion\" setting does not exist. "
                                       "The current database schema version cannot be verified. "
                                       "Try to start with an empty database. ";

            d->access->setLastError(DatabaseVersionUnknow);

            if (d->observer)
            {
                d->observer->error(DatabaseVersionUnknow);
                d->observer->finishedSchemaUpdate(DatabaseFaceInitObserver::UpdateErrorMustAbort);
            }

            return false;
        }

        // current version describes the current state of the schema in the db,
        // schemaVersion is the version required by the program.
        d->currentVersion = version.toInt();

        if (d->currentVersion > schemaVersion())
        {
            // trying to open a database with a more advanced than this SchemaUpdater supports
            if (!versionRequired.isEmpty() && versionRequired.toInt() <= schemaVersion())
            {
                // version required may be less than current version
                return true;
            }
            else
            {
                qCWarning(LIBKFACE_LOG) << "The database has been used with a more recent version of libkface "
                                           "and has been updated to a database schema which cannot be used with this version. "
                                           "(This means this libkface version is too old, or the database format is to recent.) "
                                           "Please use the more recent version of libkface that you used before.";

                d->access->setLastError(DatabaseVersionInvalid);

                if (d->observer)
                {
                    d->observer->error(DatabaseVersionInvalid);
                    d->observer->finishedSchemaUpdate(DatabaseFaceInitObserver::UpdateErrorMustAbort);
                }

                return false;
            }
        }
        else
        {
            return makeUpdates();
        }
    }
    else
    {
        //qCDebug(LIBKFACE_LOG) << "No database file available";
        DatabaseParameters parameters = d->access->parameters();

        // No legacy handling: start with a fresh db
        if (!createDatabase())
        {
            qCDebug(LIBKFACE_LOG) << "Failed to create tables in database: " << d->access->backend()->lastError().text();
                                                
            d->access->setLastError(d->access->backend()->lastError().type());

            if (d->observer)
            {
                d->observer->error(d->access->backend()->lastError().type());
                d->observer->finishedSchemaUpdate(DatabaseFaceInitObserver::UpdateErrorMustAbort);
            }

            return false;
        }

        return true;
    }
}

bool SchemaUpdater::makeUpdates()
{
    if (d->currentVersion < schemaVersion())
    {
        if (d->currentVersion == 1)
        {
            updateV1ToV2();
        }
    }

    return true;
}


bool SchemaUpdater::createDatabase()
{
    if ( createTables() && createIndices() && createTriggers())
    {
        d->currentVersion         = schemaVersion();
        d->currentRequiredVersion = 1;
        return true;
    }
    else
    {
        return false;
    }
}

bool SchemaUpdater::createTables()
{
    return d->access->backend()->execDBAction(d->access->backend()->getDBAction(QString::fromLatin1("CreateDB"))) &&
           d->access->backend()->execDBAction(d->access->backend()->getDBAction(QString::fromLatin1("CreateDBOpenCVLBPH")));
}

bool SchemaUpdater::createIndices()
{
    return d->access->backend()->execDBAction(d->access->backend()->getDBAction(QString::fromLatin1("CreateIndices")));
}

bool SchemaUpdater::createTriggers()
{
    return d->access->backend()->execDBAction(d->access->backend()->getDBAction(QString::fromLatin1("CreateTriggers")));
}

bool SchemaUpdater::updateV1ToV2()
{
/*
    if (!d->access->backend()->execDBAction(d->access->backend()->getDBAction("UpdateDBSchemaFromV1ToV2")))
    {
        qError() << "Schema upgrade in DB from V1 to V2 failed!";
        return false;
    }
*/

    d->currentVersion         = 2;
    d->currentRequiredVersion = 1;
    return true;
}

} // namespace KFaceIface
