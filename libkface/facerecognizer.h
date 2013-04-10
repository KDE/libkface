/** ===========================================================
 * @file
 *
 * This file is a part of digiKam project
 * <a href="http://www.digikam.org">http://www.digikam.org</a>
 *
 * @date    2010-03-03
 * @brief   openTLD interface.
 *
 * @author Copyright (C) 2012-2013 by Mahesh Hegde
 *         <a href="mailto:maheshmhegade at gmail dot com">maheshmhegade at gmail dot com</a>
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

#ifndef KFACE_FACERECOGNIZER_H
#define KFACE_FACERECOGNIZER_H

// Local includes

#include "face.h"
#include "image.h"

namespace KFaceIface
{

class FaceRecognizer
{

public:

    FaceRecognizer();
    ~FaceRecognizer();

    QList<float> recognizeFaces(const QList<Face>& faces);
    void storeFaces(const QList<Face>& faces);

    //void   setThreshold(float value);

private:

    class Private;
    Private* const d;
};

} // namespace KFaceIface

#endif // KFACE_FACERECOGNIZER_H
