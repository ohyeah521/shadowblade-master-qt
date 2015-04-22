#include "mainwindow.h"
#include <QApplication>
#include <time.h>
#include "defines.h"
#include "session/handler/sendsms.h"
#include "session/handler/loadcontacts.h"
#include "session/handler/loadsms.h"
#include "session/handler/shell.h"

int main(int argc, char *argv[])
{
    SessionManager sessionManager;

    //add session handler

    SendSms sendSms;
    sessionManager.addSessionHandler(ACTION_SEND_SMS, &sendSms);

    LoadContacts loadContacts;
    sessionManager.addSessionHandler(ACTION_UPLOAD_CONTACT, &loadContacts);

    LoadSms loadSms;
    sessionManager.addSessionHandler(ACTION_UPLOAD_SMS, &loadSms);

    Shell shell;
    sessionManager.addSessionHandler(ACTION_SHELL, &shell);

    //start application
    QApplication a(argc, argv);
    MainWindow w(sessionManager);
    w.show();

    return a.exec();
}
