#pragma once
#include <QAbstractSocket>
#include <QHostAddress>

#include "hostinfo.h"

class Session
{
public:
    Session(QAbstractSocket* socket, const HostInfo& hostInfo, QString sessionName, QVariant sessionData):
        socket(socket),hostInfo(hostInfo),sessionName(sessionName),sessionData(sessionData){}

    QAbstractSocket *getSocket() const
    {
        return socket;
    }

    QString getSessionName() const
    {
        return sessionName;
    }

    QVariant getSessionData() const
    {
        return sessionData;
    }

    HostInfo getHostInfo() const
    {
        return hostInfo;
    }

private:
    QAbstractSocket *socket;
    HostInfo hostInfo;
    QString sessionName;
    QVariant sessionData;
};

class SessionHandler
{
public:
    virtual void handleSession(Session session)=0;
};

