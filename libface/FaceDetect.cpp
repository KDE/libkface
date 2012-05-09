/** ===========================================================
 * @file
 *
 * This file is a part of libface project
 * <a href="http://libface.sourceforge.net">http://libface.sourceforge.net</a>
 *
 * @date    2010-01-03
 * @brief   Class to perform faces detection.
 * @section DESCRIPTION
 *
 * @author Copyright (C) 2010 by Alex Jironkin
 *         <a href="alexjironkin at gmail dot com">alexjironkin at gmail dot com</a>
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

#include <algorithm>
#include <iostream>
#include <cmath>
#include <ctime>

#include "libopencv.h"
#include "LibFaceUtils.h"
#include "Haarcascades.h"
#include "FaceDetect.h"
#include "Face.h"

using namespace std;

namespace libface
{

class DetectObjectParameters
{
public:

    DetectObjectParameters()
    {
        searchIncrement = 0;
        grouping        = 0;
        flags           = 0;
        minSize         = cvSize(0,0);
    }

    double        searchIncrement;
    int           grouping;
    int           flags;
    CvSize        minSize;
};

class CascadeProperties
{
public:

    CascadeProperties()
    {
        primaryCascade   = false;
        verifyingCascade = true;

        windowSize = cvSize(0,0);

        xROI      = 0;
        yROI      = 0;
        widthROI  = 1;
        heightROI = 1;
    }

    bool primaryCascade;
    bool verifyingCascade;

    /// The size on which the cascade was trained, read from the XML file
    CvSize windowSize;

    /** Facial features have a region of interest, e.g., the left eye is typically
     *  located in the left upper region of the presumed face.
     *  For frontal face cascades, this is 0,0 - 1x1. */
    double xROI;
    double yROI;
    double widthROI;
    double heightROI;

    /** Assumptions on the relation of the size of a facial feature to the whole face.
     *  Basically, we say the size is between 1/10 and 1/4, approx 1/6 */
    static double faceToFeatureRelationMin() { return 10; }
    static double faceToFeatureRelationMax() { return 4; }
    static double faceToFeatureRelationPresumed() { return 6; }

    /** A primary cascade does the initial scan on the whole image area
     *  A verifying cascade scans the area reported by the primary cascade */
    void setPrimaryCascade(bool isPrimary = true)
    {
        primaryCascade = isPrimary;
        verifyingCascade = !isPrimary;
    }

    void setROI(double x, double y, double width, double height)
    {
        xROI      = x;
        yROI      = y;
        widthROI  = width;
        heightROI = height;
    }

    bool isFacialFeature() const
    {
        return xROI != 0 || yROI != 0 || widthROI != 1 || heightROI != 1;
    }

    /** given the full face rect (relative to whole image), returns the rectangle
     *  of the region of interest of this cascade (still relative to whole image).
     *  For frontal face cascades, returns the given parameter unchanged. */
    CvRect faceROI(const CvRect& faceRect) const
    {
        return cvRect(lround(faceRect.x + xROI      * faceRect.width),
                      lround(faceRect.y + yROI      * faceRect.height),
                      lround(             widthROI  * faceRect.width),
                      lround(             heightROI * faceRect.height));
    }

    /**
     * Verifying cascades: Returns the minSize parameter for cvHaarDetectObjects.
     * For frontal faces, starts the scan in the same order of magnitude as the presumed face,
     * slightly smaller.
     * For facial features, which are smaller than a face, uses the faceToFeatureRelation
     * assumptions made above. Often may end using the minimum.
     */
    CvSize minSizeForFace(const CvSize& faceSize) const
    {
        CvSize minSize;

        if (!isFacialFeature())
        {
            // Start with a size slightly smaller than the presumed face
            minSize = cvSize(lround(double(faceSize.width) * 0.6),
                             lround(double(faceSize.height) * 0.6));
        }
        else
        {
            // for a feature, which is smaller than the face, start with a significantly smaller min size
            minSize = cvSize(lround(double(faceSize.width) / faceToFeatureRelationMin()),
                             lround(double(faceSize.height) / faceToFeatureRelationMin()));
        }

        if (lessThanWindowSize(minSize))
            return cvSize(0,0);
        return minSize;
    }

    /**
     * For facial features:
     * For the case that a feature ROI is small and shall be scaled up.
     * Give the real face size.
     * Returns a scale factor (>1) by which the face, or rather only the ROI,
     * should be scaled up to fit with the windowSize of this cascade.
     */
    double requestedInputScaleFactor(const CvSize& faceSize) const
    {
        if (!isFacialFeature())
            return 1.0;

        if (faceSize.width / faceToFeatureRelationMin() >= windowSize.width
            && faceSize.height / faceToFeatureRelationMin() >= windowSize.height)
            return 1.0;

        return max(double(windowSize.width) * faceToFeatureRelationPresumed() / faceSize.width,
                   double(windowSize.height) * faceToFeatureRelationPresumed() / faceSize.height);
    }

    bool lessThanWindowSize(const CvSize& size) const
    {
        return size.width < windowSize.width || size.height < windowSize.height;
    }
};

class FaceDetect::FaceDetectPriv
{

public:

    FaceDetectPriv()
    {
        cascadeSet               = 0;
        storage                  = 0;
        scaleFactor              = 1.0;

        maxDistance              = 0;
        minDuplicates            = 0;

        speedVsAccuracy          = 0.8;
        sensitivityVsSpecificity = 0.8;
    }

    Haarcascades* cascadeSet;
    CvMemStorage* storage;
    double        scaleFactor;    // Keeps the scaling factor of the internal image.

    int           maxDistance;    // Maximum distance between two faces to call them unique
    int           minDuplicates;  // Minimum number of duplicates required to qualify as a genuine face

    vector<CascadeProperties> cascadeProperties;
    // Tunable values, for accuracy
    DetectObjectParameters primaryParams;
    DetectObjectParameters verifyingParams;

    double        speedVsAccuracy;
    double        sensitivityVsSpecificity;
};

FaceDetect::FaceDetect(const string& cascadeDir)
          : d(new FaceDetectPriv)
{
    d->cascadeSet = new Haarcascades(cascadeDir);

    /* Cascades */

    d->cascadeSet->addCascade("haarcascade_frontalface_alt.xml", 1);
    d->cascadeSet->addCascade("haarcascade_frontalface_default.xml",1);
    d->cascadeSet->addCascade("haarcascade_frontalface_alt2.xml",1);
    d->cascadeSet->addCascade("haarcascade_frontalface_alt_tree.xml", 1);

    d->cascadeSet->addCascade("haarcascade_profileface.xml", 1);

    d->cascadeSet->addCascade("haarcascade_mcs_lefteye.xml", 1);
    d->cascadeSet->addCascade("haarcascade_mcs_righteye.xml", 1);
    d->cascadeSet->addCascade("haarcascade_mcs_nose.xml", 1);
    d->cascadeSet->addCascade("haarcascade_mcs_mouth.xml", 1);

    d->cascadeProperties = vector<CascadeProperties>(d->cascadeSet->getSize());
    for (int i=0; i<d->cascadeSet->getSize(); i++)
        if (d->cascadeSet->getCascade(i).haarcasc)
            d->cascadeProperties[i].windowSize = d->cascadeSet->getCascade(i).haarcasc->orig_window_size;

    d->cascadeProperties[2].setPrimaryCascade();

    d->cascadeProperties[5].setROI(0, 0, 0.6, 0.6);
    d->cascadeProperties[6].setROI(0.4, 0, 0.6, 0.6);
    d->cascadeProperties[7].setROI(0.2, 0.25, 0.6, 0.6);
    d->cascadeProperties[8].setROI(0.1, 0.4, 0.8, 0.6);

    /**
     * Modesto Castrillón, Oscar Déniz, Daniel Hernández, Javier Lorenzo
     * A comparison of face and facial feature detectors based
     * on the Viola–Jones general object detection framework
     * Machine Vision and Applications, 01/2008
     * DOI 10.1007/s00138-010-0250-7
     */
}

FaceDetect::~FaceDetect()
{
    cvReleaseMemStorage(&d->storage);
    d->cascadeSet->clear();
    delete d->cascadeSet;
    delete d;
}

double FaceDetect::accuracy() const
{
    return d->speedVsAccuracy;
}

double FaceDetect::specificity() const
{
    return d->sensitivityVsSpecificity;
}

void FaceDetect::setAccuracy(double speedVsAccuracy)
{
    d->speedVsAccuracy        = max(0.0, min(1.0, speedVsAccuracy));
}

void FaceDetect::setSpecificity(double sensitivityVsSpecificity)
{
    d->sensitivityVsSpecificity = max(0.0, min(1.0, sensitivityVsSpecificity));
}

void FaceDetect::updateParameters(const CvSize& /*scaledSize*/, const CvSize& originalSize)
{
    double origSize = double(max(originalSize.width, originalSize.height)) / 1000;

    /* Search increment will determine the number of passes over the image.
     * But with fewer passes, we will miss some faces.
     */
    if (d->speedVsAccuracy <= 0.159)
        d->primaryParams.searchIncrement = 1.5;
    else if (d->speedVsAccuracy >= 0.8)
        d->primaryParams.searchIncrement = 1.1;
    else
        d->primaryParams.searchIncrement = round(100 * (1.1 - 0.5*log10(d->speedVsAccuracy))) / 100;

    /* This is a clear tradeoff. With 1, we'll get many faces,
     * but more false positives than faces.
     * 3 is the best parameter for normal use. */
    if (d->sensitivityVsSpecificity < 0.25)
        d->primaryParams.grouping = 1;
    else if (d->sensitivityVsSpecificity < 0.5)
        d->primaryParams.grouping = 2;
    else
        d->primaryParams.grouping = 3;

    // flag speeds up (very much faster) and potentially lowers sensitivity: We mostly use it,
    // unless in we want very high sensitivity at low speed
    if (d->sensitivityVsSpecificity > 0.1 || d->speedVsAccuracy < 0.9)
        d->primaryParams.flags = CV_HAAR_DO_CANNY_PRUNING;
    else
        d->primaryParams.flags = 0;

    /* greater min size will filter small images, lowering sensitivity, enhancing specificity,
     * with false positives often small */
    double minSize = 32 * d->sensitivityVsSpecificity;
    /* Original small images deserve a smaller minimum size. */
    minSize -= 10 * (1.0 - min(1.0, origSize));
    /* A small min size means small starting size, together with search increment, determining
     * the number of operations and thus speed */
    if (d->speedVsAccuracy < 0.75)
        minSize += 100 * (0.75 - d->speedVsAccuracy);
    // Cascade minimum is 20 for most of our cascades (one is 24). Passing 0 will use the cascade minimum.
    if (minSize < 20)
        minSize = 0;
    d->primaryParams.minSize = cvSize(lround(minSize), lround(minSize));

    d->maxDistance   = 15;    // Maximum distance between two faces to call them unique
    d->minDuplicates = 0;

    d->verifyingParams.searchIncrement = 1.1;
    d->verifyingParams.flags           = 0;
    // min size is adjusted each time

    if (DEBUG)
    {
        cout << "updateParameters: accuracy " << d->speedVsAccuracy
             << " sensitivity " << d->sensitivityVsSpecificity
             << " - searchIncrement " << d->primaryParams.searchIncrement
             << " grouping " << d->primaryParams.grouping
             << " flags " << d->primaryParams.flags
             << " min size " << d->primaryParams.minSize.width << endl
             << " primary cascades: ";

        for (unsigned int i=0; i<d->cascadeProperties.size(); i++)
            if (d->cascadeProperties[i].primaryCascade)
                cout << d->cascadeSet->getCascade(i).name << " ";

        cout << endl
             << " maxDistance " << d->maxDistance
             << " minDuplicates " << d->minDuplicates << endl;
    }

    /*if (d->speedVsAccuracy < 0.5)
    {
        d->primaryCascades[0] = true;
        d->minDuplicates = 0;
    }
    else
    {
        d->primaryCascades[1] = true;
        d->primaryCascades[2] = true;
        if (d->sensitivityVsSpecificity > 0.5)
            d->minDuplicates = 1;
    }*/

    /*
    d->searchIncrement = lround(10 * (0.04 * mpx + 1.02)) / 10;
    switch(d->accu)
    {
        case 1:
        {
            d->searchIncrement = 1.21F;
            d->minSize[0]      = 1;
            d->minSize[1]      = 30;
            d->minSize[2]      = 40;
            d->minSize[3]      = 50;
            d->grouping        = 2;
            break;
        }

        case 2:
        {
            d->searchIncrement = 1.2F;
            d->minSize[0]      = 1;
            d->minSize[1]      = 20;
            d->minSize[2]      = 30;
            d->minSize[3]      = 40;
            d->grouping        = 3;
            break;
        }
        case 3:
        {
            d->searchIncrement = 1.21F;
            d->minSize[0]      = 1;
            d->minSize[1]      = 20;
            d->minSize[2]      = 26;
            d->minSize[3]      = 35;
            d->grouping        = 3;
            break;
        }
    };
    */
}

vector<Face> FaceDetect::cascadeResult(const IplImage* inputImage, CvHaarClassifierCascade* casc,
                                       const DetectObjectParameters &params)
{
    // Clear the memory d->storage which was used before
    cvClearMemStorage(d->storage);

    vector<Face> result;

    CvSeq* faces = 0;

    // Create two points to represent the face locations
    CvPoint pt1, pt2;

    // Check whether the cascade has loaded successfully. Else report and error and quit
    if (!casc)
    {
        cerr << "ERROR: Could not load classifier cascade." << endl;
        return result;
    }

    // There can be more than one face in an image. So create a growable sequence of faces.
    // Detect the objects and store them in the sequence

    /*cout << "cvHaarDetectObjects: image size " << inputImage->width << " " << inputImage->height
            << " searchIncrement " << params.searchIncrement
            << " grouping " << params.grouping
            << " flags " << params.flags
            << " min size " << params.minSize.width << " " << params.minSize.height << endl;*/

    faces = cvHaarDetectObjects(inputImage,
            casc,
            d->storage,
            params.searchIncrement,                // Increase search scale by 5% everytime
            params.grouping,                       // Drop groups of less than n detections
            params.flags,                          // Optionally, pre-test regions by edge detection
            params.minSize                         // Minimum face size to look for
    );

    // Loop the number of faces found.
    for (int i = 0; i < (faces ? faces->total : 0); i++)
    {
        // Create a new rectangle for drawing the face

        CvRect* r = (CvRect*) cvGetSeqElem(faces, i);

        // Find the dimensions of the face

        pt1.x     = r->x;
        pt2.x     = r->x + r->width;
        pt1.y     = r->y;
        pt2.y     = r->y + r->height;

        Face face = Face(pt1.x,pt1.y,pt2.x,pt2.y);

        result.push_back(face);
    }

    cvClearSeq(faces);

    //Please don't delete next line even if commented out. It helps with testing intermediate results.
    //LibFaceUtils::showImage(inputImage, result);

    return result;
}

bool FaceDetect::verifyFace(const IplImage* inputImage, const Face &face)
{
    // check if we need to verify
    int verifyingCascades = 0;
    for (unsigned int i = 0; i < d->cascadeProperties.size(); ++i)
        if (d->cascadeProperties[i].verifyingCascade)
            verifyingCascades++;

    if (!verifyingCascades)
        return true;

    clock_t detect;
    if (DEBUG)
        detect = clock();

    // Face coordinates. Add a certain margin for the other frontal cascades.
    const CvRect faceRect = cvRect(face.getX1(), face.getY1(), face.getWidth(), face.getHeight());
    const CvSize faceSize = cvSize(face.getWidth(), face.getHeight());
    const int margin      = min(40, max(faceRect.width, faceRect.height));

    // Clip to bounds of image, after adding the margin
    CvRect extendedRect   = cvRect( max(0, faceRect.x - margin),
                                    max(0, faceRect.y - margin),
                                    faceRect.width + 2*margin,
                                    faceRect.height + 2*margin );
    extendedRect.width  = min(inputImage->width - extendedRect.x, extendedRect.width);
    extendedRect.height = min(inputImage->height - extendedRect.y, extendedRect.height);

    if (DEBUG)
    {
        cout << "\nVerifying face (" << face.getX1() << "," << face.getY1() << " "
             << face.getWidth() << "x" << face.getHeight() << ")" << endl;
        cout << "extended rect " << extendedRect.x << " " << extendedRect.y
             << " " << extendedRect.width << "x" << extendedRect.height << endl;
    }

    IplImage* extendedFaceImg = LibFaceUtils::copyRect(inputImage, extendedRect);
    vector<Face> foundFaces;
    //LibFaceUtils::showImage(extendedFaceImg, foundFaces);

    int frontalFaceVotes = 0, facialFeatureVotes = 0;
    for (int i = 0; i < d->cascadeSet->getSize(); ++i)
    {
        if (d->cascadeProperties[i].verifyingCascade)
        {
            d->verifyingParams.minSize = d->cascadeProperties[i].minSizeForFace(faceSize);

            if (d->cascadeProperties[i].isFacialFeature())
            {
                d->verifyingParams.grouping = 2;

                CvRect roi = d->cascadeProperties[i].faceROI(faceRect);
                IplImage* feature = LibFaceUtils::copyRect(inputImage, roi);
                foundFaces = cascadeResult(feature, d->cascadeSet->getCascade(i).haarcasc, d->verifyingParams);
                if (foundFaces.size())
                    facialFeatureVotes++;

                /*
                 * This is pretty much working code that scales up the face if it's too small
                 * for the  facial feature cascade. It did not bring me benefit with false positives though.
                double factor = d->cascadeProperties[i].requestedInputScaleFactor(faceSize);
                IplImage* feature = LibFaceUtils::scaledSection(inputImage, roi, factor);

                / *cout << "Facial feature in " << roi.x << " " << roi.y << " " << roi.width << "x" << roi.height
                     << " scaled up to " << feature->width << " " << feature->height << endl;* /
                foundFaces = cascadeResult(feature, d->cascadeSet->getCascade(i).haarcasc, d->verifyingParams);

                for (vector<Face>::iterator it = foundFaces.begin(); it != foundFaces.end(); ++it)
                {
                    cout << "Feature face " << it->getX1() << " " << it->getY1() << " " << it->getWidth() << "x" << it->getHeight() << endl;
                    double widthScaled = it->getWidth() / factor;
                    double heightScaled = it->getHeight() / factor;

                    / *cout << "Hit feature size " << widthScaled << " " << heightScaled << " "
                         << (faceSize.width / CascadeProperties::faceToFeatureRelationMin()) << " "
                         << (faceSize.width / CascadeProperties::faceToFeatureRelationMax()) << endl;* /
                    if (
                        (widthScaled > faceSize.width / CascadeProperties::faceToFeatureRelationMin()
                         && widthScaled < faceSize.width / CascadeProperties::faceToFeatureRelationMax())
                        ||
                        (heightScaled > faceSize.height / CascadeProperties::faceToFeatureRelationMin()
                         && heightScaled < faceSize.height / CascadeProperties::faceToFeatureRelationMax())
                        )
                    {
                        facialFeatureVotes++;
                        cout << "voting" << endl;
                        break;
                    }
                }
                */

                cvReleaseImage(&feature);
            }
            else
            {
                d->verifyingParams.grouping = 3;

                foundFaces = cascadeResult(extendedFaceImg, d->cascadeSet->getCascade(i).haarcasc, d->verifyingParams);

                // We dont need to check the size of found regions, the minSize in verifyingParams is large enough
                if (foundFaces.size())
                    frontalFaceVotes++;
            }

            if (DEBUG)
                cout << "Verifying cascade " << d->cascadeSet->getCascade(i).name << " gives "
                     << foundFaces.size() << endl;
        }
    }

    cvReleaseImage(&extendedFaceImg);

    bool verified;
    // Heuristic: Discard a sufficiently large face that shows no facial features
    if (faceSize.width <= 50 && facialFeatureVotes == 0)
        verified = false;
    else
    {
        if (frontalFaceVotes && facialFeatureVotes)
            verified = true;
        else if (frontalFaceVotes >= 2)
            verified = true;
        else if (facialFeatureVotes >= 2)
            verified = true;
        else
            verified = false;
    }

    if (DEBUG)
    {
        detect = clock() - detect;
        cout << "Verification finished in " << ((double)detect / ((double)CLOCKS_PER_SEC))
             << ". Votes: Frontal " << frontalFaceVotes << " " << " Features " << facialFeatureVotes
             << ". Face verified: " << verified << endl;
    }
    return verified;
}

vector<Face> FaceDetect::mergeFaces(const IplImage* /*inputImage*/, vector<vector<Face> > combo, int maxdist, int mindups)
{
    clock_t      finalStage;
    vector<Face> finalResult;

    if (DEBUG)
        finalStage = clock();

    // Make one long vector of all faces
    for (unsigned int i = 0; i < combo.size(); ++i)
        finalResult.insert(finalResult.end(), combo[i].begin(), combo[i].end());

    // used only one cascade? No need to merge then
    int primaryCascades = 0;
    for (unsigned int i = 0; i < d->cascadeProperties.size(); ++i)
        if (d->cascadeProperties[i].primaryCascade)
            primaryCascades++;

    if (primaryCascades > 1)
    {
        if (DEBUG)
            finalStage = clock() - finalStage;

        /*
        Now, starting from the left, take a face and compare with rest. If distance is less than a threshold,
        consider them to be "overlapping" face frames and delete the "duplicate" from the vector.
        Remember that only faces to the RIGHT of the reference face will be deleted.
        */
        vector<int> genuineness;
        if (DEBUG)
            finalStage = clock();

        int ctr = 0;
        for (unsigned int i = 0; i < finalResult.size(); ++i)
        {
            int duplicates = 0;
            for (unsigned int j = i + 1; j < finalResult.size(); ++j)    // Compare with the faces to the right
            {
                ctr++;
                if (LibFaceUtils::distance(finalResult[i], finalResult[j]) < maxdist)
                {
                    finalResult.erase(finalResult.begin() + j);
                    duplicates++;
                    j--;

                }
            }
            genuineness.push_back(duplicates);
            if (duplicates < mindups)    // Less duplicates, probably not genuine, kick it out
            {
                genuineness.erase(genuineness.begin() + i);
                finalResult.erase(finalResult.begin() + i);
                i--;
            }
        }

        /*
        Note that the index of the reference element will be the same as the index of it's number of duplicates
        in the genuineness vector, so win-win!.
        */

        if (DEBUG)
        {
            printf("Faces parsed : %d, number of final faces : %d\n", ctr, (int)genuineness.size());
            finalStage = clock() - finalStage;
            printf("Pruning took: %f sec.\n", (double)finalStage / ((double)CLOCKS_PER_SEC));
        }
    }

    return finalResult;
}

int FaceDetect::getRecommendedImageSizeForDetection()
{
    return 800;
}

vector<Face> FaceDetect::detectFaces(const IplImage* inputImage, const CvSize& size)
{
    if (inputImage->imageData == 0)
    {
        cout<<"Bad image given, not detecting faces."<<endl;
        return vector<Face>();
    }

    CvSize originalSize = size;
    if (!size.width && !size.height)
        originalSize = cvSize(inputImage->width, inputImage->height);

    clock_t init, detect, duration;

    if (DEBUG)
        init = clock();

    IplImage* scaled = 0;
    d->scaleFactor   = 1;

    int inputArea  = inputImage->width*inputImage->height;

    if (DEBUG)
        printf("Input area : %d\n", inputArea);

    if (inputArea > getRecommendedImageSizeForDetection() * getRecommendedImageSizeForDetection())
    {
        if (DEBUG)
            cout << "downscaling input image" << endl;
        scaled = libface::LibFaceUtils::resizeToArea(inputImage, 786432, d->scaleFactor);
    }

    const IplImage* image = scaled ? scaled : inputImage;

    updateParameters(cvSize(image->width, image->height), originalSize);

    // Now loop through each cascade, apply it, and get back a vector of detected faces
    vector<vector<Face> > primaryResults(d->cascadeSet->getSize());
    vector<Face> finalResult;

    d->storage = cvCreateMemStorage(0);

    for (int i = 0; i < d->cascadeSet->getSize(); ++i)
    {
        if (d->cascadeProperties[i].primaryCascade)
        {
            if (DEBUG)
                detect = clock();

            primaryResults[i] = cascadeResult(image, d->cascadeSet->getCascade(i).haarcasc, d->primaryParams);

            if (DEBUG)
            {
                duration = clock() - detect;
                cout <<"Primary detection with " <<  d->cascadeSet->getCascade(i).name << ": "
                     << ((double)duration / ((double)CLOCKS_PER_SEC)) << " sec" << endl;
            }
        }
    }

    // After intelligently "merging" overlaps of face regions by different cascades,
    // this creates a list of faces.
    finalResult = mergeFaces(image, primaryResults, d->maxDistance, d->minDuplicates);
    //LibFaceUtils::showImage(image, finalResult);

    // Verify faces using other cascades
    for (vector<Face>::iterator it = finalResult.begin(); it != finalResult.end(); )
    {
        if (!verifyFace(image, *it))
            it = finalResult.erase(it);
        else
            ++it;
    }

    cvReleaseMemStorage(&d->storage);
    if (scaled)
        cvReleaseImage(&scaled);

    // Insert extracted images into face
    for (vector<Face>::iterator it = finalResult.begin(); it != finalResult.end(); ++it)
    {
        // rescale to full image
        if (d->scaleFactor != 1)
        {
            it->setX1(lround(it->getX1() * d->scaleFactor));
            it->setY1(lround(it->getY1() * d->scaleFactor));
            it->setX2(lround(it->getX2() * d->scaleFactor));
            it->setY2(lround(it->getY2() * d->scaleFactor));
        }

        //Extract face-image from whole-image.
        CvRect rect       = cvRect(it->getX1(), it->getY1(), it->getWidth(), it->getHeight());
        IplImage* faceImg = LibFaceUtils::copyRect(inputImage, rect);
        it->setFace(faceImg);
    }

    if (DEBUG)
    {
        duration = clock() - init;
        cout << "Total time : " << ((double)duration / ((double)CLOCKS_PER_SEC))<< " sec" << endl;
    }

    return finalResult;
}

vector<Face> FaceDetect::detectFaces(const string& filename)
{
    // Create a new image based on the input image
    IplImage* img = cvLoadImage(filename.data(), CV_LOAD_IMAGE_GRAYSCALE);

    vector<Face> faces = detectFaces(img);

    cvReleaseImage(&img);

    return faces;
}

} // namespace libface
