#ifndef KFACEUTILS_H
#define KFACEUTILS_H

#include <opencv/cv.h>
#include <QImage>

class KFaceUtils
{
public:
    KFaceUtils();
    static IplImage* QImage2IplImage(const QImage *img);
    static QImage* IplImage2QImage(const IplImage *img);

};

#endif // KFACEUTILS_H
