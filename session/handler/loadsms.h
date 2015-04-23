#pragma once
#include "../session.h"
#include "../datapack.h"

#include <QObject>

class LoadSms: public QObject, public SessionHandler
{
    Q_OBJECT
public:
    void handleSession(Session session);

public slots:
    void saveSmsData(QByteArray data, DataPack* dataPack);
};

