#ifndef NETWORKSERVERMANAGER_H
#define NETWORKSERVERMANAGER_H

#include <QUuid>
#include <QUdpSocket>
#include <QTcpServer>
#include <QTcpSocket>
#include <QThread>
#include <QTimer>
#include <QMutexLocker>
#include <time.h>
#include <map>
using std::pair;
using std::map;
#include <vector>
using std::vector;
#include <string.h>
using std::string;

#include "datapack.h"
#include "session.h"
#include "sessionmanager.h"
#include "hostinfo.h"

#include "../session/hostpool.h"

struct SessionInfo
{
    HostInfo hostInfo;
    QVariant sessionData;
    QString sessionName;
    time_t createTime;
};

class NetworkManager: public QObject
{
    Q_OBJECT
public:
    static const short SIGNATURE = -8531; // 0XDEAD
    enum {
        OPERATION_HEARTBEAT = 0,
        OPERATION_CONNECT_HOST = 1,
        OPERATION_LISTEN_HOST = 2,
        OPERATION_ACCEPT_HOST = 3
    };

    NetworkManager(SessionManager& sessionManager);
    virtual ~NetworkManager();
    bool isStart();
    bool start(int port);
    void stop();
    void startSession(const HostInfo& hostInfo, const QString& sessionName, const QVariant& sessionData = QVariant());

    time_t getTimeout() const;
    void setTimeout(time_t value);

    HostPool& getHostPool();

protected:
    void onIncomeHost(const HostInfo& hostInfo);
    void cleanTimeoutSessions();

signals:
    void onStartSessionSuccess(QString sessionName, HostInfo hostInfo);
    void onStartSessionFailed(QString sessionName, HostInfo hostInfo);
    void onNewSession(QString sessionName, QAbstractSocket* socket);
    void onHostPoolChange();

private slots:
    void onRecvFrom();
    void onAccept();
    void onNewSocket(const QByteArray& data, DataPack* dataPack);
    void onTimeout();

private:
    void init();
    void handleNewSocket(QAbstractSocket *socket);

private:
    time_t mTimeout;
    QUdpSocket mUdpSocket;
    QTcpServer mTcpServer;
    bool mIsStart;
    QTimer mTimer;
    QMutex mSessionInfoMapMutex;
    map<QString,SessionInfo> mSessionMap;
    SessionManager& mSessionManager;
    HostPool mHostPool;
};

#endif // NETWORKSERVERMANAGER_H
