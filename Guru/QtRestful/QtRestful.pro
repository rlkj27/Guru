#-------------------------------------------------
#
# Project created by QtCreator 2011-10-04T21:46:06
#
#-------------------------------------------------

QT       += network

QT       -= gui

TARGET = QtRestful
TEMPLATE = lib
CONFIG += staticlib

SOURCES += qrestserver.cpp \
    qrestclient.cpp

HEADERS += qrestserver.h\
    qrestclient.h

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}



