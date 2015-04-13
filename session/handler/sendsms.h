#pragma once
#include "../session.h"
#include "../datapack.h"

class SendSms: public SessionHandler
{
public:
    void handleSession(Session session);
};

