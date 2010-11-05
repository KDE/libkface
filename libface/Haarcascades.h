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

#ifndef _HAARCASCADES_H_
#define _HAARCASCADES_H_

#include <string>

#include "LibFaceConfig.h"

namespace libface
{

// A Cascade structure has :
// a name (for easy reference),
// a filename (to load the actual cascade from)
// a CvHaarClassifierCascade object pointer

typedef struct CascadeStruct
{
    std::string              name;
    CvHaarClassifierCascade* haarcasc;
} Cascade;

class FACEAPI Haarcascades
{

public:

    /**
     * Constructor which initializes the cascade directory's path
     *
     * @param path The path to the directory containing the cascade
     */
    Haarcascades(const std::string& path);

    /**
     * Destructor for Haarcascades
     */
    ~Haarcascades();

    /**
     * Adds a new cascade with a specified d->weights
     * @param newCascade A Cascade object
     * @param newWeight The weight of the cascade
     */
    void addCascade(const Cascade& newCascade, int newWeight);

    /**
     * Adds a new cascade with a specified d->weights
     * @param name The filename of the cascade
     * @param weight The weight of the cascade
     */
    void addCascade(const std::string& name, int weight);

    /**
     * Removes a cascade with the specified name
     * @param name The name of the cascade to be removed
     */
    void removeCascade(const std::string& name);

    /**
     * Removes a cascade with the specified index
     * @param index The index of the cascade to be removed
     */
    void removeCascade(int index);

    /**
     * Sets the weight of the cascade with the specified name
     * @param name The name of the cascade whose weight is to be set
     * @param weight The new weight
     */
    void setWeight(const std::string& name, int weight);

    /**
     * Sets the weight of the cascade with the specified index
     * @param index The index of the cascade whose weight is to be set
     * @param weight The new weight
     */
    void setWeight(int index, int weight);

    /**
     * Returns the weight of the cascade with the specified name
     * @param name The name of the cascade whose weight is desired
     * @return The weight of the cascade
     */
    int getWeight(const std::string& name) const;

    /**
     * Returns the weight of the cascade with the specified index
     * @param index The index of the cascade whose weight is desired
     * @return The weight of the cascade
     */
    int getWeight(int index) const;

    /**
     * Checks whether the cascade is in the set
     * @param name The name of the cascade whose presence is to be checked
     * @return Whether the cascade exists in the set or not
     */
    bool hasCascade(const std::string& name) const;

    /**
     * Returns the Cascade object for the Cascade with the specified name
     * @param name The name of the desired cascade
     * @return The Cascade object with the name
     */
    const Cascade& getCascade(const std::string& name) const;

    /**
     * Returns the Cascade object for the Cascade with the specified index
     * @param index The index of the desired cascade
     * @return The Cascade object with the index
     */
    const Cascade& getCascade(int index) const;

    /**
     * Returns the number of cascades loaded into the set
     * @return The number of cascades
     */
    int getSize() const;

    /**
     * Clears the entire set of cascades
     */
    void clear();

private:

    class HaarcascadesPriv;
    HaarcascadesPriv* const d;
};

} // namespace libface

#endif // _HAARCASCADES_H_
