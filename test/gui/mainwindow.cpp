#include "mainwindow.moc"
#include "ui_mainwindow.h"

#include <QDebug>

using namespace KFace;

MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent),
        ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->connect(ui->openImageBtn, SIGNAL(clicked()), this, SLOT(openImage()));
    this->connect(ui->openConfigBtn, SIGNAL(clicked()), this, SLOT(openConfig()));
    this->connect(ui->detectFacesBtn, SIGNAL(clicked()), this, SLOT(detectFaces()));
    this->connect(ui->recogniseBtn, SIGNAL(clicked()), this, SLOT(recognise()));
    this->connect(ui->updateDatabaseBtn, SIGNAL(clicked()), this, SLOT(updateConfig()));

    myScene = new QGraphicsScene();

    QHBoxLayout *layout = new QHBoxLayout;
    myView = new QGraphicsView(myScene);
    layout->addWidget(myView);

    ui->widget->setLayout(layout);

    myView->setRenderHints(QPainter::Antialiasing);
    myView->show();

    d = new Database(Database::InitAll, QDir::currentPath());
    
    ui->configLocation->setText(QDir::currentPath());
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void MainWindow::openImage() {

    QString file = QFileDialog::getOpenFileName(this,
            tr("Open Image"), QDir::currentPath(), tr("Image Files (*.png *.jpg *.bmp)"));

    clearScene();

    printf("Opened file - %s\n",file.toAscii().data());

    QPixmap* photo = new QPixmap(file);
    lastPhotoItem = new QGraphicsPixmapItem(*photo);
    currentPhoto = photo->toImage();

    if(1.*ui->widget->width()/photo->width() < 1.*ui->widget->height()/photo->height()) {

        scale = 1.*ui->widget->width()/photo->width();
    } else {
        scale = 1.*ui->widget->height()/photo->height();
    }

    lastPhotoItem->setScale(scale);

    myScene->addItem(lastPhotoItem);
}

void MainWindow::openConfig() {

    QString directory = QFileDialog::getExistingDirectory(this,tr("Select Config Directory"),QDir::currentPath());

    ui->configLocation->setText(directory);

    d = new Database(Database::InitAll, directory);

}

void MainWindow::detectFaces() {

    int i;
    currentFaces = d->detectFaces(currentPhoto);

    Face face;
    
    foreach(face, currentFaces)
    {
        FaceItem *faceItem = new FaceItem(0, myScene,face.toRect(), scale);
        qDebug()<< face.toRect() <<" and scale is "<<scale<<endl;
    }

}

void MainWindow::updateConfig() {

    d->updateFaces(currentFaces);
    d->saveConfig();
}

void MainWindow::clearScene() {
    QList<QGraphicsItem*> list = myScene->items();

    int i;

    for(i=0;i<list.size();i++) {
        myScene->removeItem(list.at(i));
    }
}

void MainWindow::recognise() {
    qDebug()<<"Will run MainWindow::recognise()"<<endl;

    d->recognizeFaces(currentFaces);

    //TODO: create mapping to items.
}
