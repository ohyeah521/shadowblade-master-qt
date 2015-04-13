#include "sessionmanager.h"

SessionManager::SessionManager()
{
    init();
}

SessionManager::~SessionManager()
{
    stop();
}

void SessionManager::init()
{
    mIsStart = false;
    mTimeout = 10000;
    QObject::connect(&mUdpSocket,SIGNAL(readyRead()),this,SLOT(onHostOnline()));
    QObject::connect(&mTcpServer,SIGNAL(newConnection()),this,SLOT(onNewConnect()));
}

bool SessionManager::isStart()
{
    return mIsStart;
}

bool SessionManager::start(int port)
{
    if(isStart()) return false;
    mIsStart = true;
    mUdpSocket.bind(port);
    mUdpSocket.open(QIODevice::ReadWrite);
    mTcpServer.listen(QHostAddress::Any, port);
    return true;
}

void SessionManager::stop()
{
    QMutexLocker locker(&mSessionInfoMapMutex);
    mSessionMap.clear();
    mIsStart = false;
    mUdpSocket.close();
    mTcpServer.close();
}

void SessionManager::addSessionHandler(QString sessionName, SessionHandler* handler)
{
    QMutexLocker locker(&mSessionHandlerMapMutex);
    mSessionHandlerMap[sessionName] = handler;
}
void SessionManager::removeSessionHandler(QString sessionName)
{
    QMutexLocker locker(&mSessionHandlerMapMutex);
    map<QString, SessionHandler*>::iterator it = mSessionHandlerMap.find(sessionName);
    if(it != mSessionHandlerMap.end())
    {
        mSessionHandlerMap.erase(it);
    }
}
void SessionManager::removeAllSessionHandler()
{
    QMutexLocker locker(&mSessionHandlerMapMutex);
    mSessionHandlerMap.clear();
}

SessionHandler* SessionManager::getSessionHandler(QString sessionName)
{
    QMutexLocker locker(&mSessionHandlerMapMutex);
    map<QString, SessionHandler*>::iterator itHandler = mSessionHandlerMap.find(sessionName);
    if(itHandler == mSessionHandlerMap.end())
    {
        return NULL;
    }
    return itHandler->second;
}

void SessionManager::onHostOnline()
{
    QHostAddress host;
    quint16 port;
    QByteArray datagram;
    datagram.resize(mUdpSocket.pendingDatagramSize());
    mUdpSocket.readDatagram(datagram.data(),datagram.size(), &host, &port);
    emit onIncomeHost(datagram, host, port);
}

void SessionManager::onNewConnect()
{
    QAbstractSocket *socket = mTcpServer.nextPendingConnection();

    DataPack* dataPack = new DataPack(socket);
    QObject::connect(socket,SIGNAL(destroyed()),dataPack,SLOT(deleteLater()));
    QObject::connect(dataPack,SIGNAL(onReadData(DataPack*,QByteArray)),this,SLOT(handleNewSession(DataPack*,QByteArray)));
    QObject::connect(socket,SIGNAL(error(QAbstractSocket::SocketError)),socket,SLOT(deleteLater()));
    QObject::connect(socket,SIGNAL(aboutToClose()),socket,SLOT(deleteLater()));
}

void SessionManager::handleNewSession(DataPack* dataPack, QByteArray data)
{
    QAbstractSocket* socket = dataPack->socket();
    QObject::disconnect(dataPack,SIGNAL(onReadData(DataPack*,QByteArray)),this,SLOT(handleNewSession(DataPack*,QByteArray)));
    QObject::disconnect(socket,SIGNAL(error(QAbstractSocket::SocketError)),socket,SLOT(deleteLater()));
    QObject::disconnect(socket,SIGNAL(aboutToClose()),socket,SLOT(deleteLater()));

    QMutexLocker locker(&mSessionInfoMapMutex);
    cleanTimeoutSessions();
    map<QString, SessionInfo>::iterator it = mSessionMap.find(data);
    if(it==mSessionMap.end())
    {
        socket->close();
        return;
    }

    //session startup process
    dataPack->writeDataPack(it->second.sessionName.toLocal8Bit().constData(),it->second.sessionName.toLocal8Bit().length());

    dataPack->deleteLater();

    //give session to handler
    SessionHandler* sessionHandler = getSessionHandler(it->second.sessionName);
    if(sessionHandler != NULL)
    {
        sessionHandler->handleSession(Session(socket,socket->peerAddress(),socket->peerPort(),it->second.sessionName,it->second.sessionData));
    }
}

void SessionManager::startSessionOnHosts(vector<pair<QHostAddress, quint16> > addrList, QString sessionName, QVariant sessionData)
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

    QMutexLocker locker(&mSessionInfoMapMutex);
    cleanTimeoutSessions();
    mSessionMap[sessionUuid] = info;
}

void SessionManager::cleanTimeoutSessions()
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

time_t SessionManager::getTimeout() const
{
    return mTimeout;
}

void SessionManager::setTimeout(time_t value)
{
    mTimeout = value;
}
