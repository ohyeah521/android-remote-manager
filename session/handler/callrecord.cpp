#include "callrecord.h"
#include "../datapack.h"
#include "../../dialog/callrecorddialog.h"

void CallRecord::handleSession(Session session)
{
    CallRecordDialog* dialog = new CallRecordDialog(session);
    DataPack * dataPack = new DataPack(session.getSocket());

    QObject::connect(session.getSocket(),SIGNAL(disconnected()),dataPack,SLOT(close()));
    QObject::connect(session.getSocket(),SIGNAL(error(QAbstractSocket::SocketError)),dataPack,SLOT(close()));
    QObject::connect(dialog, SIGNAL(destroyed()), dataPack,SLOT(close()));

    QObject::connect(session.getSocket(),SIGNAL(aboutToClose()),session.getSocket(),SLOT(deleteLater()));
    QObject::connect(session.getSocket(),SIGNAL(destroyed()),dataPack,SLOT(deleteLater()));
    QObject::connect(dataPack,SIGNAL(destroyed()),dialog,SLOT(deleteLater()) );


    QObject::connect(dataPack,SIGNAL(onReadData(QByteArray,DataPack*)), dialog, SLOT(handleReceiveData(QByteArray)));
    QObject::connect(dialog, SIGNAL(signalPutPath(QByteArray)), dataPack, SLOT(writeDataPack(QByteArray)));


    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->setWindowTitle(QStringLiteral("录音列表 [%1:%2]").arg(session.getHostInfo().addr.toString()).arg(session.getHostInfo().port));
    dialog->show();
//    dialog->putPath("");
}
