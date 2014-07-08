/** ===========================================================
 * @file
 *
 * This file is a part of digiKam project
 * <a href="http://www.digikam.org">http://www.digikam.org</a>
 *
 * @date   2010-06-21
 * @brief  CLI test program for trainign DB
 *
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

// Qt includes

#include <QApplication>
#include <QDir>
#include <QImage>
#include <QTime>

// KDE includes

#include <kdebug.h>

// libkface includes

#include "libkface/recognitiondatabase.h"

using namespace KFaceIface;

class SimpleTrainingDataProvider : public TrainingDataProvider
{
public:

    SimpleTrainingDataProvider(const Identity& identity, const QImage& newImage)
        : identity(identity), toTrain(QList<QImage>() << newImage)
    {
    }

    ImageListProvider* newImages(const Identity& id)
    {
        if (identity == id)
        {
            toTrain.reset();
            return &toTrain;
        }

        return &empty;
    }

    ImageListProvider* images(const Identity&)
    {
        return &empty;
    }

public:

    Identity               identity;
    QListImageListProvider toTrain;
    QListImageListProvider empty;
};

// --------------------------------------------------------------------------------------------------

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);

    RecognitionDatabase db = RecognitionDatabase::addDatabase(QDir::currentPath());

    QImage image(256, 256, QImage::Format_ARGB32);
    image.fill(Qt::red);

    Identity identity;

    for (int i=0 ; i < 100 ; i++)
    {
        QString name = QString("face%1").arg(i);
        kDebug() << "Training " << name;
        QMap<QString, QString> attributes;
        attributes["name"] = name;
        identity           = db.addIdentity(attributes);
        SimpleTrainingDataProvider data(identity, image);
        db.train(identity, &data, "test application");    
    }   
    
    return 0;
}
