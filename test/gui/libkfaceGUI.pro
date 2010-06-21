# -------------------------------------------------
# Project created by QtCreator 2010-03-20T20:13:13
# -------------------------------------------------
TARGET = libfaceGUI
TEMPLATE = app
SOURCES += main.cpp \
    mainwindow.cpp \
    faceitem.cpp
HEADERS += mainwindow.h \
    faceitem.h
FORMS += mainwindow.ui
LIBS += -L /usr/lib/ -lkface
INCLUDEPATH += ../usr/include/libkface
