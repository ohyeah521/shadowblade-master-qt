#include "sessionmanager.h"

SessionManager::SessionManager()
{
}


void SessionManager::addSessionHandler(const QString& sessionName, SessionHandler* handler)
{
    QMutexLocker locker(&mSessionHandlerMapMutex);
    mSessionHandlerMap[sessionName] = handler;
}
void SessionManager::removeSessionHandler(const QString& sessionName)
{
    QMutexLocker locker(&mSessionHandlerMapMutex);
    map<QString, SessionHandler*>::iterator it = mSessionHandlerMap.find(sessionName);
    if(it != mSessionHandlerMap.end())
    {
        mSessionHandlerMap.erase(it);
    }
}
void SessionManager::removeAllSessionHandler()
{
    QMutexLocker locker(&mSessionHandlerMapMutex);
    mSessionHandlerMap.clear();
}

SessionHandler* SessionManager::getSessionHandler(const QString& sessionName)
{
    QMutexLocker locker(&mSessionHandlerMapMutex);
    map<QString, SessionHandler*>::iterator itHandler = mSessionHandlerMap.find(sessionName);
    if(itHandler == mSessionHandlerMap.end())
    {
        return NULL;
    }
    return itHandler->second;
}

bool SessionManager::handleSession(const Session& session)
{
    SessionHandler* sessionHandler = getSessionHandler(session.getSessionName());
    if(sessionHandler != NULL)
    {
        sessionHandler->handleSession(session);
        return true;
    }
    return false;
}
