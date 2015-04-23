#include "loadsms.h"
#include "../../util/data.h"
#include "../../defines.h"

void LoadSms::handleSession(Session session)
{
    QAbstractSocket* socket = session.getSocket();
    DataPack* dataPack = new DataPack(socket);
    QObject::connect(dataPack, SIGNAL(onReadData(QByteArray,DataPack*)), this, SLOT(saveSmsData(QByteArray,DataPack*)));
    QObject::connect(socket, SIGNAL(destroyed()), dataPack, SLOT(deleteLater()));
    QObject::connect(socket,SIGNAL(error(QAbstractSocket::SocketError)),socket,SLOT(deleteLater()));
    QObject::connect(socket,SIGNAL(aboutToClose()),socket,SLOT(deleteLater()));
}

void LoadSms::saveSmsData(QByteArray data, DataPack* dataPack)
{
    dataPack->socket()->close();
    Util::Data::saveData(ACTION_UPLOAD_SMS, data);
}
