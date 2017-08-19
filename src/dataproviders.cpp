/** ===========================================================
 * @file
 *
 * This file is a part of KDE project
 *
 *
 * @date  2013-05-18
 * @brief Wrapper class for face recognition
 *
 * @author Copyright (C) 2013 by Marcel Wiesweg
 *         <a href="mailto:marcel dot wiesweg at gmx dot de">marcel dot wiesweg at gmx dot de</a>
 * @author Copyright (C) 2014 by Gilles Caulier
 *         <a href="mailto:caulier dot gilles at gmail dot com">caulier dot gilles at gmail dot com</a>
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

// Local includes

#include "dataproviders.h"

namespace KFaceIface
{

ImageListProvider::~ImageListProvider()
{
}

// ----------------------------------------------------------------------------------------

QListImageListProvider::QListImageListProvider(const QList<QImage>& lst)
    : list(lst),
      it(list.constBegin())
{
}

QListImageListProvider::QListImageListProvider()
    : it(list.constBegin())
{
}

int  QListImageListProvider::size()  const
{
    return list.size();
}

bool QListImageListProvider::atEnd() const
{
    return it == list.constEnd();
}

void QListImageListProvider::proceed(int steps)
{
    it += steps;
}

void QListImageListProvider::reset()
{
    it = list.constBegin();
}

QImage QListImageListProvider::image()
{
    return *it;
}

// ----------------------------------------------------------------------------------------

int  EmptyImageListProvider::size()  const
{
    return 0;
}

bool EmptyImageListProvider::atEnd() const
{
    return true;
}

void EmptyImageListProvider::proceed(int steps)
{
    Q_UNUSED(steps)
}

QImage EmptyImageListProvider::image()
{
    return QImage();
}

// ----------------------------------------------------------------------------------------

TrainingDataProvider::~TrainingDataProvider()
{
}

} // namespace KFaceIface
