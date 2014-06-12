/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2012-02-02
 * Description : Database initialization observer
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

#ifndef DATABASEINITIALIZATIONOBSERVER_H
#define DATABASEINITIALIZATIONOBSERVER_H

// Qt includes

#include <QString>

namespace KFaceIface
{

class InitializationObserver
{
public:

    enum UpdateResult
    {
        UpdateSuccess,
        UpdateError,
        UpdateErrorMustAbort
    };

public:

    virtual ~InitializationObserver() {};

    virtual bool continueQuery() = 0;

    virtual void moreSchemaUpdateSteps(int numberOfSteps) = 0;
    virtual void schemaUpdateProgress(const QString& message, int numberOfSteps = 1) = 0;
    virtual void finishedSchemaUpdate(UpdateResult result) = 0;

    virtual void error(const QString& errorMessage) = 0;
};

} // namespace KFaceIface

#endif // DATABASEINITIALIZATIONOBSERVER_H
