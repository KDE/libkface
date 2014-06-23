/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2007-03-18
 * Description : Storage container for database connection parameters.
 *
 * Copyright (C) 2007-2008 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2010      by Holger Foerster <hamsi2k at freenet dot de>
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

#include "databaseparameters.h"

// Qt includes

#include <QDir>
#include <QFile>

namespace KFaceIface
{

namespace
{
/*
static const char* configGroupDatabase = "Database Settings";
static const char* configDatabaseType  = "Database Type";
static const char* configDatabaseName  = "Database Name";
*/
}

DatabaseParameters::DatabaseParameters()
{
}

DatabaseParameters::DatabaseParameters(const QString& type, const QString& databaseName)
    : databaseType(type), databaseName(databaseName)
{
}

bool DatabaseParameters::operator==(const DatabaseParameters& other) const
{
    return (databaseType == other.databaseType &&
            databaseName == other.databaseName);
}

bool DatabaseParameters::operator!=(const DatabaseParameters& other) const
{
    return !operator==(other);
}

bool DatabaseParameters::isValid() const
{
    if (isSQLite())
    {
        return !databaseName.isEmpty();
    }

    return false;
}

bool DatabaseParameters::isSQLite() const
{
    return databaseType == "QSQLITE";
}

bool DatabaseParameters::isMySQL() const
{
    return databaseType == "QMYSQL";
}

QString DatabaseParameters::SQLiteDatabaseType()
{
    return "QSQLITE";
}

QString DatabaseParameters::MySQLDatabaseType()
{
    return "QMYSQL";
}

QString DatabaseParameters::SQLiteDatabaseFile() const
{
    if (isSQLite())
    {
        return databaseName;
    }

    return QString();
}

/*
DatabaseParameters DatabaseParameters::parametersFromConfig(KSharedConfig::Ptr config, const QString& configGroup)
{
    DatabaseParameters parameters;
    parameters.readFromConfig(config, configGroup);
    return parameters;
}

void DatabaseParameters::readFromConfig(KSharedConfig::Ptr config, const QString& configGroup)
{
    KConfigGroup group;

    if (configGroup.isNull())
    {
        group = config->group(configGroupDatabase);
    }
    else
    {
        group = config->group(configGroup);
    }

    databaseType             = group.readEntry(configDatabaseType, QString());
    databaseName             = group.readEntry(configDatabaseName, QString());
}

void DatabaseParameters::writeToConfig(KSharedConfig::Ptr config, const QString& configGroup) const
{
    KConfigGroup group;

    if (configGroup.isNull())
    {
        group = config->group(configGroupDatabase);
    }
    else
    {
        group = config->group(configGroup);
    }

    group.writeEntry(configDatabaseType, databaseType);
    group.writeEntry(configDatabaseName, databaseName);

*/

DatabaseParameters DatabaseParameters::parametersForSQLite(const QString& databaseFile)
{
    // only the database name is needed
    return DatabaseParameters("QSQLITE", databaseFile);
}

} // namespace KFaceIface
