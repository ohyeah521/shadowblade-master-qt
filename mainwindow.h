#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QThread>
#include <QMainWindow>
#include "session/networkmanager.h"
#include "model/hosttablemodel.h"
#include "defines.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(SessionManager& sessionManager, QWidget *parent = 0);
    ~MainWindow();

public slots:
    void updateView();

    void sendSms();
    void loadSms();
    void loadContact();

    void handleServerStart();

protected:
    void closeEvent(QCloseEvent * event);

private:
    void initView();

private:
    Ui::MainWindow *ui;
    HostTableModel mModel;
    SessionManager& mSessionManager;
    NetworkManager mNetworkManager;
};

#endif // MAINWINDOW_H
