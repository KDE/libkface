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

Database::Database(libface::Mode mode, const QString& configurationPath)
{
    d.data()->configPath = configurationPath;
    if(mode == libface::DETECT)
    {
	d.data()->libface = new libface::LibFace(DETECT);
    }
    else
    {
	d.data()->libface = new libface::LibFace(mode, configurationPath.toStdString());
    }
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

void Database::updateFaces(QList< KFace* >& faces)
{
    std::vector<Face> *faceVec;
    Face f;
    KFace kf;
    
    int i;
    for(i = 0; i < faces.size(); ++i)
    {
	kf = *faces.at(i);
	f = kf;	// Will this work properly? TODO: Test this
	faceVec->push_back(f);
    }
    
    std::vector<int> ids;
    ids = d.data()->libface->update(faceVec);
    
    for(i = 0; i <faces.size(); ++i)
    {
	faces[i]->setId(ids.at(i));
    }
    
}

QList<double> Database::recognizeFaces ( QList< KFace* >& faces )
{
    std::vector<Face> *faceVec;
    Face f;
    KFace kf;
    
    int i;
    for(i = 0; i < faces.size(); ++i)
    {
	kf = *faces.at(i);
	f = kf;	// Will this work properly? TODO: Test this
	faceVec->push_back(f);
    }
    
    QList<double> closeness;
    std::vector< std::pair<int, double> > result;
    
    result = d.data()->libface->recognise(faceVec);
    
    for(i = 0; i <faces.size(); ++i)
    {
	faces[i]->setId(result.at(i).first);
	closeness.append(result.at(i).second);
    }
    
    return closeness;
}

};
