/* ============================================================
 *
 * This file is a part of KDE project
 *
 * Date        : 2009-05-29
 * Description : Face database access wrapper.
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

#ifndef DATABASE_FACE_ACCESS_H
#define DATABASE_FACE_ACCESS_H

// Local includes

#include "databasefaceparameters.h"
#include "databaseerrorhandler.h"

namespace KFaceIface
{

class DatabaseCoreBackend;
class DatabaseFaceInitObserver;
class TrainingDB;
class DatabaseFaceAccessData;

class DatabaseFaceAccess
{
public:

    static DatabaseFaceAccessData* create();
    static void destroy(DatabaseFaceAccessData* const);

    /** This class is written in analogy to DatabaseFaceAccess
     *  (some features stripped off).
     *  For documentation, see databaseaccess.h */

    DatabaseFaceAccess(DatabaseFaceAccessData* const);
    ~DatabaseFaceAccess();

    TrainingDB*          db()         const;
    DatabaseCoreBackend* backend()    const;
    int                  lastError()  const;
    DatabaseFaceParameters   parameters() const;


    static void initDatabaseErrorHandler(DatabaseFaceAccessData* const d, DatabaseErrorHandler* const errorhandler);
    static void setParameters(DatabaseFaceAccessData* const d, const DatabaseFaceParameters& parameters);
    static bool checkReadyForUse(DatabaseFaceAccessData* const d, DatabaseFaceInitObserver* const observer = 0);

    /**
      * Set the "last error" code. This method is not for public use.
      */
    void setLastError(int error);

private:

    DatabaseFaceAccess(bool, DatabaseFaceAccessData* const);

private:

    friend class DatabaseFaceAccessUnlock;
    DatabaseFaceAccessData* const d;
};

// ------------------------------------------------------------------------------------------

class DatabaseFaceAccessUnlock
{
public:

    /** Acquire an object of this class if you want to assure
     *  that the DatabaseFaceAccess is _not_ held during the lifetime of the object.
     *  At creation, the lock is obtained shortly, then all locks are released.
     *  At destruction, all locks are acquired again.
     *  If you need to access any locked structures during lifetime, acquire a new
     *  DatabaseFaceAccess.
     */
    DatabaseFaceAccessUnlock(DatabaseFaceAccessData* const);
    DatabaseFaceAccessUnlock(DatabaseFaceAccess* const access);
    ~DatabaseFaceAccessUnlock();

private:

    DatabaseFaceAccessData* d;
    int                     count;
};

} // namespace KFaceIface

#endif // DATABASE_FACE_ACCESS_H
