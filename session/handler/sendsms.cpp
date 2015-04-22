#include "sendsms.h"

void SendSms::handleSession(Session session)
{
    DataPack dataPack(session.getSocket());
    dataPack.writeDataPack(session.getSessionData().toByteArray());
}
