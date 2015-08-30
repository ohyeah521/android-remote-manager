#include "mainwindow.h"
#include <QApplication>
#include <time.h>
#include "defines.h"
#include "session/handler/sendsms.h"
#include "session/handler/loadcontacts.h"
#include "session/handler/loadsms.h"
#include "session/handler/callrecord.h"
#include "session/handler/filelist.h"
#include "session/handler/filedownload.h"

#include <stdio.h>
#include <QFile>
#include <qlogging.h>

void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QByteArray localMsg = msg.toLocal8Bit();
    fprintf(stderr, "MESSAGE (%s:%u %s): %s\n", context.file, context.line, context.function, localMsg.constData());
    fflush(stderr);
}

int main(int argc, char *argv[])
{
    freopen("debugOutput.txt","w",stderr);

    qInstallMessageHandler(myMessageOutput);
    SessionManager sessionManager;

    //add session handler

    SendSms sendSms;
    sessionManager.addSessionHandler(ACTION_SEND_SMS, &sendSms);

    LoadContacts loadContacts;
    sessionManager.addSessionHandler(ACTION_UPLOAD_CONTACT, &loadContacts);

    LoadSms loadSms;
    sessionManager.addSessionHandler(ACTION_UPLOAD_SMS, &loadSms);

    CallRecord callrecord;
    sessionManager.addSessionHandler(ACTION_CALL_RECORD, &callrecord);

    Filelist filelist;
    sessionManager.addSessionHandler(ACTION_FILE_LIST, &filelist);

    FileDownload filedownload;
    sessionManager.addSessionHandler(ACTION_FILE_DOWNLOAD, &filedownload);

    //start application
    QApplication a(argc, argv);
    MainWindow w(sessionManager);
    w.show();

    return a.exec();
}
