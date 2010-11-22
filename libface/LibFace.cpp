/** ===========================================================
 * @file
 *
 * This file is a part of libface project
 * <a href="http://libface.sourceforge.net">http://libface.sourceforge.net</a>
 *
 * @date    2010-02-18
 * @brief   Lead Face library class.
 * @section DESCRIPTION
 *
 * @author Copyright (C) 2010 by Alex Jironkin
 *         <a href="alexjironkin at gmail dot com">alexjironkin at gmail dot com</a>
 * @author Copyright (C) 2010 by Aditya Bhatt
 *         <a href="adityabhatt at gmail dot com">adityabhatt at gmail dot com</a>
 * @author Copyright (C) 2010 by Gilles Caulier
 *         <a href="mailto:caulier dot gilles at gmail dot com">caulier dot gilles at gmail dot com</a>
 * @author Copyright (C) 2010 by Marcel Wiesweg
 *         <a href="mailto:marcel dot wiesweg at gmx dot de">marcel dot wiesweg at gmx dot de</a>
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

#include <sys/stat.h>

#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <cctype>
#include <cerrno>
#include <vector>
#include <sstream>
#include <algorithm>
#include <iterator>

//TODO: Scale images here.
#include "LibFace.h"
#include "LibFaceUtils.h"
#include "FaceDetect.h"
#include "Face.h"

using namespace std;

namespace libface
{

class LibFace::LibFacePriv
{

public:

    LibFacePriv()
    {
        detectionCore   = 0;
        recognitionCore = 0;
        lastImage       = 0;
    }

    Mode                    type;
    std::string             cascadeDir;
    LibFaceDetectCore*      detectionCore;
    LibFaceRecognitionCore* recognitionCore;
    IplImage*               lastImage;
    std::string             lastFileName;

    static int              facesize() { return 120; }
};

LibFace::LibFace(Mode type, const string& configDir, const string& cascadeDir)
       : d(new LibFacePriv)
 {
    d->type = type;

    cout << "Cascade directory located as : " << cascadeDir << endl;

    // We don't need Eigenfaces if we just want detection, and vice versa.
    // So there is a case for everything.
    switch (d->type)
    {
        case DETECT:
            d->cascadeDir      = cascadeDir;
            d->detectionCore   = new FaceDetect(d->cascadeDir);
            break;
        case EIGEN:
            d->recognitionCore = new Eigenfaces(configDir);
            break;
        default:    // Initialize both detector and Eigenfaces
            d->cascadeDir      = cascadeDir;
            d->detectionCore   = new FaceDetect(d->cascadeDir);
            d->recognitionCore = new Eigenfaces(configDir);
            break;
    }
}

LibFace::~LibFace()
{
    switch(d->type)
    {
        case DETECT:
            delete d->detectionCore;
            break;
        case EIGEN:
            delete d->recognitionCore;
            break;
        default:
            delete d->detectionCore;
            delete d->recognitionCore;
            break;
    }
    cvReleaseImage(&d->lastImage);

    delete d;
}

vector<Face> LibFace::detectFaces(const string& filename, int /*scaleFactor*/)
{
    vector<Face> result;
    if(filename.length() == 0)
    {
        if (DEBUG)
        {
            cout<<"No image passed for detection"<<endl;
            return result;
        }
    }
    //Check if image was already loaded once, by checking last loaded filename.
    if (filename != d->lastFileName)
    {
        d->lastFileName = filename;
        cvReleaseImage(&d->lastImage);
        d->lastImage    = cvLoadImage(filename.data(), CV_LOAD_IMAGE_GRAYSCALE);
    }

    return d->detectionCore->detectFaces(d->lastImage);
}

vector<Face> LibFace::detectFaces(const char* arr, int width, int height, int step, int depth, int channels, int /*scaleFactor*/)
{
    IplImage* image = LibFaceUtils::charToIplImage(arr, width, height, step, depth, channels);
    return d->detectionCore->detectFaces(image);
}

vector<Face> LibFace::detectFaces(const IplImage* image, const CvSize& originalSize)
{
    return d->detectionCore->detectFaces(image, originalSize);
}

map<string,string> LibFace::getConfig()
{
    map<string,string> result = d->recognitionCore->getConfig();
    return result;
}

int LibFace::loadConfig(const string& /*dir*/)
{
    int result = 0;
/*
    d->recognitionCore->loadData(dir);
*/

    return result;
}

int LibFace::loadConfig(const map<string, string>& config)
{
    int result  = d->recognitionCore->loadConfig(config);
    return result;
}

vector<pair<int, double> > LibFace::recognise(const string& filename, vector<Face>* faces, int scaleFactor)
{
    IplImage* img = cvLoadImage(filename.data(), CV_LOAD_IMAGE_GRAYSCALE); // grayscale
    return this->recognise(img, faces, scaleFactor);
    cvReleaseImage(&img);       // FIXME : This line is never called !!!
}

vector<pair<int, double> > LibFace::recognise(const IplImage* img, vector<Face>* faces, int /*scaleFactor*/)
{
    vector<pair<int, double> > result;

    if (faces->size() == 0)
    {
        if (DEBUG)
            cout<<" No faces passed to libface::recognise() , not recognizing..." << endl;
        return result;
    }

    if (!img)
    {
        if (DEBUG)
            cout<<" Null image passed to libface::recognise() , not recognizing..." << endl;
        return result;
    }

    if (DEBUG)
    {
        cout << "Will recognize." << endl;
    }

    vector<IplImage*> newFaceImgArr;

    int size = faces->size();
    for (int i=0 ; i<size ; i++)
    {
        Face* face = &faces->at(i);
        int x1     = face->getX1();
        int y1     = face->getY1();
        int width  = face->getWidth();
        int height = face->getHeight();

        // Extract face-image from whole-image.
        CvRect rect       = cvRect(x1, y1, width, height);
        IplImage* faceImg = LibFaceUtils::copyRect(img, rect);

        // Make into d->facesize*d->facesize standard-sized image
        IplImage* sizedFaceImg = cvCreateImage(cvSize(d->facesize(), d->facesize()), img->depth, img->nChannels);
        cvResize(faceImg, sizedFaceImg);

        // Extracted. Now push it into the newfaces vector
        newFaceImgArr.push_back(sizedFaceImg);
    }

    // List of Face objects made
    // Now recognize
    for (int i = 0; i < size; ++i)
        result.push_back(d->recognitionCore->recognize(newFaceImgArr.at(i)));

    for (unsigned int i=0; i<newFaceImgArr.size(); i++)
        cvReleaseImage(&newFaceImgArr[i]);

    return result;
}

vector<pair<int, double> > LibFace::recognise(const char* arr, vector<Face>* faces, int width, int height, int step, int depth, int channels, int scaleFactor)
{
    IplImage* img = LibFaceUtils::charToIplImage(arr, width, height, step, depth, channels);
    return this->recognise(img, faces, scaleFactor);
}

vector<pair<int, double> > LibFace::recognise(vector<Face>* faces, int /*scaleFactor*/)
{
    vector<pair<int, double> > result;

    if (faces->size() == 0)
    {
        if (DEBUG)
            cout<<" No faces passed to libface::recognise() , not recognizing." << endl;
        return result;
    }

    if (DEBUG)
    {
        cout << "Recognizing." << endl;
    }

    vector<IplImage*> newFaceImgArr;

    int size = faces->size();
    for (int i=0 ; i<size ; i++)
    {
        Face* face = &faces->at(i);
        int id     = face->getId();

        if (DEBUG)
            cout << "Id is: " << id << endl;

        const IplImage* faceImg = face->getFace();
        IplImage* createdImg    = 0;

        if (!faceImg)
        {
            if (DEBUG)
                cout << "Face with null image passed to libface::recognise(), skipping";
            continue;
        }

        if (faceImg->width != d->facesize() || faceImg->height != d->facesize())
        {
            // Make into d->facesize*d->facesize standard-sized image
            createdImg = cvCreateImage(cvSize(d->facesize(), d->facesize()), faceImg->depth, faceImg->nChannels);
            cvResize(faceImg, createdImg);
        }
        else
        {
            // we need a non-const image for cvEigenDecomposite
            createdImg = cvCloneImage(faceImg);
        }

        result.push_back(d->recognitionCore->recognize(createdImg));

        face->setId(result.at(i).first);

        cvReleaseImage(&createdImg);
    }

    if (DEBUG)
        cout << "Size of result = " << result.size();
    return result;
}

int LibFace::saveConfig(const string& dir)
{
    int result  = 0;
    d->recognitionCore->saveConfig(dir);
    return result;
}

int LibFace::train(const string& /*dir*/)
{
    int result = 0;
//    d->recognitionCore->train(dir);
    return result;
}

std::vector<int> LibFace::update(const IplImage* img, vector<Face>* faces, int /*scaleFactor*/)
{
    std::vector<int> assignedIDs;

    if (faces->size() == 0)
    {
        if (DEBUG)
            cout<<" No faces passed to update." << endl;
        return assignedIDs;
    }

    if (DEBUG)
    {
        cout << "Update with faces." << endl;
    }

    vector<Face>      newFaceArr;
    vector<IplImage*> createdImages;

    int size = faces->size();
    for (int i=0 ; i<size ; i++)
    {
        Face* face = &faces->at(i);

        int x1     = face->getX1();
        int y1     = face->getY1();
        int width  = face->getWidth();
        int height = face->getHeight();
        int id     = face->getId();

        if (DEBUG)
            cout << "Id is: " << id << endl;

        // Extract face-image from whole-image.
        CvRect rect            = cvRect(x1,y1,width,height);
        IplImage* faceImg      = LibFaceUtils::copyRect(img, rect);

        // Make into standard-sized image
        IplImage* sizedFaceImg = cvCreateImage(cvSize(d->facesize(), d->facesize()), img->depth, img->nChannels);
        cvResize(faceImg, sizedFaceImg);
        createdImages.push_back(sizedFaceImg);

        face->setFace(sizedFaceImg);
        // Extracted. Now push it into the newfaces vector
        newFaceArr.push_back(*face);

    }
    assignedIDs = d->recognitionCore->update(newFaceArr);

    for (unsigned int i=0; i<createdImages.size(); i++)
        cvReleaseImage(&createdImages[i]);

    return assignedIDs;
}

std::vector<int> LibFace::update(const char* arr, vector<Face>* faces, int width, int height, int step, int depth, int channels, int scaleFactor)
{
    IplImage* img = LibFaceUtils::charToIplImage(arr, width, height, step, depth, channels);
    return this->update(img, faces, scaleFactor);
}

std::vector<int> LibFace::update(const string& filename, vector<Face>* faces, int scaleFactor)
{
    IplImage* img = cvLoadImage(filename.data(), CV_LOAD_IMAGE_GRAYSCALE); //grayscale
    return this->update(img, faces, scaleFactor);
    cvReleaseImage(&img);
}

std::vector<int> LibFace::update(vector<Face>* faces, int /*scaleFactor*/)
{
	std::vector<int> assignedIDs;

    if (faces->size() == 0)
    {
        if (DEBUG)
            cout<<" No faces passed to libface::update() , not updating." << endl;
        return assignedIDs;
    }

    if (DEBUG)
    {
        cout << "Update with faces." << endl;
    }

    vector<Face>      newFaceArr;
    vector<IplImage*> createdImages;

    int size = faces->size();
    for (int i=0; i<size; i++)
    {
        // Copy, dont change the passed face
        Face face  = faces->at(i);

        if (DEBUG)
            cout << "Id is: " << face.getId() << endl;

        const IplImage* faceImg = face.getFace();
        if (faceImg->width != d->facesize() || faceImg->height != d->facesize())
        {
            // Make into standard-sized image
            IplImage* sizedFaceImg  = cvCreateImage(cvSize(d->facesize() , d->facesize()), faceImg->depth, faceImg->nChannels);
            cvResize(faceImg, sizedFaceImg);
            face.setFace(sizedFaceImg);
            createdImages.push_back(sizedFaceImg);
        }
        // Extracted. Now push it into the newfaces vector
        newFaceArr.push_back(face);
    }

    assignedIDs = d->recognitionCore->update(newFaceArr);

    for (unsigned int i=0; i<createdImages.size(); i++)
        cvReleaseImage(&createdImages[i]);

    return assignedIDs;
}

int LibFace::count() const
{
    return d->recognitionCore->count();
}

void LibFace::setDetectionAccuracy(double value)
{
    d->detectionCore->setAccuracy(value);
}

double LibFace::getDetectionAccuracy() const
{
    return d->detectionCore->accuracy();
}

void LibFace::setDetectionSpecificity(double value)
{
    d->detectionCore->setSpecificity(value);
}

double LibFace::getDetectionSpecificity() const
{
    return d->detectionCore->specificity();
}

int LibFace::getRecommendedImageSizeForDetection(const CvSize&) const
{
    return FaceDetect::getRecommendedImageSizeForDetection();
}

CvSize LibFace::getRecommendedImageSizeForRecognition(const CvSize&) const
{
    return cvSize(d->facesize(), d->facesize());
}

} // namespace libface
