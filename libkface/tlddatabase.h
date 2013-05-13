/** ===========================================================
 * @file
 *
 * This file is a part of digiKam project
 * <a href="http://www.digikam.org">http://www.digikam.org</a>
 *
 * @date    2010-03-03
 * @brief   openTLD interface.
 * @section DESCRIPTION
 *
 * Face detection data storage in SQlite database.
 *
 * @author Copyright (C) 2012-2013 by Mahesh Hegde
 *         <a href="mailto:maheshmhegade at gmail dot com">maheshmhegade at gmail dot com</a>
 * @author Copyright (C) 2013 by Gilles Caulier
 *         <a href="mailto:caulier dot gilles at gmail dot com">caulier dot gilles at gmail dot com</a>
 *
 * @section LICENSE
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef TLD_DATABASE_H
#define TLD_DATABASE_H

// C++ includes

#include <iostream>

// Qt includes

//#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlTableModel>
#include <QtCore/QDebug>
#include <QtCore/QObject>
#include <QtGui/QImage>

// KDE includes

#include <kstandarddirs.h>

// Local includes

#include "tldrecognition.h"
#include "libkface_export.h"

using namespace std;

namespace KFaceIface
{

class KFACE_EXPORT Tlddatabase
{
public:

    /**
     * @brief Tlddatabase:Convinience class to Store and retrie facemodel data to/from dedicated database file
     *(faceDatabase.db).
     */
    Tlddatabase();
    ~Tlddatabase();

    void openFaceDatabase();
    void createFaceTable();


    int insertFaceModel(unitFaceModel* const)     const;

    /**
     * @brief querybyName
     * @return the index of faceModel in database and "not the tagID"
     */
    int querybyName(const QString&)               const;

    /**
     * @brief queryFaceID
     * @param id:index of the facemodel in the database.
     * @return tagID corresponding to faceModel
     */
    int queryFaceID(int id)                       const;

    /**
     * @brief queryNumfacesinDatabase
     * @return How many face currently exist in the recognition database.
     */
    int queryNumfacesinDatabase()                 const;

    /**
     * @brief querybyFaceid
     * @param faceid:index corresponding to faceModel
     * @return Name corresponding faceModel
     */
    QString        querybyFaceid(int faceid)      const;

    unitFaceModel* getFaceModel(int faceid)       const;
    IplImage*      QImage2IplImage(const QImage&) const;

private:

    QSqlDatabase faceDatabase;
};

} // namespace KFaceIface

#endif // TLD_DATABASE_H
