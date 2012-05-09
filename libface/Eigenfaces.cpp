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

#ifdef WIN32
// To avoid warnings from MSVC compiler about OpenCV headers
#pragma warning( disable : 4996 )
#endif // WIN32

#include <sys/stat.h>

#include <ctime>
#include <cctype>
#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <vector>
#include <sstream>
#include <algorithm>
#include <iterator>

#include "libopencv.h"

//TODO: Check where the data needs to be released to reduce memoryconsumption.
#include "Eigenfaces.h"
#include "FaceDetect.h"
#include "LibFaceUtils.h"

using namespace std;

namespace libface
{

class Eigenfaces::EigenfacesPriv
{

public:

    EigenfacesPriv()
    {
        CUT_OFF     = 10000000.0; //50000000.0;
        UPPER_DIST  = 10000000;
        LOWER_DIST  = 10000000;
        THRESHOLD   = 23000000.0;
        FACE_WIDTH  = 120;
        FACE_HEIGHT = 120;
    }

    /**
     * Performs PCA on the current training data, projects the training faces, and stores them in a DB.
     */
    void learn(int index, IplImage* newFace);

    /**
     * Converts integer to string, convenience function. TODO: Move to Utils
     * @param x The integer to be converted to std::string
     * @return Stringified version of integeer
     */
    inline string stringify(unsigned int x) const;

public:

    // Face data members, stored in the DB
    std::vector<IplImage*> faceImgArr;                 // Array of face images
    std::vector<int>       indexMap;

    // Config data members
    std::string            configFile;

    double                 CUT_OFF;
    double                 UPPER_DIST;
    double                 LOWER_DIST;
    double                 THRESHOLD;
    int                    FACE_WIDTH;
    int                    FACE_HEIGHT;
};

/**
 * Performs PCA on the current training data, projects the training faces, and stores them in a DB.
 */
void Eigenfaces::EigenfacesPriv::learn(int index, IplImage* newFace)
{
    int i;
    std::vector<IplImage*> tempFaces;

    tempFaces.push_back(newFace);
    tempFaces.push_back(faceImgArr.at(index));

    float* projectedFace = (float*)malloc(sizeof(float));

    CvSize size=cvSize(FACE_WIDTH, FACE_HEIGHT);

    //Set PCA's termination criterion
    CvTermCriteria mycrit = cvTermCriteria(CV_TERMCRIT_NUMBER,
            1,0);
    //Initialise pointer to the pointers with eigen objects
    IplImage** eigenObjects = new IplImage *[2];

    float* eigenValues;
    //Initialize array with eigen values
    if( !(eigenValues = (float*) cvAlloc( 2*sizeof(float) ) ) )
        cout<<"Problems initializing eigenValues..."<<endl;

    IplImage* pAvgTrainImg;
    //Initialize pointer to the average image
    if( !(pAvgTrainImg = cvCreateImage( size, IPL_DEPTH_32F, 1) ) )
        cout<<"Problems initializing pAvgTrainImg..."<<endl;

    for(i = 0; i < 2; i++ )
    {
        eigenObjects[i] = cvCreateImage( size, IPL_DEPTH_32F, 1 );
        if(!(eigenObjects[i] ) )
            cout<<"Problems initializing eigenObjects"<<endl;
    }

    //Perform PCA
    cvCalcEigenObjects(2, &tempFaces.front(), eigenObjects,
            CV_EIGOBJ_NO_CALLBACK, 0, NULL, &mycrit, pAvgTrainImg, eigenValues );

    cvEigenDecomposite(tempFaces.at(0), 1, eigenObjects,
            CV_EIGOBJ_NO_CALLBACK, NULL, pAvgTrainImg, projectedFace );

    IplImage* proj = cvCreateImage(size, IPL_DEPTH_8U, 1);
    cvEigenProjection(eigenObjects, 1,
            CV_EIGOBJ_NO_CALLBACK, NULL, projectedFace, pAvgTrainImg, proj);

    //LibFaceUtils::showImage(proj);

    cvReleaseImage(&faceImgArr.at(index));
    faceImgArr.at(index) = proj;

    //free other stuff allocated above.
    cvFree_(eigenValues);
    free(projectedFace);

    cvReleaseImage(&pAvgTrainImg);
    cvReleaseImage(&eigenObjects[0]);
    cvReleaseImage(&eigenObjects[1]);

    tempFaces.clear();
}

string Eigenfaces::EigenfacesPriv::stringify(unsigned int x) const {
    ostringstream o;
    if (!(o << x))
    {
        if (DEBUG)
            cerr << "Could not convert" << endl;
    }

    return o.str();
}

Eigenfaces::Eigenfaces(const string& dir)
          : d(new EigenfacesPriv)
{

    struct stat stFileInfo;
    d->configFile = dir + string("/libface-config.xml");

    if(DEBUG)
        cout << "Config location: " << d->configFile << endl;

    int intStat = stat(d->configFile.c_str(),&stFileInfo);
    if (intStat == 0)
    {
        if (DEBUG)
            cout << "libface config file exists." << endl;

        loadConfig(dir);
    }
    else
    {
        if (DEBUG)
            cout << "libface config file does not exist." << endl;
    }
}

Eigenfaces::~Eigenfaces()
{
    for (std::vector<IplImage*>::iterator it = d->faceImgArr.begin(); it != d->faceImgArr.end(); ++it)
        cvReleaseImage(&(*it));

    d->faceImgArr.clear();
    d->indexMap.clear();

    delete d;
}

int Eigenfaces::count() const
{
    return d->faceImgArr.size();
}

map<string, string> Eigenfaces::getConfig()
{
    map<string, string> config;

    config["nIds"] = d->faceImgArr.size();
    //config["FACE_WIDTH"] = sprintf(value, "%d",d->indexMap.at(i));;

    for ( unsigned int i = 0; i < d->faceImgArr.size(); i++ )
    {
        char facename[200];
        sprintf(facename, "person_%d", i);
        config[string(facename)] = LibFaceUtils::imageToString(d->faceImgArr.at(i));
    }

    for ( unsigned int i = 0; i < d->indexMap.size(); i++ )
    {
        char facename[200];
        sprintf(facename, "id_%d", i);
        char value[10];
        config[string(facename)] = sprintf(value, "%d",d->indexMap.at(i));
    }

    return config;
}

int Eigenfaces::loadConfig(const string& dir)
{
    d->configFile = dir + string("/libface-config.xml");

    if (DEBUG)
        cout << "Load training data" << endl;

    CvFileStorage* fileStorage = cvOpenFileStorage(d->configFile.data(), 0, CV_STORAGE_READ);
    cout << "opened" << endl;
    if (!fileStorage)
    {
        if (DEBUG)
            cout << "Can't open config file for reading :" << d->configFile << endl;
        return 1;
    }

    //d->clearTrainingStructures();

    int nIds       = cvReadIntByName(fileStorage, 0, "nIds", 0), i;
    d->FACE_WIDTH  = cvReadIntByName(fileStorage, 0, "FACE_WIDTH",d->FACE_WIDTH);
    d->FACE_HEIGHT = cvReadIntByName(fileStorage, 0, "FACE_HEIGHT",d->FACE_HEIGHT);
    d->THRESHOLD   = cvReadRealByName(fileStorage, 0, "THRESHOLD", d->THRESHOLD);
    //LibFaceUtils::printMatrix(d->projectedTrainFaceMat);

    for ( i = 0; i < nIds; i++ )
    {
        char facename[200];
        sprintf(facename, "person_%d", i);
        d->faceImgArr.push_back( (IplImage*)cvReadByName(fileStorage, 0, facename, 0) );
    }

    for ( i = 0; i < nIds; i++ )
    {
        char idname[200];
        sprintf(idname, "id_%d", i);
        d->indexMap.push_back( cvReadIntByName(fileStorage, 0, idname, 0));
    }

    // Release file storage
    cvReleaseFileStorage(&fileStorage);

    return 0;
}

int Eigenfaces::loadConfig(const map<string, string>& c)
{
    // FIXME: Because std::map has no convenient const accessor, make a copy.
    map<string, string> config(c);
    if (DEBUG)
        cout<<"Load config data from a map"<<endl;

    int nIds  = atoi(config["nIds"].c_str()), i;

    //Not sure what depath and # of channels should be in faceImgArr. Store them in config?
    for ( i = 0; i < nIds; i++ )
    {
        char facename[200];
        sprintf(facename, "person_%d", i);
        d->faceImgArr.push_back( LibFaceUtils::stringToImage(config[string(facename)], IPL_DEPTH_32F, 1) );
    }

    for ( i = 0; i < nIds; i++ )
    {
        char idname[200];
        sprintf(idname, "id_%d", i);
        d->indexMap.push_back( atoi(config[string(idname)].c_str()));
    }

    return 0;
}

pair<int, float> Eigenfaces::recognize(IplImage* input)
{
    if (input == 0)
    {
        if (DEBUG)
            cout << "No faces passed. No recognition to do." << endl;

        return make_pair<int, float>(-1, -1); // Nothing
    }

    float minDist = FLT_MAX;
    int id        = -1;
    clock_t recog = clock();
    std::vector<IplImage*> tempFaces;
    tempFaces.push_back(input);

    unsigned int j, i;
    for( j = 0; j<d->faceImgArr.size(); j++)
    {
        tempFaces.push_back(d->faceImgArr.at(j));

        float* eigenValues;
        //Initialize array with eigen values
        if( !(eigenValues = (float*) cvAlloc( 2*sizeof(float) ) ) )
            cout << "Problems initializing eigenValues..." << endl;

        float* projectedTestFace = (float*)malloc(sizeof(float));

        CvSize size=cvSize(tempFaces.at(0)->width, tempFaces.at(0)->height);

        //Set PCA's termination criterion
        CvTermCriteria mycrit = cvTermCriteria(CV_TERMCRIT_NUMBER,
                1,0);
        //Initialise pointer to the pointers with eigen objects
        IplImage** eigenObjects = new IplImage *[2];

        IplImage* pAvgTrainImg;
        //Initialize pointer to the average image
        if( !(pAvgTrainImg = cvCreateImage( size, IPL_DEPTH_32F, 1) ) )
            cout<<"Problems initializing pAvgTrainImg..."<<endl;

        for(i = 0; i < 2; i++ )
        {
            eigenObjects[i] = cvCreateImage( size, IPL_DEPTH_32F, 1 );
            if(!(eigenObjects[i] ) )
                cout<<"Problems initializing eigenObjects"<<endl;
        }

        //Perform PCA
        cvCalcEigenObjects(2, &tempFaces.front(), eigenObjects,
                CV_EIGOBJ_NO_CALLBACK, 0, NULL, &mycrit, pAvgTrainImg, eigenValues );

        //This is a simple min distance mechanism for recognition. Perhaps we should check similarity of
        //images.
        if(eigenValues[0] < minDist)
        {
            minDist = eigenValues[0];
            id = j;
        }

        //cvEigenDecomposite(tempFaces.at(0), nEigens, eigenObjects,
        //CV_EIGOBJ_NO_CALLBACK, NULL, pAvgTrainImg, projectedTestFace );

        //IplImage *proj = cvCreateImage(cvSize(input->width, input->height), IPL_DEPTH_8U, 1);
        //cvEigenProjection(eigenObjects, nEigens,
        //        CV_EIGOBJ_NO_CALLBACK, NULL, projectedTestFace, pAvgTrainImg, proj);

        //LibFaceUtils::showImage(proj);

        free(projectedTestFace);
        cvFree_(eigenValues);
        cvReleaseImage(&pAvgTrainImg);
        cvReleaseImage(&eigenObjects[0]);
        cvReleaseImage(&eigenObjects[1]);

        tempFaces.pop_back();
    }

    tempFaces.clear();

    recog = clock() - recog;
    if (DEBUG)
        printf("Recognition took: %f sec.\n", (double)recog / ((double)CLOCKS_PER_SEC));

    if(minDist > d->THRESHOLD)
    {
        id = -1;
        minDist = -1;

        if(DEBUG)
            printf("The value is below the threshold.\n");
    }
    else
    {
        if(DEBUG)
            printf("The value is: %f\n.", minDist);
    }

    return make_pair<int, float>(id, minDist);
}

int Eigenfaces::saveConfig(const string& dir)
{
    if (DEBUG)
        cout << "Saving config in "<< dir << endl;

    string configFile          = dir + string("/libface-config.xml");
    CvFileStorage* fileStorage = cvOpenFileStorage(d->configFile.c_str(), 0, CV_STORAGE_WRITE);

    if (!fileStorage)
    {
        if (DEBUG)
            cout << "Cant open for storing :" << d->configFile << endl;

        return 1;
    }

    // Start storing
    unsigned int nIds = d->faceImgArr.size(), i;

    // Write some initial params and matrices
    cvWriteInt( fileStorage, "nIds", nIds );
    cvWriteInt( fileStorage, "FACE_WIDTH", d->FACE_WIDTH);
    cvWriteInt( fileStorage, "FACE_HEIGHT", d->FACE_HEIGHT);
    cvWriteReal( fileStorage, "THRESHOLD", d->THRESHOLD);

    // Write all the training faces
    for ( i = 0; i < nIds; i++ )
    {
        char facename[200];
        sprintf(facename, "person_%d", i);
        cvWrite(fileStorage, facename, d->faceImgArr.at(i), cvAttrList(0,0));
    }

    for ( i = 0; i < nIds; i++ )
    {
        char idname[200];
        sprintf(idname, "id_%d", i);
        cvWriteInt(fileStorage, idname, d->indexMap.at(i));
    }

    // Release the fileStorage
    cvReleaseFileStorage(&fileStorage);
    return 0;
}

std::vector<int> Eigenfaces::update(vector<Face>& newFaceArr)
{
    std::vector<int> result (newFaceArr.size(),-1);
    if (newFaceArr.size() == 0)
    {
        if (DEBUG)
            cout << " No faces passed. Not training." << endl;

        return result;
    }

    clock_t update;
    update = clock();

    unsigned int i;

    // Our method : If no Id specified (-1), then give the face the next available ID.
    // Now, all ID's, when the DB is read back, were earlier read as the storage index with which the face was stored in the DB.
    // Note that indexIdMap, unlike idCountMap, is only changed when a face with a new ID is added.
    for (i = 0; i < newFaceArr.size() ; ++i)
    {
        if(newFaceArr.at(i).getId() == -1)
        {
            if (DEBUG)
                cout << "Has no specified ID" << endl;

            int newId = d->faceImgArr.size();

            // We now have the greatest unoccupied ID.
            if (DEBUG)
                cout << "Giving it the ID = " << newId << endl;

            d->faceImgArr.push_back(cvCloneImage(newFaceArr.at(i).getFace()));
            newFaceArr.at(i).setId(newId);

            // A new face with a new ID is added. So map it's DB storage index with it's ID
            //d->indexIdMap[newId] = newId;
            d->indexMap.push_back(newId);
        }
        else
        {
            int id = newFaceArr.at(i).getId();

            if (DEBUG)
                cout << " Given ID as " << id << endl;

            find (d->indexMap.begin(), d->indexMap.end(), id);

            std::vector<int>::iterator it = find(d->indexMap.begin(), d->indexMap.end(), id);//d->indexMap.
            if(it != d->indexMap.end())
            {
                //unsigned int j = 0;

                if (DEBUG)
                    cout<<"Specified ID already axists in DB, averaging"<<endl;

                d->learn(*it, cvCloneImage(newFaceArr.at(i).getFace()));

            }
            else
            {
                // If this is a fresh ID, and not autoassigned
                if (DEBUG)
                    cout << "Specified ID does not exist in DB, so this is a new face" << endl;

                d->faceImgArr.push_back(cvCloneImage(newFaceArr.at(i).getFace()));
                // A new face with a new ID is added. So map it's DB storage index with it's ID
                d->indexMap.push_back(id);
            }
        }
        result.at(i) = newFaceArr.at(i).getId();
    }

    update = clock() - update;
    if (DEBUG)
        printf("Updating took: %f sec.\n", (double)update / ((double)CLOCKS_PER_SEC));

    return result;
}

} // namespace libface
