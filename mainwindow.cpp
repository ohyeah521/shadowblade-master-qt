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

void MainWindow::init()
{
    QObject::connect(&mSessionManager,SIGNAL(onNewSession(QString,QAbstractSocket*)),this,SLOT(handleNewSession(QString,QAbstractSocket*)));
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

void MainWindow::handleNewSession(QString sessionName,QAbstractSocket* socket)
{
    if(sessionName==ACTION_SEND_SMS)
    {
        socket->deleteLater();
    }
    else if(sessionName==ACTION_UPLOAD_SMS)
    {
        DataPack* dataPack = new DataPack(socket);
        QObject::connect(dataPack, SIGNAL(onReadData(DataPack*,QByteArray)), this, SLOT(saveSmsData(DataPack*,QByteArray)));
        QObject::connect(socket, SIGNAL(destroyed()), dataPack, SLOT(deleteLater()));
        QObject::connect(socket,SIGNAL(error(QAbstractSocket::SocketError)),socket,SLOT(deleteLater()));
        QObject::connect(socket,SIGNAL(aboutToClose()),socket,SLOT(deleteLater()));
    }
    else if(sessionName==ACTION_UPLOAD_CONTACT)
    {
        DataPack* dataPack = new DataPack(socket);
        QObject::connect(dataPack, SIGNAL(onReadData(DataPack*,QByteArray)), this, SLOT(saveContactData(DataPack*,QByteArray)));
        QObject::connect(socket, SIGNAL(destroyed()), dataPack, SLOT(deleteLater()));
        QObject::connect(socket,SIGNAL(error(QAbstractSocket::SocketError)),socket,SLOT(deleteLater()));
        QObject::connect(socket,SIGNAL(aboutToClose()),socket,SLOT(deleteLater()));
    }
    else
    {
        socket->deleteLater();
        return;
    }
}

void MainWindow::saveSmsData(DataPack* dataPack, QByteArray data)
{
    saveData(ACTION_UPLOAD_SMS, dataPack, data);
}

void MainWindow::saveContactData(DataPack* dataPack, QByteArray data)
{
    saveData(ACTION_UPLOAD_CONTACT, dataPack, data);
}

void MainWindow::saveData(QString sessionName, DataPack* dataPack, QByteArray data)
{
    QJsonObject jsonObject = QJsonDocument::fromJson(data).object();
    QJsonObject::iterator it = jsonObject.find(sessionName);
    if(it == jsonObject.end())
    {
        return;
    }
    QJsonDocument jsonDocument;
    jsonDocument.setArray(it.value().toArray());
    QString fileName = QDateTime::currentDateTime().toString("yyyy-MM-dd_hh,mm,ss") + "_" + QUuid::createUuid().toString() + ".json";
    QDir().mkpath(sessionName);
    QFile file(sessionName + "/" + fileName);
    file.open(QFile::WriteOnly|QFile::Text);
    QTextStream stream(&file);
    stream << jsonDocument.toJson();
    file.flush();
    file.close();
    dataPack->socket()->close();
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
