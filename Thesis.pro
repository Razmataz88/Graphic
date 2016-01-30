#-------------------------------------------------
#
# Project created by QtCreator 2014-08-28T20:12:35
#
#-------------------------------------------------

QT       += core gui svg

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Graphic
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    node.cpp \
    edge.cpp \
    canvasscene.cpp \
    preview.cpp \
    graphmimedata.cpp \
    graph.cpp \
    canvasview.cpp \
    label.cpp \
    sizecontroller.cpp \
    labelsizecontroller.cpp \
    labelcontroller.cpp \
    basicgraphs.cpp \
    colourfillcontroller.cpp \
    colourlinecontroller.cpp


HEADERS  += mainwindow.h \
    node.h \
    preview.h \
    graphmimedata.h \
    graph.h \
    edge.h \
    canvasview.h \
    canvasscene.h \
    label.h \
    sizecontroller.h \
    labelsizecontroller.h \
    labelcontroller.h \
    basicgraphs.h \
    colourlinecontroller.h \
    colourfillcontroller.h


FORMS    += mainwindow.ui

CONFIG += c++11 \
    thread


RESOURCES = application.qrc


