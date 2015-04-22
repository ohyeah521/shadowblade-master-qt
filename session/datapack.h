#pragma once

#include <QAbstractSocket>
#include <QDataStream>
#include <QtEndian>

class DataPack: public QObject
{
    Q_OBJECT
public:
    static const unsigned short SIGNATURE = -8531; // 0XDEAD
    DataPack(QAbstractSocket* socket, bool asyncRead = true);
    ~DataPack();
    QByteArray readDataPack();
    QAbstractSocket* socket();

public slots:
    void writeDataPack(const char *data, qint64 len);
    void writeDataPack(const QByteArray& data);
    void close();
signals:
    void onReadData(const QByteArray& data, DataPack* dataPack);
private:
    int readingDataPack();
private slots:
    void onReadReady();
private:
    QAbstractSocket* mSocket;
    QByteArray mData;
    qint32 mHasRead;
    bool mAsyncRead;
};
