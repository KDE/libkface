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

#include <libface/LibFace.h>
#include <libface/Face.h>

#include "kface_global.h"

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
     * @return A QList of QRects, where each rect contains a face
     */
    QList<QRect> detectFaces(const QImage& image);

    /**
     * In the given region of this image, try to recognize a face.
     * If a face is recognized, the identifier associated with this face is returned.
     * If no face is recognized, a null QString is returned.
     * @param image A QImage from which faces will be extracted for recognition
     * @param rectangles A QList of QRects, where each rect contains a face
     */
    QList<QString> recognizeFaces(const QImage& image, const QList<QRect>& rectangles);

    /**
     * Inform that the face with the given identifier is found on the image in the given region.
     * There is no guarantee that rectangle is in the list of rectangles returned by detectFaces(),
     * and no assumption if the face was previously recognized by recognizeFace or not.
     * The identifier is a non-empty string, no format or restriction is specified here.
     */
    void trainFaces(const QImage& image, const QList<QRect>& rectangle, const QList<QString>& identifier);

};

};
#endif // DATABASE_H
