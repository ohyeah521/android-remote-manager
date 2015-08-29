#ifndef FILEDOWNLOAD_H
#define FILEDOWNLOAD_H

#include "../session.h"
#include "filedownloadproc.h"

#include <QObject>

class FileDownload: public SessionHandler
{
public:
    void handleSession(Session session);
};

#endif // FILEDOWNLOAD_H
