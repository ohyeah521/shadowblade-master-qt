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

#include "networksession.h"

struct SessionInfo
{
    QByteArray sessionData;
    QString sessionName;
    time_t createTime;
};

class NetworkSessionManager: public QObject
{
    Q_OBJECT
public:
    NetworkSessionManager();
    virtual ~NetworkSessionManager();
    bool isStart();
    bool start(int port);
    void stop();
    void startSessionOnHosts(vector<pair<QHostAddress, quint16> > addrList, QString sessionName, QByteArray sessionData = QByteArray());

    time_t getTimeout() const;
    void setTimeout(time_t value);

private:
    void init();

signals:
    void onIncomeHost(QString info, QHostAddress host, quint16 port);
    void onNewSession(NetworkSession* networkSession);

public slots:
    void onHostOnline();
    void onNewConnect();
    void handleNewSession(NetworkSession* networkSession, QByteArray data);

private:
    void cleanTimeoutSessions();

private:
    time_t mTimeout;
    QUdpSocket mUdpSocket;
    QTcpServer mTcpServer;
    QMutex mMutex;
    bool mIsStart;
    map<QString,SessionInfo> mSessionMap;
};

#endif // NETWORKSERVERMANAGER_H
