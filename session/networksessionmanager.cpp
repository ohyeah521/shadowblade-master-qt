#include "networksessionmanager.h"

NetworkSessionManager::NetworkSessionManager()
{
    init();
}

NetworkSessionManager::~NetworkSessionManager()
{
    stop();
}

void NetworkSessionManager::init()
{
    mIsStart = false;
    mTimeout = 10000;
    QObject::connect(&mUdpSocket,SIGNAL(readyRead()),this,SLOT(onHostOnline()));
    QObject::connect(&mTcpServer,SIGNAL(newConnection()),this,SLOT(onNewConnect()));
}

bool NetworkSessionManager::isStart()
{
    return mIsStart;
}

bool NetworkSessionManager::start(int port)
{
    if(isStart()) return false;
    mIsStart = true;
    mUdpSocket.bind(port);
    mUdpSocket.open(QIODevice::ReadWrite);
    mTcpServer.listen(QHostAddress::Any, port);
    return true;
}

void NetworkSessionManager::stop()
{
    QMutexLocker locker(&mMutex);
    mSessionMap.clear();
    mIsStart = false;
    mUdpSocket.close();
    mTcpServer.close();
}

void NetworkSessionManager::onHostOnline()
{
    QHostAddress host;
    quint16 port;
    QByteArray datagram;
    datagram.resize(mUdpSocket.pendingDatagramSize());
    mUdpSocket.readDatagram(datagram.data(),datagram.size(), &host, &port);
    emit onIncomeHost(datagram, host, port);
}

void NetworkSessionManager::onNewConnect()
{
    QAbstractSocket *socket = mTcpServer.nextPendingConnection();

    DataPack* dataPack = new DataPack(socket);
    QObject::connect(socket,SIGNAL(destroyed()),dataPack,SLOT(deleteLater()));
    QObject::connect(dataPack,SIGNAL(onReadData(DataPack*,QByteArray)),this,SLOT(handleNewSession(DataPack*,QByteArray)));
    QObject::connect(socket,SIGNAL(error(QAbstractSocket::SocketError)),socket,SLOT(deleteLater()));
    QObject::connect(socket,SIGNAL(aboutToClose()),socket,SLOT(deleteLater()));
}

void NetworkSessionManager::handleNewSession(DataPack* dataPack, QByteArray data)
{
    QAbstractSocket* socket = dataPack->socket();
    QObject::disconnect(dataPack,SIGNAL(onReadData(DataPack*,QByteArray)),this,SLOT(handleNewSession(DataPack*,QByteArray)));
    QObject::disconnect(socket,SIGNAL(error(QAbstractSocket::SocketError)),socket,SLOT(deleteLater()));
    QObject::disconnect(socket,SIGNAL(aboutToClose()),socket,SLOT(deleteLater()));

    QMutexLocker locker(&mMutex);
    cleanTimeoutSessions();
    map<QString, SessionInfo>::iterator it = mSessionMap.find(data);
    if(it==mSessionMap.end())
    {
        dataPack->socket()->close();
        return;
    }

    //send session name
    dataPack->writeDataPack(it->second.sessionName.toLocal8Bit().constData(), it->second.sessionName.length());

    //send session init data if has
    if( it->second.sessionData.length() >0 )
    {
        dataPack->writeDataPack(it->second.sessionData.constData(), it->second.sessionData.length());
    }
    emit onNewSession(it->second.sessionName, dataPack->socket());
    dataPack->deleteLater();
}

void NetworkSessionManager::startSessionOnHosts(vector<pair<QHostAddress, quint16> > addrList, QString sessionName, QByteArray sessionData)
{
    if(addrList.size()==0) return;
    QByteArray sessionUuid = QUuid::createUuid().toByteArray();
    vector<pair<QHostAddress, quint16> >::iterator it = addrList.begin();
    while(it!=addrList.end())
    {
        mUdpSocket.writeDatagram(sessionUuid,it->first,it->second);
        ++it;
    }
    SessionInfo info;
    info.sessionData = sessionData;
    info.sessionName = sessionName;
    info.createTime = clock();

    QMutexLocker locker(&mMutex);
    cleanTimeoutSessions();
    mSessionMap[sessionUuid] = info;
}

void NetworkSessionManager::cleanTimeoutSessions()
{
    time_t expiredTime = clock() - mTimeout;

    map<QString,SessionInfo>::iterator it = mSessionMap.begin();
    while(it!=mSessionMap.end())
    {
        if(it->second.createTime < expiredTime)
        {
            mSessionMap.erase(it);
        }
        ++it;
    }
}

time_t NetworkSessionManager::getTimeout() const
{
    return mTimeout;
}

void NetworkSessionManager::setTimeout(time_t value)
{
    mTimeout = value;
}
