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
* @note: This is a derivative of Face class
* @author: Aditya Bhatt
*/

#ifndef KFACE_H
#define KFACE_H

#include <QImage>
#include <libface/Face.h>

#include "kfaceutils.h"

namespace kface {
    
class KFace : public libface::Face
{
public:

    /** Will convert given QImage to an internal IplImage.
     * @param image The QImage to be set as the face image for the KFace object
     */
    void setImage(const QImage& image);
    
    /** Will return a QImage version of the internal face image stored in the KFace object
     * @return The QImage version of the internal face image
     */
    QImage* getImage();
    
    /** Assignment operator that assigns a KFace's data to another KFace
     * @param other A reference to a KFace object
     * @return A reference to the copied KFace object
     */
    
    KFace& operator=(KFace& other);
    
    KFace& operator=(libface::Face& other);

};

};
#endif // KFACE_H
