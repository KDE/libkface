/*  Copyright 2011 AIT Austrian Institute of Technology
*
*   This file is part of OpenTLD.
*
*   OpenTLD is free software: you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*   (at your option) any later version.
*
*   OpenTLD is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with OpenTLD.  If not, see <http://www.gnu.org/licenses/>.
*
*/

/*
 * main.h
 *
 *  Created on: Nov 18, 2011
 *      Author: Georg Nebehay
 */

#ifndef MAIN_H_
#define MAIN_H_

#include "TLD.h"

enum Retval
{
    PROGRAM_EXIT = 0,
    SUCCESS = 1
};

class Main
{
public:
    tld::TLD *tld;
    double threshold;
    int *initialBB;
    int seed;

    Main()
    {
        tld = new tld::TLD();
        threshold = 0.5;
        initialBB = NULL;
        seed = 0;
    }

    ~Main()
    {
        delete tld;
    }

    void generatefacemodel(IplImage* , unitFaceModel *modeltostore);
    float getrecognitionconfidence(IplImage* , unitFaceModel *comparemodel);
    void printFaceModel();
};

#endif /* MAIN_H_ */
