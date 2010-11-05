/** ===========================================================
 * @file
 *
 * This file is a part of libface project
 * <a href="http://libface.sourceforge.net">http://libface.sourceforge.net</a>
 *
 * @date    2010-03-03
 * @brief   Class for information about a face in an image.
 * @section DESCRIPTION
 *
 * Holds information about a face. The coordinates of the box representing
 * the face and the associated ID. It is used as the output for any face detection.
 * Used as input for any face recognition.
 *
 * @author Copyright (C) 2010 by Alex Jironkin
 *         <a href="alexjironkin at gmail dot com">alexjironkin at gmail dot com</a>
 * @author Copyright (C) 2010 by Gilles Caulier
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

#include "Face.h"

namespace libface
{

Face::Face(int x1, int y1, int x2, int y2, int id, const IplImage* face)
{
    //Set coordinates of the face rectangle;
    this->x1     = x1;
    this->x2     = x2;
    this->y1     = y1;
    this->y2     = y2;

    this->width  = x2 - x1;
    this->height = y2 - y1;

    //Set id of the face. By default it is -1.
    this->id     = id;

    //Set the pointer to the face.
    this->face   = face;
}

Face::~Face()
{
}

void Face::setX1(int x1)
{
    this->x1    = x1;
    this->width = this->x2 - this->x1;
}

void Face::setX2(int x2)
{
    this->x2    = x2;
    this->width = this->x2 - this->x1;
}

void Face::setY1(int y1)
{
    this->y1     = y1;
    this->height = this->y2 - this->y1;
}

void Face::setY2(int y2)
{
    this->y2     = y2;
    this->height = this->y2 - this->y1;
}

void Face::setId(int id)
{
    this->id = id;
}

void Face::setFace(const IplImage* face)
{
    this->face = face;
}

IplImage* Face::takeFace() const
{
    if (this->face)
        return (IplImage*)this->face;

    return 0;
}

const IplImage* Face::getFace() const
{
    if (this->face)
	return this->face;

    return 0;
}

int Face::getHeight() const
{
    return this->height;
}

int Face::getWidth() const
{
    return this->width;
}

int Face::getX1() const
{
    return this->x1;
}

int Face::getX2() const
{
    return this->x2;
}

int Face::getY1() const
{
    return this->y1;
}

int Face::getY2() const
{
    return this->y2;
}

int Face::getId() const
{
    return this->id;
}

} // namespace libface
