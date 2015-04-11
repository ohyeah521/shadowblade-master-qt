#ifndef NETWORKSESSION_H
#define NETWORKSESSION_H

#include <QAbstractSocket>
#include <QDataStream>
#include <QtEndian>

#define SIGNATURE 0XEEFF

class NetworkSession: public QObject
{
    Q_OBJECT
public:
    NetworkSession(QAbstractSocket* socket);
    ~NetworkSession();
    void write(const QByteArray& data);
    void close();
    void setSessionName(QString sessionName);
    QString getSessionName();
    void setSessionUuid(QString sessionUuid);
    QString getSessionUuid();
    void setSessionData(QByteArray sessionData);
    QByteArray getSessionData();
    QAbstractSocket* socket();
private slots:
    void onReadReady();
signals:
    void onReadData(NetworkSession* networkSession, QByteArray data);
private:
    QAbstractSocket* mSocket;
    QByteArray mData;
    qint32 mHasRead;
    QString mSessionName;
    QString mSessionUuid;
    QByteArray mSessionData;
};


#endif // NETWORKSESSION_H
