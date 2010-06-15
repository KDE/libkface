/**
* This file is part of libkface.
*
* libkface is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 2 of the License, or
* (at your option) any later version.
*
* libkface is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with libkface.  If not, see <http://www.gnu.org/licenses/>.
*
* @note: The Database class wraps the libface database
* @author: Aditya Bhatt
*/

#ifndef DATABASE_H
#define DATABASE_H

#include <QExplicitlySharedDataPointer>
#include <QImage>
#include <QString>
#include <QSharedDataPointer>
#include <QPair>

#include <libface/LibFace.h>
#include <libface/Face.h>

#include "kface_global.h"
#include "kface.h"

namespace kface
{

class DatabasePriv;

class KFACESHARED_EXPORT Database {
private:
    QExplicitlySharedDataPointer<DatabasePriv> d;
    
    
    Database(const QString& configurationPath);
    qint32 indentifierToQint32(const QString& identifier);

public:
    Database();
    Database(const Database& other);
    ~Database();
    Database& operator=(const Database& other);

    /**
     * Creates a Face Detection/Recognition database, using the given directory
     * for configuration. If no directory is specified, creates a new directory
     * at a default place as obtained from kdelibs.
     * @param configurationPath The directory to look in for the configuration
     * @return A Database object as read from the config
     */
    static Database database(const QString& configurationPath = QString());

    /** 
     * Explicitly save configuration. Automatically done in destructor.
     */
    void saveConfig();

    /**
     * Scan an image for faces. Return a list with regions possibly
     * containing faces.
     * @param image The image in which faces are to be detected
     * @return A QList of detected KFace *'s, with the extracted face images loaded into them.
     */
    QList<KFace *> detectFaces(const QImage& image);

    /**
     * Update the training database with a QList of KFace pointers which hold the face images
     * Faces that have not been given any ID by the caller will automatically be given the next available ID,
     * and this ID will be updated in the KFace objects.
     * @param faces A QList of KFace *'s, which hold the face image too, for updating the DB.
     */
    void updateFaces(QList<KFace *>& faces);
    
    /**
     * Function to recognize faces in a QList of KFace pointers which hold the face images.
     * Recognized faces will have their ID's changed in the KFace objects
     * @param faces A QList of KFace *'s, which hold the face image too, for recongition.
     * @return A QList of "closeness" of recognized faces, in the same order as the argument
     */
    QList<double> recognizeFaces(QList<KFace *>& faces);

};

};
#endif // DATABASE_H
