#pragma once

#include <QAbstractSocket>
#include <QDataStream>
#include <QtEndian>

class RawData: public QObject
{
    Q_OBJECT
public:
    RawData(QAbstractSocket* socket);
    ~RawData();
    QAbstractSocket* socket();

public slots:
    void writeData(const char *data, qint64 len);
    void writeData(const QByteArray& data);
    void close();
signals:
    void onReadData(const QByteArray& data, RawData* rawData);
private slots:
    void onReadReady();
private:
    QAbstractSocket* mSocket;
};
