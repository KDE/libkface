#include <QApplication>

#include <QImage>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QDebug>
#include <QGraphicsPixmapItem>

#include <libkface/database.h>
#include <libkface/kface.h>

using namespace KFace;

int main(int argc, char **argv) {

    if (argc<2)
    {
        qDebug()<<"Bad Args!!!\nUsage: "<<argv[0] << " <image1> <image2> ...";
        return 0;
    }
    
    QApplication app(argc, argv);

    // Set up a View and Scene
    QGraphicsView *myView = new QGraphicsView();
    QGraphicsScene *myScene = new QGraphicsScene();
    myView->setScene(myScene);
    
    // Load an image from a file and add it to scene
    QPixmap *p = new QPixmap(argv[1]);
    QGraphicsPixmapItem* pItem = new QGraphicsPixmapItem(*p);
    myScene->addItem(pItem);
    
    myView->show();
    
    // Make a new instance of Database and then detect faces from the image
    KFace::Database *d = new KFace::Database(KFace::Database::InitDetection, "");
    QList<Face> result = d->detectFaces(p->toImage());
    

    return app.exec();
}


