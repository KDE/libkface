/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2007-03-18
 * Description : Storage container for database connection parameters.
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

#ifndef DATABASEPARAMETERS_H
#define DATABASEPARAMETERS_H

// Qt includes

#include <QString>
#include <QtGlobal>
#include <QUrl>

// Local includes

#include "databaseconfigelement.h"

namespace KFaceIface
{

class DatabaseParameters
{

public:

    /**
     * This class encapsulates all parameters needed to establish
     * a connection to a database (inspired by the API of QT SQL of Qt4).
     * The values can be read from and written to a KUrl.
     */
    DatabaseParameters(const QString& type, const QString& databaseFilePath);
    DatabaseParameters();

    QString databaseType;
    QString databaseName;
    QString connectOptions;

    bool operator==(const DatabaseParameters& other) const;
    bool operator!=(const DatabaseParameters& other) const;

    /**
     * Performs basic checks that the parameters are not empty and have the information
     * required for the databaseType.
     */
    bool isValid() const;

    bool    isSQLite() const;
    bool    isMySQL() const;
    QString SQLiteDatabaseFile() const;

    /**
     * Returns the databaseType designating the said database.
     * If you have a DatabaseParameters object already, you can use isSQLite() as well.
     * These strings are identical to the driver identifiers in the Qt SQL module.
     */
    static QString SQLiteDatabaseType();
    static QString MySQLDatabaseType();

    /**
     * Return a set of default parameters for the give type
     */
    static DatabaseParameters defaultParameters(const QString databaseType);

    //static DatabaseParameters parametersFromConfig(KSharedConfig::Ptr config = KGlobal::config(), const QString& configGroup = QString());

    /**
     * Read and write parameters from config. You can specify the group,
     * or use the default value.
     */
    //void readFromConfig(KSharedConfig::Ptr config = KGlobal::config(), const QString& configGroup = QString());
    //void writeToConfig(KSharedConfig::Ptr config = KGlobal::config(), const QString& configGroup = QString()) const;

    /**
     * Convenience method to create a DatabaseParameters object for an
     * SQLITE 3 database specified by the local file path.
     */
    static DatabaseParameters parametersForSQLite(const QString& databaseFile);
};

} // namespace KFaceIface

#endif // DATABASEPARAMETERS_H
