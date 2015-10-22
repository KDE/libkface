/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2009-05-29
 * Description : Thumbnail database access wrapper.
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

#ifndef DATABASEACCESS_H
#define DATABASEACCESS_H

// Local includes

#include "databasefaceparameters.h"
#include "databaseerrorhandler.h"

namespace KFaceIface
{

class DatabaseCoreBackend;
class DatabaseFaceInitObserver;
class TrainingDB;
class DatabaseAccessData;

/** Database error codes in extension of QSqlError::Type
 */
enum DatabaseErrorCode
{
    DatabaseConfigFileInvalid    = 995,  /// Database XML config is invalid.
    CannotOpenDatabaseConfigFile = 996,  /// Database XML config cannot be open.
    DatabaseVersionInvalid       = 997,  /// Database version ID is incompatible with the current schema.
    DatabaseVersionUnknow        = 998,  /// No Database version ID found. Cannot verify schema.
    SQLite3DriverUnavialable     = 999   /// The Qt driver for SQLite3 databases is not available.
};

class DatabaseAccess
{
public:

    static DatabaseAccessData* create();
    static void destroy(DatabaseAccessData* const);

    /** This class is written in analogy to DatabaseAccess
     *  (some features stripped off).
     *  For documentation, see databaseaccess.h */

    DatabaseAccess(DatabaseAccessData* const);
    ~DatabaseAccess();

    TrainingDB*          db()         const;
    DatabaseCoreBackend* backend()    const;
    int                  lastError()  const;
    DatabaseFaceParameters   parameters() const;


    static void initDatabaseErrorHandler(DatabaseAccessData* const d, DatabaseErrorHandler* const errorhandler);
    static void setParameters(DatabaseAccessData* const d, const DatabaseFaceParameters& parameters);
    static bool checkReadyForUse(DatabaseAccessData* const d, DatabaseFaceInitObserver* const observer = 0);

    /**
      * Set the "last error" code. This method is not for public use.
      */
    void setLastError(int error);

private:

    DatabaseAccess(bool, DatabaseAccessData* const);

private:

    friend class DatabaseAccessUnlock;
    DatabaseAccessData* const d;
};

// ------------------------------------------------------------------------------------------

class DatabaseAccessUnlock
{
public:

    /** Acquire an object of this class if you want to assure
     *  that the DatabaseAccess is _not_ held during the lifetime of the object.
     *  At creation, the lock is obtained shortly, then all locks are released.
     *  At destruction, all locks are acquired again.
     *  If you need to access any locked structures during lifetime, acquire a new
     *  DatabaseAccess.
     */
    DatabaseAccessUnlock(DatabaseAccessData* const);
    DatabaseAccessUnlock(DatabaseAccess* const access);
    ~DatabaseAccessUnlock();

private:

    DatabaseAccessData* d;
    int                 count;
};

} // namespace KFaceIface

#endif // DATABASEACCESS_H
