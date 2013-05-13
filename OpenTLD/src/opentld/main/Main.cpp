#include "Main.h"

#include "TLDUtil.h"
#include <iostream>
using namespace std;
using namespace tld;
using namespace cv;

void Main::generatefacemodel(IplImage* img, unitFaceModel *modeltostore)
{
    Mat grey(img->height, img->width, CV_8UC1);

    cvtColor(cv::Mat(img), grey,CV_BGR2GRAY);// CV_BGR2GRAY);
    //imshow("ty",im);
    tld->detectorCascade->imgWidth = grey.cols;
    tld->detectorCascade->imgHeight = grey.rows;
    tld->detectorCascade->imgWidthStep = grey.step;

    initialBB = new int[4];
    initialBB[0] = 5;
    initialBB[1] = 5;
    initialBB[2] = img->width-5;
    initialBB[3] = img->height-5;
    Rect bb = tldArrayToRect(initialBB);
    tld->selectObject(grey, &bb);
    cvtColor(cv::Mat(img), grey, CV_BGR2GRAY);
    tld->putObjModel(modeltostore);

}

float Main::getrecognitionconfidence(IplImage* img, unitFaceModel *comparemodel)
{
    Mat grey(img->height, img->width, CV_8UC1);

    cvtColor(cv::Mat(img), grey,CV_BGR2GRAY);// CV_BGR2GRAY);
    //imshow("ty",im);
    tld->detectorCascade->imgWidth = grey.cols;
    tld->detectorCascade->imgHeight = grey.rows;
    tld->detectorCascade->imgWidthStep = grey.step;
    tld->getObjModel(comparemodel);//modelPath);
    cvtColor(cv::Mat(img), grey, CV_BGR2GRAY);
    tld->processImage(img);
    return tld->currConf;
}
