#ifndef HOSTINFO_H
#define HOSTINFO_H

#include <QHostAddress>

struct HostInfo
{
    HostInfo(const QHostAddress& addr = QHostAddress(), const quint16& port = 0, int mode = REFLECT_CONNECT, const QVariant& info = QVariant()):addr(addr), port(port), mode(mode), info(info)
    {}
    enum {
        DIRECT_CONNECT,
        REFLECT_CONNECT
    };
    QHostAddress addr;
    quint16 port;
    QVariant info;
    int mode;
};

#endif // HOSTINFO_H
