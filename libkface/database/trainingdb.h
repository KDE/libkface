/* ============================================================
 *
 * This file is a part of Tumorprofil
 *
 * Date        : 02.02.2012
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

#ifndef PATIENTDB_H
#define PATIENTDB_H

// Qt includes

#include <QString>
#include <QVariant>

// Local includes

#include "identity.h"
#include "unitfacemodel.h"

namespace KFaceIface
{

class DatabaseCoreBackend;

class TrainingDB
{
public:

    TrainingDB(DatabaseCoreBackend* db);
    ~TrainingDB();

    void setSetting(const QString& keyword, const QString& value);
    QString setting(const QString& keyword);

    int addIdentity();
    void updateIdentity(const Identity& p);
    void deleteIdentity(int id);
    QList<Identity> identities();
    QList<int> identityIds();

    /// OpenTLD

    void addTLDFaceModel(int identity, const UnitFaceModel& model);
    QList<UnitFaceModel> tldFaceModels(int identity);
/*
    int addDisease(int patientId, const Disease& d);
    void updateDisease(const Disease& dis);
    QList<Disease> findDiseases(int patientId);

    int addPathology(int diseaseId, const Pathology& path);
    void updatePathology(const Pathology& path);
    QList<Pathology> findPathologies(int diseaseId);

    enum PropertyType
    {
        IdentityProperties,
        PathologyProperties
    };

    QList<Property> properties(PropertyType e, int id);
    void addProperty(PropertyType e, int id, const QString& property,
                     const QString& value, const QString& detail);
    void removeProperties(PropertyType e, int id,
                          const QString& property = QString(),
                          const QString& value = QString());*/

private:


    class TrainingDBPriv;
    TrainingDBPriv* const d;
};


} // namespace

#endif // PATIENTDB_H
