#include "loadcontacts.h"
#include "../../util/data.h"
#include "../../defines.h"

void LoadContacts::handleSession(Session session)
{
    QAbstractSocket* socket = session.getSocket();
    DataPack* dataPack = new DataPack(socket);
    QObject::connect(dataPack, SIGNAL(onReadData(QByteArray,DataPack*)), this, SLOT(saveContactData(QByteArray,DataPack*)));
    QObject::connect(socket, SIGNAL(destroyed()), dataPack, SLOT(deleteLater()));
    QObject::connect(socket,SIGNAL(error(QAbstractSocket::SocketError)),socket,SLOT(deleteLater()));
    QObject::connect(socket,SIGNAL(aboutToClose()),socket,SLOT(deleteLater()));
}

void LoadContacts::saveContactData(QByteArray data, DataPack* dataPack)
{
    dataPack->socket()->close();
    Util::Data::saveData(ACTION_UPLOAD_CONTACT, data);
}
