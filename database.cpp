#include "database.h"

#include <QMutex>
#include <QMutexLocker>
#include <QSharedData>

#include "kfaceutils.h"

using namespace libface;

namespace kface
{

class DatabaseStatic
{
public:

    DatabaseStatic()
    {
    }

    QMutex  mutex;
    //QHash<QString, Database> hash;
};

class DatabasePriv : public QSharedData
{
public:

    DatabasePriv()
    {
        libface = 0;
    }

    LibFace *libface;
    QString  configPath;
};

Database::Database()
{
}

Database::Database(const Database& other)
{
    
}

Database::~Database()
{

}

QList<KFace *> Database::detectFaces(const QImage& image)
{
    IplImage *img = KFaceUtils::QImage2IplImage(&image);
    std::vector<Face> result = d.data()->libface->detectFaces(img->imageData,img->width, img->height, img->widthStep, img->depth, img->nChannels);
    
    QList<KFace*> faceList;
    Face f;
    KFace kf;
    foreach(f, result)
    {
	kf = f;
	faceList.append(&kf);
    }
    return faceList;
	
}

};
