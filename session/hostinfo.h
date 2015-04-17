#ifndef HOSTINFO_H
#define HOSTINFO_H

#include <QHostAddress>

struct HostInfo
{
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
