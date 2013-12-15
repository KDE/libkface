/* ============================================================
 *
 * This file is a part of digikam/libkface
 *
 * Date        : 02.02.2012
 *
 * Copyright (C) 2012-13 by Marcel Wiesweg <marcel dot wiesweg at uk-essen dot de>
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

#ifndef TRAININGDB_H
#define TRAININGDB_H

// Qt includes

#include <QString>
#include <QVariant>

// Local includes

#include "identity.h"
#include "unitfacemodel.h"

namespace KFaceIface
{

class DatabaseCoreBackend;
class LBPHFaceModel;

class TrainingDB
{
public:

    TrainingDB(DatabaseCoreBackend* const db);
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

    /// OpenCV LBPH

    void updateLBPHFaceModel(LBPHFaceModel& model);
    LBPHFaceModel lbphFaceModel();
    void clearLBPHTraining(const QString& context = QString());
    void clearLBPHTraining(const QList<int>& identities, const QString& context = QString());

private:

    class Private;
    Private* const d;
};

} // namespace KFaceIface

#endif // PATIENTDB_H
