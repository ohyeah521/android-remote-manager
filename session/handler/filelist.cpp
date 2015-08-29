#include "filelist.h"
#include "../datapack.h"
#include "../../dialog/filetransferdialog.h"

void Filelist::handleSession(Session session)
{
    FileTransferDialog* dialog = new FileTransferDialog(session);
    DataPack * dataPack = new DataPack(session.getSocket());

    QObject::connect(session.getSocket(),SIGNAL(disconnected()),dataPack,SLOT(close()));
    QObject::connect(session.getSocket(),SIGNAL(error(QAbstractSocket::SocketError)),dataPack,SLOT(close()));
    QObject::connect(dialog, SIGNAL(destroyed()), dataPack,SLOT(close()));

    QObject::connect(session.getSocket(),SIGNAL(aboutToClose()),session.getSocket(),SLOT(deleteLater()));
    QObject::connect(session.getSocket(),SIGNAL(destroyed()),dataPack,SLOT(deleteLater()));
    QObject::connect(dataPack,SIGNAL(destroyed()),dialog,SLOT(deleteLater()) );


    QObject::connect(dataPack,SIGNAL(onReadData(QByteArray,DataPack*)), dialog, SLOT(handleReceiveData(QByteArray)));
    QObject::connect(dialog, SIGNAL(signalPutPath(QByteArray)), dataPack, SLOT(writeDataPack(QByteArray)));


    dialog->setWindowTitle(QStringLiteral("文件管理 [%1:%2]").arg(session.getHostInfo().addr.toString()).arg(session.getHostInfo().port));
    dialog->show();
//    dialog->putPath("/");
}
