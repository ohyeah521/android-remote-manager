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
    sendsmsdialog.cpp \
    Tea.cpp \
    crypt.cpp \
    filetransferdialog.cpp \
    progressdialog.cpp \
    filedownload.cpp \
    callrecorddialog.cpp

HEADERS  += mainwindow.h \
    hosttablemodel.h \
    networksessionmanager.h \
    networksession.h \
    sendsmsdialog.h \
    Tea.h \
    crypt.h \
    filetransferdialog.h \
    progressdialog.h \
    filedownload.h \
    callrecorddialog.h

FORMS    += mainwindow.ui \
    sendsmsdialog.ui \
    filetransferdialog.ui \
    progressdialog.ui \
    callrecorddialog.ui
