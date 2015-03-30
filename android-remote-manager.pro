#-------------------------------------------------
#
# Project created by QtCreator 2015-03-30T22:51:13
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = android-remote-manager
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    androidhosttablemodel.cpp

HEADERS  += mainwindow.h \
    androidhosttablemodel.h

FORMS    += mainwindow.ui
