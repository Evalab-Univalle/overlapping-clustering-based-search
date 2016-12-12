#-------------------------------------------------
#
# Project created by QtCreator 2013-09-22T14:31:42
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = clustering
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    algoritmo.cpp \
    cromosoma.cpp \
    lectoraarchivo.cpp

HEADERS  += mainwindow.h \
    algoritmo.h \
    cromosoma.h \
    lectoraarchivo.h

FORMS    += mainwindow.ui

INCLUDEPATH += /usr/local/include/igraph

LIBS += -L/usr/local/lib/ -ligraph

CONFIG   += c++11
