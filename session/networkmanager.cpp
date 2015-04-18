#include "networkmanager.h"

NetworkManager::NetworkManager(SessionManager& sessionManager):mSessionManager(sessionManager)
{
    init();
}

NetworkManager::~NetworkManager()
{
    stop();
}

void NetworkManager::init()
{
    mIsStart = false;
    mTimeout = 10000;
    QObject::connect(&mUdpSocket,SIGNAL(readyRead()),this,SLOT(onRecvFrom()));
    QObject::connect(&mTcpServer,SIGNAL(newConnection()),this,SLOT(onAccept()));
}

bool NetworkManager::isStart()
{
    return mIsStart;
}

bool NetworkManager::start(int port)
{
    if(isStart()) return false;
    mIsStart = true;
    mUdpSocket.bind(port);
    mUdpSocket.open(QIODevice::ReadWrite);
    mTcpServer.listen(QHostAddress::Any, port);
    return true;
}

void NetworkManager::stop()
{
    QMutexLocker locker(&mSessionInfoMapMutex);
    mSessionMap.clear();
    mIsStart = false;
    mUdpSocket.close();
    mTcpServer.close();
}

void NetworkManager::onRecvFrom()
{
    QHostAddress host;
    quint16 port;
    QByteArray datagram;
    datagram.resize(mUdpSocket.pendingDatagramSize());
    mUdpSocket.readDatagram(datagram.data(),datagram.size(), &host, &port);
    QDataStream dataStream(datagram);
    short signature = 0;
    dataStream >> signature;
    if( signature != SIGNATURE )
    {
        return;
    }
    short operation = 0;
    dataStream >> operation;
    if(operation == OPERATION_HEARTBEAT)
    {
        QByteArray info(datagram);
        info.remove(0, sizeof(signature) + sizeof(operation));
        HostInfo hostInfo;
        hostInfo.addr = host;
        hostInfo.port = port;
        hostInfo.info = info;
        hostInfo.mode = HostInfo::REFLECT_CONNECT;
        emit onIncomeHost(hostInfo);
    }
    else if(operation == OPERATION_ACCEPT_HOST)
    {
        short listenPort = 0;
        dataStream >> listenPort;
        if(listenPort == 0)
        {
            return;
        }
        QByteArray uuid(datagram);
        uuid.remove(0, sizeof(signature) + sizeof(operation) + sizeof(listenPort));
        QMutexLocker locker(&mSessionInfoMapMutex);
        map<QString, SessionInfo>::iterator it = mSessionMap.find(uuid);
        if(it==mSessionMap.end())
        {
            return;
        }
        QTcpSocket *socket = new QTcpSocket();
        handleNewSocket(socket);
        socket->connectToHost(host,listenPort);
    }
    else
    {
        ;
    }
}

void NetworkManager::onAccept()
{
    handleNewSocket(mTcpServer.nextPendingConnection());
}

void NetworkManager::handleNewSocket(QAbstractSocket *socket)
{
    DataPack* dataPack = new DataPack(socket);
    QObject::connect(socket,SIGNAL(destroyed()),dataPack,SLOT(deleteLater()));
    QObject::connect(dataPack,SIGNAL(onReadData(DataPack*,QByteArray)),this,SLOT(onNewSocket(DataPack*,QByteArray)));
    QObject::connect(socket,SIGNAL(error(QAbstractSocket::SocketError)),socket,SLOT(deleteLater()));
    QObject::connect(socket,SIGNAL(aboutToClose()),socket,SLOT(deleteLater()));
}

void NetworkManager::onNewSocket(DataPack* dataPack, QByteArray data)
{
    QAbstractSocket* socket = dataPack->socket();
    QObject::disconnect(dataPack,SIGNAL(onReadData(DataPack*,QByteArray)),this,SLOT(onNewSocket(DataPack*,QByteArray)));
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
    //start session success
    emit onStartSessionSuccess(it->second.sessionName, it->second.hostInfo);

    //session startup process
    dataPack->writeDataPack(it->second.sessionName.toLocal8Bit().constData(),it->second.sessionName.toLocal8Bit().length());

    dataPack->deleteLater();

    //give session to handler
    mSessionManager.handleSession(Session(socket,
                                          HostInfo(socket->peerAddress(),socket->peerPort(), HostInfo::REFLECT_CONNECT),
                                          it->second.sessionName,it->second.sessionData));
}

void NetworkManager::startSession(const HostInfo& hostInfo, const QString& sessionName, const QVariant& sessionData)
{
    if(hostInfo.mode != HostInfo::REFLECT_CONNECT || hostInfo.mode != HostInfo::REFLECT_CONNECT) return;
    QByteArray sessionUuid = QUuid::createUuid().toByteArray();
    QByteArray data;
    QDataStream dataStream(&data, QIODevice::WriteOnly);
    dataStream.setByteOrder(QDataStream::BigEndian);
    dataStream << SIGNATURE << (short)( (hostInfo.mode == HostInfo::REFLECT_CONNECT) ? OPERATION_CONNECT_HOST : OPERATION_LISTEN_HOST ); //signature & operation code
    dataStream.writeRawData(sessionUuid.data(),sessionUuid.length());
    mUdpSocket.writeDatagram(data,hostInfo.addr,hostInfo.port);

    SessionInfo info;
    info.sessionData = sessionData;
    info.sessionName = sessionName;
    info.hostInfo = hostInfo;
    info.createTime = clock();

    QMutexLocker locker(&mSessionInfoMapMutex);
    cleanTimeoutSessions();
    mSessionMap[sessionUuid] = info;
}

void NetworkManager::cleanTimeoutSessions()
{
    time_t expiredTime = clock() - mTimeout;

    map<QString,SessionInfo>::iterator it = mSessionMap.begin();
    while(it!=mSessionMap.end())
    {
        if(it->second.createTime < expiredTime)
        {
            //start session failed
            emit onStartSessionFailed(it->second.sessionName, it->second.hostInfo);
            //remove it
            mSessionMap.erase(it);
        }
        ++it;
    }
}

time_t NetworkManager::getTimeout() const
{
    return mTimeout;
}

void NetworkManager::setTimeout(time_t value)
{
    mTimeout = value;
}
