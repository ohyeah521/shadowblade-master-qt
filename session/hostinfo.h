#ifndef HOSTINFO_H
#define HOSTINFO_H

#include <QHostAddress>

struct HostInfo
{
    HostInfo(const QHostAddress& addr = QHostAddress(), const quint16& port = 0, int mode = 0, const QVariant& info = QVariant()):addr(addr), port(port), mode(mode), info(info)
    {}
    enum {
        DIRECT_CONNECT = 1,
        REFLECT_CONNECT = 2
    };
    QHostAddress addr;
    quint16 port;
    QVariant info;
    int mode;
};

#endif // HOSTINFO_H
