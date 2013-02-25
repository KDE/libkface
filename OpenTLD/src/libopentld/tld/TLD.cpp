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
 * TLD.cpp
 *
 *  Created on: Nov 17, 2011
 *      Author: Georg Nebehay
 */

#include "TLD.h"

#include <iostream>

#include "NNClassifier.h"
#include "TLDUtil.h"
#include <opencv/highgui.h>

using namespace std;
using namespace cv;

namespace tld
{

TLD::TLD()
{
    trackerEnabled = true;
    detectorEnabled = true;
    learningEnabled = true;
    alternating = false;
    valid = false;
    wasValid = false;
    learning = false;
    currBB = prevBB = NULL;

    detectorCascade = new DetectorCascade();
    nnClassifier = detectorCascade->nnClassifier;

    medianFlowTracker = new MedianFlowTracker();
}

TLD::~TLD()
{
    storeCurrentData();

    delete detectorCascade;
    delete medianFlowTracker;
}

void TLD::release()
{
    detectorCascade->release();
    medianFlowTracker->cleanPreviousData();
    delete currBB;
}

void TLD::storeCurrentData()
{
    prevImg.release();
    prevImg = currImg; //Store old image (if any)
    delete prevBB;
    prevBB = currBB;        //Store old bounding box (if any)

    detectorCascade->cleanPreviousData(); //Reset detector results
    medianFlowTracker->cleanPreviousData();

    wasValid = valid;
}

void TLD::selectObject(const Mat &img, Rect *bb)
{
    //Delete old object
    detectorCascade->release();

    detectorCascade->objWidth = bb->width;
    detectorCascade->objHeight = bb->height;

    //Init detector cascade
    detectorCascade->init();

    currImg = img;
    currBB = tldCopyRect(bb);
    currConf = 1;
    valid = true;

    initialLearning();

}

void TLD::processImage(const Mat &img)
{
    storeCurrentData();
    Mat grey_frame;
    cvtColor(img, grey_frame, CV_RGB2GRAY);
    currImg = grey_frame; // Store new image , right after storeCurrentData();

    if(trackerEnabled)
    {
        medianFlowTracker->track(prevImg, currImg, prevBB);
    }

    if(detectorEnabled && (!alternating || medianFlowTracker->trackerBB == NULL))
    {
        detectorCascade->detect(grey_frame);
    }

    fuseHypotheses();

    learn();

}

void TLD::fuseHypotheses()
{
    Rect *trackerBB = medianFlowTracker->trackerBB;
    int numClusters = detectorCascade->detectionResult->numClusters;
    Rect *detectorBB = detectorCascade->detectionResult->detectorBB;

    currBB = NULL;
    currConf = 0;
    valid = false;

    float confDetector = 0;
    if(numClusters == 1)
    {
        confDetector = nnClassifier->classifyBB(currImg, detectorBB);
    }

    if(trackerBB != NULL)
    {
        float confTracker = nnClassifier->classifyBB(currImg, trackerBB);
        if(numClusters == 1 && confDetector > confTracker && tldOverlapRectRect(*trackerBB, *detectorBB) < 0.5)
        {

            currBB = tldCopyRect(detectorBB);
            currConf = confDetector;
        }
        else
        {
            currBB = tldCopyRect(trackerBB);
            currConf = confTracker;

            if(confTracker > nnClassifier->thetaTP)
            {
                valid = true;
            }
            else if(wasValid && confTracker > nnClassifier->thetaFP)
            {
                valid = true;
            }
        }
    }
    else if(numClusters == 1)
    {
        currBB = tldCopyRect(detectorBB);
        currConf = confDetector;
    }
    /*
    float var = CalculateVariance(patch.values, nn->patch_size*nn->patch_size);

    if(var < min_var) { //TODO: Think about incorporating this
        printf("%f, %f: Variance too low \n", var, classifier->min_var);
        valid = 0;
    }*/
}

void TLD::initialLearning()
{
    learning = true; //This is just for display purposes

    DetectionResult *detectionResult = detectorCascade->detectionResult;

    detectorCascade->detect(currImg);

    //This is the positive patch
    NormalizedPatch patch;
    tldExtractNormalizedPatchRect(currImg, currBB, patch.values);
    patch.positive = 1;

    float initVar = tldCalcVariance(patch.values, TLD_PATCH_SIZE * TLD_PATCH_SIZE);
    detectorCascade->varianceFilter->minVar = initVar / 2;


    float *overlap = new float[detectorCascade->numWindows];
    tldOverlapRect(detectorCascade->windows, detectorCascade->numWindows, currBB, overlap);

    //Add all bounding boxes with high overlap

    vector< pair<int, float> > positiveIndices;
    vector<int> negativeIndices;

    //First: Find overlapping positive and negative patches

    for(int i = 0; i < detectorCascade->numWindows; i++)
    {

        if(overlap[i] > 0.6)
        {
            positiveIndices.push_back(pair<int, float>(i, overlap[i]));
        }

        if(overlap[i] < 0.2)
        {
            float variance = detectionResult->variances[i];

            if(!detectorCascade->varianceFilter->enabled || variance > detectorCascade->varianceFilter->minVar)   //TODO: This check is unnecessary if minVar would be set before calling detect.
            {
                negativeIndices.push_back(i);
            }
        }
    }

    sort(positiveIndices.begin(), positiveIndices.end(), tldSortByOverlapDesc);

    vector<NormalizedPatch> patches;

    patches.push_back(patch); //Add first patch to patch list

    int numIterations = std::min<size_t>(positiveIndices.size(), 10); //Take at most 10 bounding boxes (sorted by overlap)

    for(int i = 0; i < numIterations; i++)
    {
        int idx = positiveIndices.at(i).first;
        //Learn this bounding box
        //TODO: Somewhere here image warping might be possible
        detectorCascade->ensembleClassifier->learn(&detectorCascade->windows[TLD_WINDOW_SIZE * idx], true, &detectionResult->featureVectors[detectorCascade->numTrees * idx]);
    }

    srand(1); //TODO: This is not guaranteed to affect random_shuffle

    random_shuffle(negativeIndices.begin(), negativeIndices.end());

    //Choose 100 random patches for negative examples
    for(size_t i = 0; i < std::min<size_t>(100, negativeIndices.size()); i++)
    {
        int idx = negativeIndices.at(i);

        NormalizedPatch patch;
        tldExtractNormalizedPatchBB(currImg, &detectorCascade->windows[TLD_WINDOW_SIZE * idx], patch.values);
        patch.positive = 0;
        patches.push_back(patch);
    }

    detectorCascade->nnClassifier->learn(patches);

    delete[] overlap;

}

//Do this when current trajectory is valid
void TLD::learn()
{
    if(!learningEnabled || !valid || !detectorEnabled)
    {
        learning = false;
        return;
    }

    learning = true;

    DetectionResult *detectionResult = detectorCascade->detectionResult;

    if(!detectionResult->containsValidData)
    {
        detectorCascade->detect(currImg);
    }

    //This is the positive patch
    NormalizedPatch patch;
    tldExtractNormalizedPatchRect(currImg, currBB, patch.values);

    float *overlap = new float[detectorCascade->numWindows];
    tldOverlapRect(detectorCascade->windows, detectorCascade->numWindows, currBB, overlap);

    //Add all bounding boxes with high overlap

    vector<pair<int, float> > positiveIndices;
    vector<int> negativeIndices;
    vector<int> negativeIndicesForNN;

    //First: Find overlapping positive and negative patches

    for(int i = 0; i < detectorCascade->numWindows; i++)
    {

        if(overlap[i] > 0.6)
        {
            positiveIndices.push_back(pair<int, float>(i, overlap[i]));
        }

        if(overlap[i] < 0.2)
        {
            if(!detectorCascade->ensembleClassifier->enabled || detectionResult->posteriors[i] > 0.1)   //TODO: Shouldn't this read as 0.5?
            {
                negativeIndices.push_back(i);
            }

            if(!detectorCascade->ensembleClassifier->enabled || detectionResult->posteriors[i] > 0.5)
            {
                negativeIndicesForNN.push_back(i);
            }

        }
    }

    sort(positiveIndices.begin(), positiveIndices.end(), tldSortByOverlapDesc);

    vector<NormalizedPatch> patches;

    patch.positive = 1;
    patches.push_back(patch);
    //TODO: Flip


    int numIterations = std::min<size_t>(positiveIndices.size(), 10); //Take at most 10 bounding boxes (sorted by overlap)

    for(size_t i = 0; i < negativeIndices.size(); i++)
    {
        int idx = negativeIndices.at(i);
        //TODO: Somewhere here image warping might be possible
        detectorCascade->ensembleClassifier->learn(&detectorCascade->windows[TLD_WINDOW_SIZE * idx], false, &detectionResult->featureVectors[detectorCascade->numTrees * idx]);
    }

    //TODO: Randomization might be a good idea
    for(int i = 0; i < numIterations; i++)
    {
        int idx = positiveIndices.at(i).first;
        //TODO: Somewhere here image warping might be possible
        detectorCascade->ensembleClassifier->learn(&detectorCascade->windows[TLD_WINDOW_SIZE * idx], true, &detectionResult->featureVectors[detectorCascade->numTrees * idx]);
    }

    for(size_t i = 0; i < negativeIndicesForNN.size(); i++)
    {
        int idx = negativeIndicesForNN.at(i);

        NormalizedPatch patch;
        tldExtractNormalizedPatchBB(currImg, &detectorCascade->windows[TLD_WINDOW_SIZE * idx], patch.values);
        patch.positive = 0;
        patches.push_back(patch);
    }

    detectorCascade->nnClassifier->learn(patches);

    //cout << "NN has now " << detectorCascade->nnClassifier->truePositives->size() << " positives and " << detectorCascade->nnClassifier->falsePositives->size() << " negatives.\n";

    delete[] overlap;
}

typedef struct
{
    int index;
    int P;
    int N;
} TldExportEntry;

void TLD::getObjModel(unitFaceModel *faceModel)
{
    NNClassifier *nn = detectorCascade->nnClassifier;
    EnsembleClassifier *ec = detectorCascade->ensembleClassifier;

    detectorCascade->objWidth = faceModel->objWidth;
    detectorCascade->objHeight = faceModel->objWidth;
    detectorCascade->varianceFilter->minVar = faceModel->minVar;
    QList<QList<float> > allPositivePatches = faceModel->deserialisePositivePatches();
    QList<QList<float> > allNegativePatches = faceModel->deserialiseNegativePatches();

    for (int i=0 ;i < allPositivePatches.size() ;i++)
    {
        NormalizedPatch patch;
        for(int j=0;j < allPositivePatches.at(i).size();j++)
        {
            patch.values[j] = allPositivePatches.at(i)[j];
        }
        nn->truePositives->push_back(patch);
    }
    for (int i = 0 ; i < allNegativePatches.size() ; i++)
    {
        NormalizedPatch patch;
        for(int j=0;j < allNegativePatches.at(i).size();j++)
        {
            patch.values[j] = allNegativePatches.at(i)[j];
        }
        nn->falsePositives->push_back(patch);
    }
    QList<QList<QList<float> > > allFeatures = faceModel->deserialiseFeatures();
    QList<QList<QList<int> > > allLeaves = faceModel->deserialiseLeaves();
    ec->numFeatures = allFeatures.at(0).size();
    ec->numTrees = allFeatures.size();
    detectorCascade->numFeatures = ec->numFeatures;
    detectorCascade->numTrees = ec->numTrees;
    int size = 2 * 2 * ec->numFeatures * ec->numTrees;
    ec->features = new float[size];
    ec->numIndices = pow(2.0f, ec->numFeatures);
    ec->initPosteriors();
    for (int i=0 ;i < allFeatures.size() ;i++ )
    {
        for (int j=0 ; j < allFeatures.at(0).size() ; j++)
        {
            float *features = ec->features + 4 * ec->numFeatures * i + 4 * j;
            features[0] = allFeatures.at(i).at(j)[0];features[1] = allFeatures.at(i).at(j)[1];features[2] = allFeatures.at(i).at(j)[2];
            features[3] = allFeatures.at(i).at(j)[3];
        }
        for (int j=0 ; j < allLeaves.at(i).size() ; j++ )
        {
            int posarray[3],negarray[3];posarray[0] = allLeaves.at(i).at(j)[0];posarray[1] = 1;posarray[2] = allLeaves.at(i).at(j)[1];
            negarray[0] = allLeaves.at(i).at(j)[0];negarray[1] = 0;negarray[2] = allLeaves.at(i).at(j)[2];
            ec->callupdatePosterior(i,posarray);
            ec->callupdatePosterior(i,negarray);
        }
    }
    detectorCascade->initWindowsAndScales();
    detectorCascade->initWindowOffsets();

    detectorCascade->propagateMembers();

    detectorCascade->initialised = true;

    ec->initFeatureOffsets();
}

unitFaceModel *TLD::putObjModel()
{
    NNClassifier *nn = detectorCascade->nnClassifier;
    EnsembleClassifier *ec = detectorCascade->ensembleClassifier;

    unitFaceModel *faceModel = new unitFaceModel;
    faceModel->objWidth = detectorCascade->objWidth;
    faceModel->objHeight = detectorCascade->objHeight;
    faceModel->minVar = detectorCascade->varianceFilter->minVar;

    QList<QList<float> > allPositivePatches;
    for(size_t s = 0; s < nn->truePositives->size(); s++)
    {
        QList<float> unitPositivePatch;
        for (int i = 0; i < 225;i++)
        {
            unitPositivePatch.append(nn->truePositives->at(s).values[i]);
        }
        allPositivePatches.append(unitPositivePatch);
    }
    faceModel->serialisePositivePatches(allPositivePatches);
    QList<QList<float> > allNegativePatches;
    for(size_t s = 0; s < nn->falsePositives->size(); s++)
    {
        QList<float> unitNegativePatch;
        for (int i = 0; i < 225 ;i++)
        {
            unitNegativePatch.append(nn->falsePositives->at(s).values[i]);
        }
        allNegativePatches.append(unitNegativePatch);
    }
    faceModel->serialiseNegativePatches(allNegativePatches);

    detectorCascade->numTrees = ec->numTrees;
    detectorCascade->numFeatures = ec->numFeatures;

    QList<QList<QList<float> > > allFeatures;
    QList<QList<QList<int> > > allLeaves;

    for(int i = 0; i < ec->numTrees; i++)
    {
        QList<QList<float> > unitFeature;
        for(int j = 0; j < ec->numFeatures; j++)
        {
            QList<float> subunitFeature;
            float *features = ec->features + 4 * ec->numFeatures * i + 4 * j;
            for (int i=0;i<4;i++)
            {
                subunitFeature.append(features[i]);
            }// cout << features[0] << "\t"<< features[1] << "\t"<< features[2] << "\t"<< features[3] << "\t"<<endl;
            unitFeature.append(subunitFeature);
        }
        allFeatures.append(unitFeature);

        vector<TldExportEntry> list;

        for(int index = 0; index < pow(2.0f, ec->numFeatures); index++)
        {
            int p = ec->positives[i * ec->numIndices + index];

            if(p != 0)
            {
                TldExportEntry entry;
                entry.index = index;
                entry.P = p;
                entry.N = ec->negatives[i * ec->numIndices + index];
                list.push_back(entry);
            }
        }

        QList<QList<int>  > unitLeave;
        for(size_t j = 0; j < list.size(); j++)
        {
            QList<int> subunitLeave;
            TldExportEntry entry = list.at(j);
            subunitLeave.append(entry.index);
            subunitLeave.append(entry.P);
            subunitLeave.append(entry.N);
            unitLeave.append(subunitLeave);
        }
        allLeaves.append(unitLeave);
    }
    faceModel->serialiseFeatures(allFeatures);
    faceModel->serialiseLeaves(allLeaves);
    return faceModel;
}

} /* namespace tld */
