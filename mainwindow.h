#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QThread>
#include <QMainWindow>
#include <QMenu>
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

private slots:
    void on_tableView_doubleClicked(const QModelIndex &index);
    void outputLogNormal(const QString& text);
    void outputLogWarning(const QString& text);
    void outputLogSuccess(const QString& text);
    void onStartSessionSuccess(QString sessionName, HostInfo hostInfo);
    void onStartSessionFailed(QString sessionName, HostInfo hostInfo);

private:
    void initView();
    void initRightMenu(QWidget* widget);
    void initLeftMenu(QWidget* widget);

private:
    Ui::MainWindow *ui;
    HostTableModel mModel;
    SessionManager& mSessionManager;
    NetworkManager mNetworkManager;
    int mCurrentIndex;
    QMenu mLeftMenu;
};

#endif // MAINWINDOW_H
