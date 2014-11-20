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

#include "databaseparameters.h"
#include "databaseerrorhandler.h"

class QMutexLocker;

namespace KFaceIface
{

class DatabaseCoreBackend;
class InitializationObserver;
class TrainingDB;
class DatabaseAccessData;

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

    TrainingDB* db() const;

    DatabaseCoreBackend* backend() const;
    QString lastError() const;
    DatabaseParameters parameters() const;


    static void initDatabaseErrorHandler(DatabaseAccessData* const d, DatabaseErrorHandler* const errorhandler);
    static void setParameters(DatabaseAccessData* const d, const DatabaseParameters& parameters);
    static bool checkReadyForUse(DatabaseAccessData* const d, InitializationObserver* const observer = 0);

    /**
      * Set the "last error" message. This method is not for public use.
      */
    void setLastError(const QString& error);

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
