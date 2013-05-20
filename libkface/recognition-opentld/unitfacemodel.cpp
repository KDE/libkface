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

#include "unitfacemodel.h"

namespace KFaceIface
{

UnitFaceModel::UnitFaceModel()
    : objWidth(0),
      objHeight(0),
      minVar(0)
{
}

UnitFaceModel::~UnitFaceModel()
{
}

template <class List>
static void serialize(const List& list, QByteArray& dest)
{
    dest.clear();
    QDataStream out(&dest, QIODevice::WriteOnly);
    out << list;
}

void UnitFaceModel::serialisePositivePatches(const QList<QList<float> >& allPositivePatches)
{
    serialize(allPositivePatches, serialisedPositivePatches);
}

void UnitFaceModel::serialiseNegativePatches(const QList<QList<float> >& allNegativePatches)
{
    serialize(allNegativePatches, serialisedNegativePatches);
}

void UnitFaceModel::serialiseFeatures(const QList<QList<QList<float> > >& allFeatures)
{
    serialize(allFeatures, serialisedFeatures);
}

void UnitFaceModel::serialiseLeaves(const QList<QList<QList<int> > >& allLeaves)
{
    serialize(allLeaves, serialisedLeaves);
}

template <class List>
static List deserialize(const QByteArray& src)
{
    List list;
    QDataStream in(src);
    in >> list;
    return list;
}

QList<QList<float> > UnitFaceModel::deserialisePositivePatches() const
{
    return deserialize<QList<QList<float> > >(serialisedPositivePatches);
}

QList<QList<float> > UnitFaceModel::deserialiseNegativePatches() const
{
    return deserialize<QList<QList<float> > >(serialisedNegativePatches);
}

QList<QList<QList<float> > > UnitFaceModel::deserialiseFeatures() const
{
    return deserialize<QList<QList<QList<float> > > >(serialisedFeatures);
}

QList<QList<QList<int> > > UnitFaceModel::deserialiseLeaves() const
{
    return deserialize<QList<QList<QList<int> > > >(serialisedLeaves);
}

} // namespace KFaceIface
