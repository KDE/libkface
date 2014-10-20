/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
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

// Qt includes

#include <QDir>
#include <QDomDocument>
#include <QDomElement>
#include <QDomNode>
#include <QDomNodeList>
#include <QFile>
#include <QIODevice>
#include <QTextStream>

// KDE includes

#include <kstandarddirs.h>
#include <kdebug.h>
#include <klocale.h>

// Local includes

#include "databaseconfigelement.h"
//#include "dbconfigversion.h"

namespace KFaceIface
{

namespace
{
    int dbconfig_xml_version = 1;
}

// ------------------------------------------------------------------

class DatabaseConfigElementLoader
{
public:

    DatabaseConfigElementLoader();

    bool                  readConfig();
    DatabaseConfigElement readDatabase(QDomElement& databaseElement);
    void                  readDBActions(QDomElement& sqlStatementElements, DatabaseConfigElement& configElement);

public:

    bool                                 isValid;
    QString                              errorMessage;
    QMap<QString, DatabaseConfigElement> databaseConfigs;
};

Q_GLOBAL_STATIC(DatabaseConfigElementLoader, loader)

DatabaseConfigElementLoader::DatabaseConfigElementLoader()
{
    isValid = readConfig();

    if (!isValid)
    {
        kWarning() << errorMessage;
    }
}

DatabaseConfigElement DatabaseConfigElementLoader::readDatabase(QDomElement& databaseElement)
{
    DatabaseConfigElement configElement;
    configElement.databaseID = "Unidentified";

    if (!databaseElement.hasAttribute("name"))
    {
        kDebug() << "Missing statement attribute <name>.";
    }

    configElement.databaseID = databaseElement.attribute("name");
    QDomElement element      = databaseElement.namedItem("databaseName").toElement();

    if (element.isNull())
    {
        kDebug() << "Missing element <databaseName>.";
    }

    configElement.databaseName = element.text();
    element                    = databaseElement.namedItem("userName").toElement();

    if (element.isNull())
    {
        kDebug() << "Missing element <userName>.";
    }

    configElement.userName = element.text();
    element                = databaseElement.namedItem("password").toElement();

    if (element.isNull())
    {
        kDebug() << "Missing element <password>.";
    }

    configElement.password = element.text();
    element                = databaseElement.namedItem("hostName").toElement();

    if (element.isNull())
    {
        kDebug() << "Missing element <hostName>.";
    }

    configElement.hostName = element.text();
    element                = databaseElement.namedItem("port").toElement();

    if (element.isNull())
    {
        kDebug() << "Missing element <port>.";
    }

    configElement.port = element.text();
    element            = databaseElement.namedItem("connectoptions").toElement();

    if (element.isNull())
    {
        kDebug() << "Missing element <connectoptions>.";
    }

    configElement.connectOptions = element.text();
    element                      = databaseElement.namedItem("dbservercmd").toElement();

    if (element.isNull())
    {
        kDebug() << "Missing element <dbservercmd>.";
    }

    configElement.dbServerCmd = element.text();
    element                   = databaseElement.namedItem("dbinitcmd").toElement();

    if (element.isNull())
    {
        kDebug() << "Missing element <dbinitcmd>.";
    }

    configElement.dbInitCmd = element.text();
    element                 = databaseElement.namedItem("dbactions").toElement();

    if (element.isNull())
    {
        kDebug() << "Missing element <dbactions>.";
    }

    readDBActions(element, configElement);

    return configElement;
}

void DatabaseConfigElementLoader::readDBActions(QDomElement& sqlStatementElements, DatabaseConfigElement& configElement)
{
    QDomElement dbActionElement = sqlStatementElements.firstChildElement("dbaction");

    for ( ; !dbActionElement.isNull();  dbActionElement=dbActionElement.nextSiblingElement("dbaction"))
    {
        if (!dbActionElement.hasAttribute("name"))
        {
            kDebug() << "Missing statement attribute <name>.";
        }

        DatabaseAction action;
        action.name = dbActionElement.attribute("name");
        //kDebug() << "Getting attribute " << dbActionElement.attribute("name");

        if (dbActionElement.hasAttribute("mode"))
        {
            action.mode = dbActionElement.attribute("mode");
        }

        QDomElement databaseElement = dbActionElement.firstChildElement("statement");

        for ( ; !databaseElement.isNull();  databaseElement=databaseElement.nextSiblingElement("statement"))
        {
            if (!databaseElement.hasAttribute("mode"))
            {
                kDebug() << "Missing statement attribute <mode>.";
            }

            DatabaseActionElement actionElement;
            actionElement.mode      = databaseElement.attribute("mode");
            actionElement.statement = databaseElement.text();

            action.dbActionElements.append(actionElement);
        }

        configElement.sqlStatements.insert(action.name, action);
    }
}

bool DatabaseConfigElementLoader::readConfig()
{
    QString filepath = KStandardDirs::locate("data", "libkface/database/dbconfig.xml");
    //QString filepath = ":/database/dbconfig.xml";

    QFile file(filepath);

    if (!file.exists())
    {
        errorMessage = i18n("Could not open the dbconfig.xml file. "
                            "This file is installed with libkface "
                            "and is absolutely required to run recognition. "
                            "Please check your installation.");
        return false;
    }

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        errorMessage = i18n("Could not open dbconfig.xml file <filename>%1</filename>", filepath);
        return false;
    }

    QDomDocument doc("DBConfig");

    if (!doc.setContent(&file))
    {
        file.close();
        errorMessage = i18n("The XML in the dbconfig.xml file <filename>%1</filename> "
                            "is invalid and cannot be read.", filepath);
        return false;
    }

    file.close();

    QDomElement element = doc.namedItem("databaseconfig").toElement();

    if (element.isNull())
    {
        errorMessage = i18n("The XML in the dbconfig.xml file <filename>%1</filename> "
                            "is missing the required element <icode>%2</icode>", filepath, element.tagName());
        return false;
    }

    QDomElement versionElement = element.namedItem("version").toElement();
    int version = 0;

    kDebug() << versionElement.isNull() << versionElement.text() << versionElement.text().toInt() << dbconfig_xml_version;

    if (!versionElement.isNull())
    {
        version = versionElement.text().toInt();
    }

    if (version < dbconfig_xml_version)
    {
        errorMessage = i18n("An old version of the dbconfig.xml file <filename>%1</filename> "
                            "is found. Please ensure that the right version of libkface "
                            "is installed.", filepath);
        return false;
    }

    QDomElement databaseElement = element.firstChildElement("database");

    for ( ; !databaseElement.isNull(); databaseElement=databaseElement.nextSiblingElement("database"))
    {
        DatabaseConfigElement l_DBCfgElement = readDatabase(databaseElement);
        databaseConfigs.insert(l_DBCfgElement.databaseID, l_DBCfgElement);
    }

    return true;
}

// ------------------------------------------------------------------

DatabaseConfigElement DatabaseConfigElement::element(const QString& databaseType)
{
    // Unprotected read-only access? Usually accessed under DatabaseAccess protection anyway
    return loader()->databaseConfigs.value(databaseType);
}

bool DatabaseConfigElement::checkReadyForUse()
{
    return loader()->isValid;
}

QString DatabaseConfigElement::errorMessage()
{
    return loader()->errorMessage;
}

} // namespace KFaceIface
