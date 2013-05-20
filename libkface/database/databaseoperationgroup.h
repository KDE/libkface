/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2010-10-12
 * Description : Convenience object for grouping database operations
 *
 * Copyright (C) 2010 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
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

#ifndef DATABASEOPERATIONGROUP_H
#define DATABASEOPERATIONGROUP_H

// Qt includes

#include <QVariant>

namespace KFaceIface
{

class DatabaseAccess;
class DatabaseAccessData;

class DatabaseOperationGroup
{
public:

    /**
     * When you intend to execute a number of write operations to the database,
     * group them while holding a DatabaseOperationGroup.
     * For some database systems (SQLite), keeping a transaction across write operations
     * occurring in short time results in enormous speedup (800x).
     * For system that do not need this optimization, this class is a no-op.
     */

    /**
     * Retrieve a DatabaseAccess object each time when constructing and destructing.
     */
    DatabaseOperationGroup(DatabaseAccessData* db);
    /**
     * Use an existing DatabaseAccess object, which must live as long as this object exists.
     */
    DatabaseOperationGroup(DatabaseAccess* access);
    ~DatabaseOperationGroup();

    /**
     * This will - if a transaction is held - commit the transaction and acquire a new one.
     * This may improve concurrent access.
     */
    void lift();

    void setMaximumTime(int msecs);

    /** Resets to 0 the time used by allowLift() */
    void resetTime();
    /**
     * Allows to lift(). The transaction will be lifted if the time set by setMaximumTime()
     * has expired.
     */
    void allowLift();

private:

    class DatabaseOperationGroupPriv;
    DatabaseOperationGroupPriv* const d;
};

} // namespace



#endif // DATABASETRANSACTION_H
