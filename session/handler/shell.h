#ifndef SHELL_H
#define SHELL_H

#include "../session.h"

class Shell: public SessionHandler
{
public:
    void handleSession(Session session);
};

#endif // SHELL_H
