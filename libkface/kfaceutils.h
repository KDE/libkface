#ifndef KFACEUTILS_H
#define KFACEUTILS_H

#include <opencv/cv.h>
#include <QImage>

namespace KFace
{

namespace KFaceUtils
{
    IplImage* QImage2IplImage(const QImage& img);
    QImage IplImage2QImage(const IplImage *img);
};

}

#endif // KFACEUTILS_H
