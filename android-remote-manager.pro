#-------------------------------------------------
#
# Project created by QtCreator 2015-03-30T22:51:13
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = android-remote-manager
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    hosttablemodel.cpp \
    networksessionmanager.cpp \
    networksession.cpp \
    sendsmsdialog.cpp

HEADERS  += mainwindow.h \
    hosttablemodel.h \
    networksessionmanager.h \
    networksession.h \
    sendsmsdialog.h

FORMS    += mainwindow.ui \
    sendsmsdialog.ui
