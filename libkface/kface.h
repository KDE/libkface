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
    
    /** Will set the co-ordinates of KFace object to the specified rectangle
     * @param rect The QRect rectangle which is to be set as the rectangle for KFace instance
     */
    void setRect(const QRect& rect);
    
    /** Will return a QRect of the KFace object, for better interop with functions that don't want to directly use KFace
     * @return A QRect version of the bounding box for a face
     */
    QRect getRect();
    /** Assignment operator that assigns a KFace's data to another KFace
     * @param other A reference to a KFace object
     * @return A reference to the copied KFace object
     */
    KFace& operator=(KFace& other);
    
    /** Assignment operator that assigns a Face's data to another KFace
     * @param other A reference to a Face object
     * @return A reference to the copied KFace object
     */
    KFace& operator=(libface::Face& other);
    
    

};

};
#endif // KFACE_H
