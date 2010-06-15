# -------------------------------------------------
# Project created by QtCreator 2010-06-14T01:10:03
# -------------------------------------------------
QT += gui
TARGET = kface
TEMPLATE = lib
DEFINES += KFACE_LIBRARY
SOURCES += database.cpp \
    kface.cpp \
    kfaceutils.cpp
HEADERS += database.h \
    kface_global.h \
    kface.h \
    kfaceutils.h
LIBS += -L \
    /usr/lib/ \
    -lface
