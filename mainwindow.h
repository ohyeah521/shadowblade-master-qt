#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QThread>
#include <QMainWindow>
#include "session/networksessionmanager.h"
#include "model/hosttablemodel.h"

#define ACTION_SEND_SMS "send_sms"
#define ACTION_UPLOAD_SMS "upload_sms"
#define ACTION_UPLOAD_CONTACT "upload_contact"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void updateView();

    void sendSms();
    void loadSms();
    void loadContact();

    void handleNewSession(QString sessionName,QAbstractSocket* socket);
    void saveSmsData(DataPack* dataPack, QByteArray data);
    void saveContactData(DataPack* dataPack, QByteArray data);
    void saveData(QString sessionName, DataPack* dataPack, QByteArray data);
    void handleServerStart();

protected:
    void closeEvent(QCloseEvent * event);

private:
    void init();
    void initView();

private:
    Ui::MainWindow *ui;
    HostTableModel mModel;
    NetworkSessionManager mSessionManager;
};

#endif // MAINWINDOW_H
