/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2009-09-27
 * Description : gui database error handler
 *
 * Copyright (C) 2009-2010 by Holger Foerster <Hamsi2k at freenet dot de>
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

#ifndef SQLQUERY_H
#define SQLQUERY_H

// Qt includes

#include <QtCore/QMetaType>
#include <QtCore/QString>
#include <QtSql/QSqlQuery>

// Local includes

namespace KFaceIface
{

class SqlQuery : public QSqlQuery
{

public:

    SqlQuery(const QSqlQuery& other);
    SqlQuery(const QSqlDatabase& db);
    virtual ~SqlQuery();

    virtual SqlQuery& operator=(const SqlQuery& other);
    virtual bool prepare(const QString& query);
    virtual QString lastQuery() const;

private:

    QString m_query;
};

} // namespace KFaceIface

#endif /* SQLQUERY_H */
