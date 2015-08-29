#ifndef CALLRECORD_H
#define CALLRECORD_H

#include "../session.h"

class CallRecord: public SessionHandler
{
public:
    void handleSession(Session session);
};

#endif // CALLRECORD_H
