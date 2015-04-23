#include "shell.h"

#include "../../libs/libSessionConsole/session_console.h"
#include "../rawdata.h"

void Shell::handleSession(Session session)
{
    SessionConsole* sessionConsole = new SessionConsole();
    sessionConsole->setAttribute(Qt::WA_DeleteOnClose);
    sessionConsole->setWindowTitle(QString("Terminal [%1:%2]").arg(session.getHostInfo().addr.toString()).arg(session.getHostInfo().port));
    sessionConsole->show();

    RawData * rawData = new RawData(session.getSocket());

    QObject::connect(session.getSocket(),SIGNAL(disconnected()),rawData,SLOT(close()));
    QObject::connect(session.getSocket(),SIGNAL(error(QAbstractSocket::SocketError)),rawData,SLOT(close()));
    QObject::connect(sessionConsole, SIGNAL(destroyed()), rawData,SLOT(close()));

    QObject::connect(session.getSocket(),SIGNAL(aboutToClose()),session.getSocket(),SLOT(deleteLater()));
    QObject::connect(session.getSocket(),SIGNAL(destroyed()),rawData,SLOT(deleteLater()));
    QObject::connect(rawData,SIGNAL(destroyed()),sessionConsole,SLOT(deleteLater()) );


    QObject::connect(rawData,SIGNAL(onReadData(QByteArray, RawData*)), sessionConsole, SLOT(putEcho(QByteArray)));
    QObject::connect(sessionConsole, SIGNAL(startCmd(QByteArray)), rawData, SLOT(writeData(QByteArray)));
}
