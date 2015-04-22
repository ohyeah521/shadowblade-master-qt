#include "rawdata.h"

#include "RawData.h"

RawData::RawData(QAbstractSocket* socket):mSocket(socket)
{
    QObject::connect(mSocket,SIGNAL(readyRead()),this,SLOT(onReadReady()));
}

RawData::~RawData()
{
}

void RawData::writeData(const char *data, qint64 len)
{
    mSocket->write(data, len);
    mSocket->flush();
}

void RawData::writeData(const QByteArray& data)
{
    writeData(data,data.length());
}

void RawData::onReadReady()
{
//    mSocket->waitForReadyRead(3000);
    emit onReadData(mSocket->readAll(), this);
}

QAbstractSocket* RawData::socket()
{
    return mSocket;
}

void RawData::close()
{
    mSocket->close();
}
