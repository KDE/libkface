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
 * Face detection through openTLD interface.
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

#ifndef TLD_FACE_H
#define TLD_FACE_H

// C++ includes

#include <iostream>

// Qt includes

#include <QtCore/QList>
#include <QtCore/QBuffer>
#include <QtCore/QVariant>

// Local includes

#include "libkface_export.h"

namespace KFaceIface
{

class KFACE_EXPORT UnitFaceModel
{
public:

    /**
     * @brief UnitFaceModel:describe single face.(OpenTLD communicates about the face data through this class object)
     * This is a Convinient class to hold facemodel to be compatible for OpenTLD demanding features of object(face)
     * Objects of this class are used to set parameters of OpenTLD recognizer(tld).
     */
    UnitFaceModel();
    ~UnitFaceModel();

    /**
     * @brief Following four methoods used to serialise the dynamic data generated during initial learning of OpenTLD
     * so that data can be stored in database
     */
    void serialisePositivePatches(const QList<QList<float> >&);
    void serialiseNegativePatches(const QList<QList<float> >&);
    void serialiseFeatures(const QList<QList<QList<float> > >&);
    void serialiseLeaves(const QList<QList<QList<int> > >&);

    /**
     * @Perform reverse of the above operation
     */
    QList<QList<float> > deserialisePositivePatches()  const;
    QList<QList<float> >  deserialiseNegativePatches() const;
    QList<QList<QList<float> > > deserialiseFeatures() const;
    QList<QList<QList<int> > > deserialiseLeaves()     const;

public:

    QByteArray serialisedPositivePatches;
    QByteArray serialisedNegativePatches;
    QByteArray serialisedFeatures;
    QByteArray serialisedLeaves;

    int     objWidth;
    int     objHeight;

    float   minVar;
};

} // namespace KFaceIface

#endif // TLD_FACE_H
