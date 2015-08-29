#ifndef FILELIST_H
#define FILELIST_H

#include "../session.h"

class Filelist: public SessionHandler
{
public:
    void handleSession(Session session);
};

#endif // FILELIST_H
