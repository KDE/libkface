/** ===========================================================
 * @file
 *
 * This file is a part of libface project
 * <a href="http://libface.sourceforge.net">http://libface.sourceforge.net</a>
 *
 * @date    2010-03-14
 * @brief   Haar cascades parser
 * @section DESCRIPTION
 *
 * @author Copyright (C) 2010 by Aditya Bhatt
 *         <a href="adityabhatt at gmail dot com">adityabhatt at gmail dot com</a>
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

#include <string>
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <cmath>
#include <cfloat>
#include <climits>
#include <ctime>
#include <cctype>

#include "libopencv.h"
#include "Haarcascades.h"

// TODO: Free memory before erasing cascades from vector, LOTS of exception handling

using namespace std;

namespace libface
{

class Haarcascades::HaarcascadesPriv
{

public:

    HaarcascadesPriv()
    {
        size = 0;
    }

    std::string          cascadePath;
    std::vector<Cascade> cascades;
    std::vector<int>     weights;
    int                  size;
};

Haarcascades::Haarcascades(const string& path)
            : d(new HaarcascadesPriv)
{
    this->d->cascadePath = path;
}

Haarcascades::~Haarcascades()
{
    this->clear();
    d->size = 0;
    delete d;
}

void Haarcascades::addCascade(const Cascade& newCascade, int newWeight)
{
    if (this->hasCascade(newCascade.name))
        return;

    d->cascades.push_back(newCascade);
    d->weights.push_back(newWeight);
    d->size++;
}

void Haarcascades::addCascade(const string& name, int newWeight)
{
    if (this->hasCascade(name))
        return;

    Cascade newCascade;
    newCascade.name     = name;
    newCascade.haarcasc = (CvHaarClassifierCascade*) cvLoad((d->cascadePath + string("/") + name).data(), 0, 0, 0);
    this->addCascade(newCascade, newWeight);
}

bool Haarcascades::hasCascade(const string& name) const
{
    for (int i = 0; i < d->size-1; ++i)
    {
        if (name == d->cascades[i].name)
            return true;
    }
    return false;
}

void Haarcascades::removeCascade(const string& name)
{
    int i;
    for (i = 0; i < d->size-1; ++i)
    {
        if (name == d->cascades[i].name)
            break;
    }

    d->cascades.erase(d->cascades.begin() + i);
    d->weights.erase(d->weights.begin() + i);
    d->size--;
}

void Haarcascades::removeCascade(int index)
{
    d->cascades.erase(d->cascades.begin() + index);
    d->weights.erase(d->weights.begin() + index);
    d->size--;
}

int Haarcascades::getWeight(const string& name) const
{
    for (int i = 0; i < d->size-1; ++i)
    {
        if (name == d->cascades[i].name)
            return d->weights[i];
    }
    return -1;	// No such name found, return -1
}

int Haarcascades::getWeight(int index) const
{
    return d->weights[index];
}

void Haarcascades::setWeight(const string& name, int weight)
{
    int i;
    for (i = 0; i < d->size-1; ++i)
    {
        if (name == d->cascades[i].name)
            break;
    }

    d->weights[i] = weight;
}

void Haarcascades::setWeight(int index, int weight)
{
    d->weights[index] = weight;
}

const Cascade& Haarcascades::getCascade(const string& name) const
{
    int i;
    for (i = 0; i < d->size-1; ++i)
    {
        if (name == d->cascades[i].name)
            break;
    }
    return d->cascades[i];
}

const Cascade& Haarcascades::getCascade(int index) const
{
    return d->cascades[index];
}

int Haarcascades::getSize() const
{
    return d->size;
}

void Haarcascades::clear()
{
    for(unsigned int i = 0; i < d->cascades.size(); ++i)
    {
        cvReleaseHaarClassifierCascade(&d->cascades[i].haarcasc);
    }
    d->cascades.clear();
    d->weights.clear();
    d->size = 0;
}

} // namespace libface
