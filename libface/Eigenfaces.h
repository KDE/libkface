/** ===========================================================
 * @file
 *
 * This file is a part of libface project
 * <a href="http://libface.sourceforge.net">http://libface.sourceforge.net</a>
 *
 * @date    2009-12-27
 * @brief   Eigenfaces parser
 * @section DESCRIPTION
 *
 * This class is an implementation of Eigenfaces. The image stored is the projection of the faces with the
 * closest match. The relation in recognition is determined by the Eigen value when decomposed.
 *
 * @author Copyright (C) 2009-2010 by Alex Jironkin
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

#ifndef _EIGENFACES_H_
#define _EIGENFACES_H_

#include <string>

#include "LibFaceConfig.h"
#include "LibFaceCore.h"

namespace libface
{

class FACEAPI Eigenfaces : public LibFaceRecognitionCore
{
public:

    /**
     * Constructor for Eigenfaces. Takes a directory string as argument to determine the location of config
     * xml file.
     *
     * @param dir The directory in which the DB is to be found/created
     */
    Eigenfaces(const std::string& dir = ".");

    /**
     * Deconstructor that frees the data variables.
     */
    ~Eigenfaces();

    /**
     * Returns the number of unique faces in the database.
     */
    int count() const;

    /**
     * Get the mapping between config variables and the data. This can be stored and then
     * loaded back into config.
     *
     * @return Returns a config std::map with variable names as keys and data encoded as std::string.
     */
    std::map<std::string, std::string> getConfig();

    /**
     * Method for loading the mapping of config variables and the data back into libface.
     *
     * @param config A std::map config returned by getConfig() method.
     *
     * @return Returns 0 if operation was successful, or positive error codeotherwise.
     */
    int loadConfig(const std::map<std::string, std::string>& config);

    /**
     * Attempts to load config from specified directory.
     *
     * @param dir A directory to look for libface-config.xml file.
     *
     * @return Returns 0 if config was loaded or positive error code otherwise.
     */
    int loadConfig(const std::string& dir);

    /**
     * Method to attempt to compare images with the known projected images. Uses a specified type of
     * distance to see how far away they are from each of the images in the projection.
     *
     * @param input The pointer to IplImage * image, which is to be recognized.
     *
     * @return A pair with ID and closeness of the closest face.
     *
     */
    std::pair<int, float> recognize(IplImage* input);

    /**
     * Saves the config is a given directory.
     *
     * @param dir A std::string path to directory where config should be stored.
     *
     * @return Returns 0 if operation was successful, or positive error code otherwise.
     */
    int saveConfig(const std::string& dir);

    /**
     * Updates the config with a vector of input training faces.
     * If id of the face is -1, then face is added to the end of the faces vector.
     *
     * If id is not -1 and a new id, then face is added to the end of the faces vector.
     *
     * If id is not -1 and it already exist, then given the given faces is projected together with the
     * known face at that position using eigen decomposition and the projected face is stored in it's place.
     *
     * @param newFaceArr The vector of input Face objects
     *
     * @return Returns 0 if update was successful, or positive int otherwise.
     */
    std::vector<int> update(std::vector<Face>&);

private:

    class EigenfacesPriv;
    EigenfacesPriv* const d;
};

} // namespace libface

#endif /* _EIGENFACES_H_ */
