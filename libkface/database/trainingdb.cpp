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

#include "trainingdb.h"

// Qt includes

#include <QDebug>

// Local includes

#include "databasecorebackend.h"

namespace KFaceIface
{


class TrainingDB::TrainingDBPriv
{
public:
    TrainingDBPriv()
        : db(0)
    {
    }

    DatabaseCoreBackend* db;
};

TrainingDB::TrainingDB(DatabaseCoreBackend* db)
    : d(new TrainingDBPriv)
{
    d->db = db;
}

TrainingDB::~TrainingDB()
{
    delete d;
}

void TrainingDB::setSetting(const QString& keyword, const QString& value)
{
    d->db->execSql( QString("REPLACE into Settings VALUES (?,?);"),
                    keyword, value );
}

QString TrainingDB::setting(const QString& keyword)
{
    QList<QVariant> values;
    d->db->execSql( QString("SELECT value FROM Settings "
                            "WHERE keyword=?;"),
                    keyword, &values );

    if (values.isEmpty())
    {
        return QString();
    }
    else
    {
        return values.first().toString();
    }
}

int TrainingDB::addIdentity()
{
    QVariant id;
    d->db->execSql("INSERT INTO Identities (type) VALUES (0)", 0, &id);
    return id.toInt();
}

void TrainingDB::updateIdentity(const Identity& p)
{
    d->db->execSql("DELETE FROM IdentityAttributes WHERE id=?", p.id);
    QMap<QString, QString>::const_iterator it;
    for (it=p.attributes.begin(); it != p.attributes.end(); ++it)
    {
        d->db->execSql("INSERT INTO IdentityAttributes (attribute, value) VALUES (?,?)", it.key(), it.value());
    }
}

void TrainingDB::deleteIdentity(int id)
{
    // Triggers do the rest
    d->db->execSql("DELETE FROM Identities WHERE id=?", id);
}

QList<Identity> TrainingDB::identities()
{
    QList<QVariant> ids;
    QList<Identity> results;
    d->db->execSql("SELECT id FROM Identities", &ids);

    foreach (const QVariant& v, ids)
    {
        QList<QVariant> values;
        Identity id;
        id.id = v.toInt();
        d->db->execSql("SELECT attribute, value FROM IdentityAttributes WHERE id=?", id.id, &values);
        for (QList<QVariant>::const_iterator it = values.constBegin(); it != values.constEnd();)
        {
            QString attribute = it->toString();
            ++it;
            QString value     = it->toString();
            ++it;

            id.attributes[attribute] = value;
        }
        results << id;
    }
    return results;
}

QList<int> TrainingDB::identityIds()
{
    QList<QVariant> ids;
    d->db->execSql("SELECT id FROM Identities", &ids);

    QList<int> results;
    foreach (const QVariant& var, ids)
    {
        results << var.toInt();
    }
    return results;
}

void TrainingDB::addTLDFaceModel(int identity, const UnitFaceModel& model)
{
    QVariantList boundValues;
    boundValues << identity
                << model.objWidth << model.objHeight << model.minVar
                << model.serialisedPositivePatches
                << model.serialisedNegativePatches
                << model.serialisedFeatures
                << model.serialisedLeaves;
    d->db->execSql("INSERT INTO OpenTLDData "
                   "(identity, width, height, minVar, positivePatches, negativePatches, allFeatures, allLeaves) "
                   "VALUES (?, ?, ?, ?, ?, ?, ?, ?)", boundValues);
}

QList<UnitFaceModel> TrainingDB::tldFaceModels(int identity)
{
    QList<UnitFaceModel> results;
    QList<QVariant> values;

    d->db->execSql("SELECT width, height, minVar, positivePatches, negativePatches, allFeatures, allLeaves "
                  " FROM OpenTLDData WHERE identity=?", identity, &values);

    for (QList<QVariant>::const_iterator it = values.constBegin(); it != values.constEnd();)
    {
        UnitFaceModel model;
        model.objWidth = it->toInt();
        ++it;
        model.objHeight = it->toInt();
        ++it;
        model.minVar = it->toDouble();
        ++it;

        model.serialisedPositivePatches = it->toByteArray();
        ++it;
        model.serialisedNegativePatches = it->toByteArray();
        ++it;
        model.serialisedFeatures        = it->toByteArray();
        ++it;
        model.serialisedLeaves          = it->toByteArray();
        ++it;

        results << model;
    }
    return results;
}


/*
int TrainingDB::addDisease(int patientId, const Disease& dis)
{
    QVariant id;
    d->db->execSql("INSERT INTO Diseases (patientId, initialDiagnosis, cTNM, pTNM) "
                   "VALUES (?, ?, ?, ?)",
                   patientId, dis.initialDiagnosis.toString(Qt::ISODate),
                   dis.initialTNM.cTNM(), dis.initialTNM.pTNM(), 0, &id);
    return id.toInt();
}

void TrainingDB::updateDisease(const Disease& dis)
{
    d->db->execSql("UPDATE Diseases SET initialDiagnosis=?, cTNM=?, pTNM=? WHERE id=?;",
                   dis.initialDiagnosis.toString(Qt::ISODate),
                   dis.initialTNM.cTNM(), dis.initialTNM.pTNM(), dis.id);
}

QList<Disease> TrainingDB::findDiseases(int patientId)
{
    QList<QVariant> values;

    d->db->execSql("SELECT id, initialDiagnosis, cTNM, pTNM FROM Diseases WHERE patientId = ?;",
                   patientId, &values);

    QList<Disease> diseases;
    for (QList<QVariant>::const_iterator it = values.constBegin(); it != values.constEnd();)
    {
        Disease d;

        d.id        = it->toInt();
        ++it;
        d.initialDiagnosis = QDate::fromString(it->toString(), Qt::ISODate);
        ++it;
        d.initialTNM.addTNM(it->toString()); // cTNM string
        ++it;
        d.initialTNM.addTNM(it->toString()); // pTNM string
        ++it;

        diseases << d;
    }

    return diseases;
}

int TrainingDB::addPathology(int diseaseId, const Pathology& path)
{
    QVariant id;
    d->db->execSql("INSERT INTO Pathologies (diseaseId, entity, sampleOrigin, context, date) "
                   "VALUES (?, ?, ?, ?, ?)",
                   QVariantList() << diseaseId << path.entity << path.sampleOrigin << path.context
                   << path.date.toString(Qt::ISODate), 0, &id);
    return id.toInt();
}

void TrainingDB::updatePathology(const Pathology& path)
{
    d->db->execSql("UPDATE Pathologies SET entity=?, sampleOrigin=?, context=?, date=? WHERE id=?;",
                   QVariantList() << path.entity << path.sampleOrigin
                   << path.context << path.date.toString(Qt::ISODate) << path.id );
}

QList<Pathology> TrainingDB::findPathologies(int diseaseId)
{
    QList<QVariant> values;

    d->db->execSql("SELECT id, entity, sampleOrigin, context, date FROM Pathologies WHERE diseaseId = ?;",
                   diseaseId, &values);

    QList<Pathology> pathologies;
    for (QList<QVariant>::const_iterator it = values.constBegin(); it != values.constEnd();)
    {
        Pathology p;

        p.id           = it->toInt();
        ++it;
        p.entity       = (Pathology::Entity)it->toInt();
        ++it;
        p.sampleOrigin = (Pathology::SampleOrigin)it->toInt();
        ++it;
        p.context      = it->toString();
        ++it;
        p.date         = QDate::fromString(it->toString(), Qt::ISODate);
        ++it;

        pathologies << p;
    }

    return pathologies;
}

QList<Property> TrainingDB::properties(PropertyType e, int id)
{
    QList<QVariant> values;

    d->db->execSql( "SELECT property, value, detail FROM " + d->tableName(e) +
                    " WHERE " + d->idName(e) + "=?;",
                    id, &values );

    QList<Property> properties;

    if (values.isEmpty())
    {
        return properties;
    }

    for (QList<QVariant>::const_iterator it = values.constBegin(); it != values.constEnd();)
    {
        Property property;

        property.property = (*it).toString();
        ++it;
        property.value    = (*it).toString();
        ++it;
        property.detail   = (*it).toString();
        ++it;

        properties << property;
    }

    return properties;
}

void TrainingDB::addProperty(PropertyType e, int id, const QString& property,
                            const QString& value, const QString& detail)
{
    d->db->execSql("INSERT INTO " + d->tableName(e) +
                   " (" + d->idName(e) + ", property, value, detail) VALUES(?, ?, ?, ?);",
                   id, property, value, detail);
}

void TrainingDB::removeProperties(PropertyType e, int id, const QString& property, const QString& value)
{
    if (property.isNull())
    {
        d->db->execSql("DELETE FROM " + d->tableName(e) + " WHERE " +
                       d->idName(e) + "=?;",
                       id);
    }
    else if (value.isNull())
    {
        d->db->execSql("DELETE FROM " + d->tableName(e) + " WHERE " +
                       d->idName(e) + "=? AND property=?;",
                       id, property);
    }
    else
    {
        d->db->execSql("DELETE FROM " + d->tableName(e) + " WHERE " +
                       d->idName(e) + "=? AND property=? AND value=?;",
                       id, property, value);
    }
}
*/

} // namespace
