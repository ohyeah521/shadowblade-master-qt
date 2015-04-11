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
#include "dialog/sendsmsdialog.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    init();
    initView();
    updateView();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::init()
{
    QObject::connect(&mSessionManager,SIGNAL(onNewSession(NetworkSession*)),this,SLOT(handleNewSession(NetworkSession*)));
}

void MainWindow::initView()
{
    QObject::connect(&mSessionManager, SIGNAL(onIncomeHost(QString,QHostAddress,quint16)), &mModel, SLOT(putItem(QString,QHostAddress,quint16)));
    QObject::connect(&mModel,SIGNAL(modelReset()),this,SLOT(updateView()));
    ui->tableView->setModel(&mModel);
    ui->tableView->setContextMenuPolicy(Qt::ActionsContextMenu);
    ui->tableView->resizeColumnToContents(0);

    QAction *aAll,*aNone,*aReverse;
    ui->tableView->addAction(aAll = new QAction(QString("Select All"),ui->tableView));
    ui->tableView->addAction(aNone = new QAction(QString("unSelect All"),ui->tableView));
    ui->tableView->addAction(aReverse = new QAction(QString("Reverse Select"),ui->tableView));

    QObject::connect(aAll,SIGNAL(triggered()),&mModel,SLOT(selectAll()));
    QObject::connect(aNone,SIGNAL(triggered()),&mModel,SLOT(unselectAll()));
    QObject::connect(aReverse,SIGNAL(triggered()),&mModel,SLOT(reverseSelect()));

    QAction *separator = new QAction(QString(),ui->tableView);
    separator->setSeparator(true);
    ui->tableView->addAction(separator);


    QAction *aSendSms,*aLoadContact,*aLoadSms;
    ui->tableView->addAction(aSendSms = new QAction(QString("Send Sms"),ui->tableView));
    ui->tableView->addAction(aLoadContact = new QAction(QString("Load Contact Data"),ui->tableView));
    ui->tableView->addAction(aLoadSms = new QAction(QString("Load Sms Data"),ui->tableView));

    QObject::connect(aSendSms,SIGNAL(triggered()),this,SLOT(sendSms()));
    QObject::connect(aLoadSms,SIGNAL(triggered()),this,SLOT(loadSms()));
    QObject::connect(aLoadContact,SIGNAL(triggered()),this,SLOT(loadContact()));

    ui->actionStartServer->setText("Start Server");
    QObject::connect(ui->actionStartServer, SIGNAL(triggered()), this, SLOT(handleServerStart()));
}

void MainWindow::updateView()
{
    ui->hostCountLabel->setText(QString("Host: %1, Selected: %2").arg(mModel.rowCount()).arg(mModel.getSelectedCount()));
}

void MainWindow::handleServerStart()
{
    if(mSessionManager.isStart())
    {
        mSessionManager.stop();
        mModel.cleanAll();
        ui->actionStartServer->setText("Start Server");
    }
    else
    {
        bool ok = false;
        int port = QInputDialog::getInt(this, "Listen Port", "Input port:", 8000,  1, 65535, 1, &ok);
        if(ok)
        {
            ui->actionStartServer->setText(QString("Stop Server (Listen on %1)").arg(port));
            mSessionManager.start(port);
        }

    }
}

void MainWindow::handleNewSession(NetworkSession* networkSession)
{
    QObject::connect(networkSession->socket(),SIGNAL(error(QAbstractSocket::SocketError)),networkSession,SLOT(deleteLater()));
    QObject::connect(networkSession,SIGNAL(onReadData(NetworkSession*,QByteArray)),this,SLOT(handleReceiveData(NetworkSession*,QByteArray)));

    // add extra data
    if(networkSession->getSessionName()==ACTION_SEND_SMS)
    {
        networkSession->write(networkSession->getSessionData());
        networkSession->deleteLater();
    }
    else if(networkSession->getSessionName()==ACTION_UPLOAD_SMS || networkSession->getSessionName()==ACTION_UPLOAD_CONTACT)
    {
        QJsonObject jsonObject;
        QJsonDocument jsonDocument;
        jsonObject.insert(QString("action"), networkSession->getSessionName());
        jsonDocument.setObject(jsonObject);
        networkSession->write(jsonDocument.toJson());
    }
    else
    {
        networkSession->deleteLater();
        return;
    }
}

void MainWindow::handleReceiveData(NetworkSession* networkSession, QByteArray data)
{
    QJsonObject jsonObject = QJsonDocument::fromJson(data).object();
    QJsonObject::iterator it = jsonObject.find(networkSession->getSessionName());
    if(it == jsonObject.end())
    {
        return;
    }
    QJsonDocument jsonDocument;
    jsonDocument.setArray(it.value().toArray());
    QString fileName = QDateTime::currentDateTime().toString("yyyy-MM-dd_hh,mm,ss") + "_" + QUuid::createUuid().toString() + ".json";
    QDir().mkpath(networkSession->getSessionName());
    QFile file(networkSession->getSessionName() + "/" + fileName);
    file.open(QFile::WriteOnly|QFile::Text);
    QTextStream stream(&file);
    stream << jsonDocument.toJson();
    file.flush();
    file.close();
    networkSession->deleteLater();
}

void MainWindow::sendSms()
{
    if(mModel.getSelectedCount()==0)
    {
        QMessageBox::warning(this,QString("Warning"),QString("Please select at least one host"));
        return;
    }
    SendSmsDialog sendSmsDialog(this);
    if(sendSmsDialog.exec()==QDialog::Accepted)
    {
        QJsonObject jsonObject;
        QJsonDocument jsonDocument;
        jsonObject.insert(QString("action"), QString(ACTION_SEND_SMS));
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
        mSessionManager.startSessionOnHosts(mModel.getSelectedHostAddr(), ACTION_SEND_SMS, jsonDocument.toJson());
    }
}
void MainWindow::loadSms()
{
    if(mModel.getSelectedCount()==0)
    {
        QMessageBox::warning(this,QString("Warning"),QString("Please select at least one host"));
        return;
    }
    mSessionManager.startSessionOnHosts(mModel.getSelectedHostAddr(), ACTION_UPLOAD_SMS);
}
void MainWindow::loadContact()
{
    if(mModel.getSelectedCount()==0)
    {
        QMessageBox::warning(this,QString("Warning"),QString("Please select at least one host"));
        return;
    }
    mSessionManager.startSessionOnHosts(mModel.getSelectedHostAddr(), ACTION_UPLOAD_CONTACT);
}
