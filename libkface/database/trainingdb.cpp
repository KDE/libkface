/* ============================================================
 *
 * This file is a part of digikam/libkface
 *
 * Date        : 02.02.2012
 *
 * Copyright (C) 2012-2013 by Marcel Wiesweg <marcel dot wiesweg at uk-essen dot de>
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

// OpenCV includes need to show up before Qt includes
#include "lbphfacemodel.h"

#include "trainingdb.h"

// KDE includes

#include <kdebug.h>

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
        d->db->execSql("INSERT INTO IdentityAttributes (id, attribute, value) VALUES (?, ?,?)", p.id, it.key(), it.value());
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

namespace
{
    enum
    {
        LBPHStorageVersion = 1
    };
}

void TrainingDB::updateLBPHFaceModel(LBPHFaceModel& model)
{
    QVariantList values;
    values << LBPHStorageVersion << model.radius() << model.neighbors() << model.gridX() << model.gridY();
    if (model.databaseId)
    {
        values << model.databaseId;
        d->db->execSql("UPDATE OpenCVLBPHRecognizer SET version=?, radius=?, neighbors=?, grid_x=?, grid_y=? WHERE id=?", values);
    }
    else
    {
        QVariant insertedId;
        d->db->execSql("INSERT INTO OpenCVLBPHRecognizer (version, radius, neighbors, grid_x, grid_y) VALUES (?,?,?,?,?)",
                       values, 0, &insertedId);
        model.databaseId = insertedId.toInt();
    }

    QList<LBPHistogramMetadata> metadataList = model.histogramMetadata();
    for (int i=0; i<metadataList.size(); i++)
    {
        const LBPHistogramMetadata& metadata = metadataList[i];
        if (metadata.storageStatus == LBPHistogramMetadata::Created)
        {
            OpenCVMatData data = model.histogramData(i);
            QByteArray compressed = qCompress(data.data);
            QVariantList histogramValues;
            QVariant insertedId;
            histogramValues << model.databaseId << metadata.identity << metadata.context
                            << data.type << data.rows << data.cols << compressed;
            d->db->execSql("INSERT INTO OpenCVLBPHistograms (recognizerid, identity, context, type, rows, cols, data) "
                           "VALUES (?,?,?,?,?,?,?)",
                           histogramValues, 0, &insertedId);
            model.setWrittenToDatabase(i, insertedId.toInt());
        }
    }
}

LBPHFaceModel TrainingDB::lbphFaceModel()
{
    QVariantList values;
    //kDebug() << "Loading LBPH model";
    d->db->execSql("SELECT id, version, radius, neighbors, grid_x, grid_y FROM OpenCVLBPHRecognizer", &values);

    for (QList<QVariant>::const_iterator it = values.constBegin(); it != values.constEnd();)
    {
        LBPHFaceModel model;
        model.databaseId = it->toInt();
        ++it;
        //kDebug() << "Found model id" << model.databaseId;

        int version = it->toInt();
        ++it;
        if (version > LBPHStorageVersion)
        {
            kDebug() << "Unsupported LBPH storage version" << version;
            it += 4;
            continue;
        }

        model.setRadius(it->toInt());
        ++it;
        model.setNeighbors(it->toInt());
        ++it;
        model.setGridX(it->toInt());
        ++it;
        model.setGridY(it->toInt());
        ++it;

        SqlQuery query = d->db->execQuery("SELECT id, identity, context, type, rows, cols, data "
                                          "FROM OpenCVLBPHistograms WHERE recognizerid=?",
                                          model.databaseId);
        QList<OpenCVMatData> histograms;
        QList<LBPHistogramMetadata> histogramMetadata;
        while (query.next())
        {
            LBPHistogramMetadata metadata;
            OpenCVMatData data;

            metadata.databaseId    = query.value(0).toInt();
            metadata.identity      = query.value(1).toInt();
            metadata.context       = query.value(2).toString();
            metadata.storageStatus = LBPHistogramMetadata::InDatabase;

            // cv::Mat
            data.type       = query.value(3).toInt();
            data.rows       = query.value(4).toInt();
            data.cols       = query.value(5).toInt();
            data.data       = qUncompress(query.value(6).toByteArray());
            //kDebug() << "Adding histogram" << metadata.databaseId << "identity" << metadata.identity << "size" << data.data.size();

            histograms << data;
            histogramMetadata << metadata;
        }

        model.setHistograms(histograms, histogramMetadata);
        return model;
    }

    return LBPHFaceModel();
}

void TrainingDB::clearLBPHTraining(const QString& context)
{
    if (context.isNull())
    {
        d->db->execSql("DELETE FROM OpenCVLBPHistograms");
        d->db->execSql("DELETE FROM OpenCVLBPHRecognizer");
    }
    else
    {
        d->db->execSql("DELETE FROM OpenCVLBPHistograms WHERE context=?", context);
    }
}

void TrainingDB::clearLBPHTraining(const QList<int>& identities, const QString& context)
{
    foreach (int id, identities)
    {
        if (context.isNull())
        {
            d->db->execSql("DELETE FROM OpenCVLBPHistograms WHERE identity=?", id);
        }
        else
        {
            d->db->execSql("DELETE FROM OpenCVLBPHistograms WHERE identity=? AND context=?", id, context);
        }
    }
}


} // namespace
