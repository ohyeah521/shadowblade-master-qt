#ifndef SESSIONMANAGER_H
#define SESSIONMANAGER_H

#include "session.h"
#include <QMutexLocker>
#include <map>
using std::map;

class SessionManager
{
public:
    SessionManager();

    bool handleSession(const Session& session);
    void addSessionHandler(const QString& sessionName, SessionHandler* handler);
    SessionHandler* getSessionHandler(const QString& sessionName);
    void removeSessionHandler(const QString& sessionName);
    void removeAllSessionHandler();

private:
    QMutex mSessionHandlerMapMutex;
    map<QString,SessionHandler*> mSessionHandlerMap;
};

#endif // SESSIONMANAGER_H
