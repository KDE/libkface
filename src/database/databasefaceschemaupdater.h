/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2009-05-29
 * Description : Face database schema update
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

#ifndef DATABASE_FACE_SCHEMA_UPDATER_H
#define DATABASE_FACE_SCHEMA_UPDATER_H

namespace KFaceIface
{

class DatabaseFaceAccess;
class DatabaseFaceInitObserver;

class DatabaseFaceSchemaUpdater
{
public:

    DatabaseFaceSchemaUpdater(DatabaseFaceAccess* const access);
    ~DatabaseFaceSchemaUpdater();

    bool update();
    void setObserver(DatabaseFaceInitObserver* const observer);

    static int schemaVersion();

private:

    bool startUpdates();
    bool makeUpdates();
    bool createDatabase();
    bool createTables();
    bool createIndices();
    bool createTriggers();
    bool updateV1ToV2();

private:

    class Private;
    Private* const d;
};

} // namespace KFaceIface

#endif // DATABASE_FACE_SCHEMA_UPDATER_H
