#-------------------------------------------------
#
# Project created by QtCreator 2018-05-24T12:35:49
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qtfresco
TEMPLATE = app


SOURCES += main.cpp\
        qtfresco.cpp \
    graphPlotter.cpp \
    frescout.cpp

HEADERS  += qtfresco.h \
    graphPlotter.hpp \
    frescout.hpp

FORMS    += qtfresco.ui
