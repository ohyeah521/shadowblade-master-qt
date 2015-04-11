#ifndef NETWORKSESSION_H
#define NETWORKSESSION_H

#include <QAbstractSocket>
#include <QDataStream>
#include <QtEndian>

#define SIGNATURE 0XEEFF

class DataPack: public QObject
{
    Q_OBJECT
public:
    DataPack(QAbstractSocket* socket, bool asyncRead = true);
    ~DataPack();
    void writeDataPack(const char *data, qint64 len);
    QByteArray readDataPack();
    QAbstractSocket* socket();
private:
    int readingDataPack();
private slots:
    void onReadReady();
signals:
    void onReadData(DataPack* networkSession, QByteArray data);
private:
    QAbstractSocket* mSocket;
    QByteArray mData;
    qint32 mHasRead;
    bool mAsyncRead;
};


#endif // NETWORKSESSION_H
