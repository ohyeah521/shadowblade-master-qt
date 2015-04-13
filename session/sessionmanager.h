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

struct SessionInfo
{
    QVariant sessionData;
    QString sessionName;
    time_t createTime;
};

class SessionManager: public QObject
{
    Q_OBJECT
public:
    SessionManager();
    virtual ~SessionManager();
    bool isStart();
    bool start(int port);
    void stop();
    void startSessionOnHosts(vector<pair<QHostAddress, quint16> > addrList, QString sessionName, QVariant sessionData = QVariant());

    time_t getTimeout() const;
    void setTimeout(time_t value);

    void addSessionHandler(QString sessionName, SessionHandler* handler);
    SessionHandler* getSessionHandler(QString sessionName);
    void removeSessionHandler(QString sessionName);
    void removeAllSessionHandler();

private:
    void init();

signals:
    void onIncomeHost(QString info, QHostAddress host, quint16 port);
    void onNewSession(QString sessionName, QAbstractSocket* socket);

private slots:
    void onHostOnline();
    void onNewConnect();
    void handleNewSession(DataPack* dataPack, QByteArray data);

private:
    void cleanTimeoutSessions();

private:
    time_t mTimeout;
    QUdpSocket mUdpSocket;
    QTcpServer mTcpServer;
    bool mIsStart;
    QMutex mSessionInfoMapMutex;
    map<QString,SessionInfo> mSessionMap;
    QMutex mSessionHandlerMapMutex;
    map<QString,SessionHandler*> mSessionHandlerMap;
};

#endif // NETWORKSERVERMANAGER_H
