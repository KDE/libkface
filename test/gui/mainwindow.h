#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QFileDialog>

#include <libkface/database.h>
#include <libkface/kface.h>

#include "faceitem.h"

namespace Ui {
    class MainWindow;
}

using namespace KFace;
using namespace std;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
public slots:
    void openImage();
    void openConfig();
    void detectFaces();
    void updateConfig();
    void recognise();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::MainWindow *ui;
    QGraphicsScene *myScene;
    QGraphicsView *myView;
    QGraphicsPixmapItem *lastPhotoItem;
    Database *d;
    QImage currentPhoto;
    double scale;
    void clearScene();
    QList<Face> currentFaces;
};

#endif // MAINWINDOW_H
