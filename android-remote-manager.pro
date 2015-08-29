#-------------------------------------------------
#
# Project created by QtCreator 2015-03-30T22:51:13
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = android-remote-manager
TEMPLATE = app

FORMS += \
    mainwindow.ui \
    dialog/callrecorddialog.ui \
    dialog/filetransferdialog.ui \
    dialog/progressdialog.ui \
    dialog/sendsmsdialog.ui

HEADERS += \
    mainwindow.h \
    model/hosttablemodel.h \
    dialog/callrecorddialog.h \
    dialog/filetransferdialog.h \
    dialog/progressdialog.h \
    dialog/sendsmsdialog.h \
    util/crypt.h \
    util/Tea.h \
    util/data.h \
    session/datapack.h \
    session/hostinfo.h \
    session/hostpool.h \
    session/networkmanager.h \
    session/rawdata.h \
    session/session.h \
    session/sessionmanager.h \
    session/handler/loadcontacts.h \
    session/handler/loadsms.h \
    session/handler/sendsms.h \
    defines.h \
    session/handler/callrecord.h \
    session/handler/filelist.h \
    session/handler/filedownload.h \
    session/handler/filedownloadproc.h

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    model/hosttablemodel.cpp \
    dialog/callrecorddialog.cpp \
    dialog/filetransferdialog.cpp \
    dialog/progressdialog.cpp \
    dialog/sendsmsdialog.cpp \
    util/crypt.cpp \
    util/Tea.cpp \
    util/data.cpp \
    session/datapack.cpp \
    session/hostpool.cpp \
    session/networkmanager.cpp \
    session/rawdata.cpp \
    session/sessionmanager.cpp \
    session/handler/loadcontacts.cpp \
    session/handler/loadsms.cpp \
    session/handler/sendsms.cpp \
    session/handler/callrecord.cpp \
    session/handler/filelist.cpp \
    session/handler/filedownload.cpp \
    session/handler/filedownloadproc.cpp
