/* ============================================================
 *
 * This file is a part of KDE project
 *
 * Date        : 2012-02-02
 * Description : Face database initialization observer
 *
 * Copyright (C) 2012 by Marcel Wiesweg <marcel dot wiesweg at uk-essen dot de>
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

#ifndef DATABASE_FACE_INIT_IOBSERVER_H
#define DATABASE_FACE_INIT_IOBSERVER_H

// Qt includes

#include <QString>

namespace KFaceIface
{

class DatabaseFaceInitObserver
{
public:

    enum UpdateResult
    {
        UpdateSuccess,
        UpdateError,
        UpdateErrorMustAbort
    };

public:

    virtual ~DatabaseFaceInitObserver() {};

    virtual bool continueQuery() = 0;

    virtual void moreSchemaUpdateSteps(int numberOfSteps) = 0;
    virtual void schemaUpdateProgress(const QString& message, int numberOfSteps = 1) = 0;
    virtual void finishedSchemaUpdate(UpdateResult result) = 0;

    virtual void error(int error) = 0;
};

} // namespace KFaceIface

#endif // DATABASE_FACE_INIT_IOBSERVER_H
