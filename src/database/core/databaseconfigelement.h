/* ============================================================
 *
 * This file is a part of KDE project
 *
 * Date        : 2009-06-27
 * Description : Database element configuration
 *
 * Copyright (C) 2009-2010 by Holger Foerster <hamsi2k at freenet dot de>
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

#ifndef DATABASECONFIGELEMENT_H_
#define DATABASECONFIGELEMENT_H_

// Qt includes

#include <QMap>
#include <QString>

namespace KFaceIface
{

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

class DatabaseActionElement
{
public:

    DatabaseActionElement()
        : order(0)
    {
    }

    QString mode;
    int     order;
    QString statement;
};

// -----------------------------------------------------------------------

class DatabaseAction
{
public:

    QString                      name;
    QString                      mode;
    QList<DatabaseActionElement> dbActionElements;
};

// -----------------------------------------------------------------------

class DatabaseConfigElement
{
public:

    static bool                   checkReadyForUse();
    static int                    error();
    static DatabaseConfigElement  element(const QString& databaseType);

public:

    QString                       databaseID;
    QString                       hostName;
    QString                       port;
    QString                       connectOptions;
    QString                       databaseName;
    QString                       userName;
    QString                       password;
    QString                       dbServerCmd;
    QString                       dbInitCmd;
    QMap<QString, DatabaseAction> sqlStatements;
};

} // namespace KFaceIface

#endif /* DATABASECONFIGELEMENT_H_ */
