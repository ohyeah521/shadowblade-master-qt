#pragma once
#include "../session.h"
#include "../datapack.h"

#include <QObject>

class LoadContacts: public QObject, public SessionHandler
{
    Q_OBJECT
public:
    void handleSession(Session session);
public slots:
    void saveContactData(DataPack* dataPack, QByteArray data);
};

