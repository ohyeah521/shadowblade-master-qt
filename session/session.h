#pragma once
#include <QAbstractSocket>
#include <QHostAddress>

class Session
{
public:
    Session(QAbstractSocket* socket, QHostAddress hostAddress, quint16 port, QString sessionName, QVariant sessionData):
        socket(socket),hostAddress(hostAddress),port(port),sessionName(sessionName),sessionData(sessionData){}

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

    QHostAddress getHostAddress() const
    {
        return hostAddress;
    }

    quint16 getPort() const
    {
        return port;
    }

private:
    QAbstractSocket *socket;
    QHostAddress hostAddress;
    quint16 port;
    QString sessionName;
    QVariant sessionData;
};

class SessionHandler
{
public:
    virtual void handleSession(Session session)=0;
};

