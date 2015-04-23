#include "session_console.h"

SessionConsole::SessionConsole(QWidget *parent):
    QWidget(parent),
    ui(new Ui::Form)
{
    ui->setupUi(this);

    ui->progressBar->setValue(0);
    ui->plainTextEdit->setReadOnly(true);

    ui->plainTextEdit->setStyleSheet("background:#872657;color:white;");
    ui->plainTextEdit->setFont(QFont("微软雅黑",13));

    ui->pushButton->setStyleSheet("background:#436eee;color:white;");
    ui->pushButton->setFont(QFont("微软雅黑",10));

    ui->lineEdit->setStyleSheet("background:#872657;color:white;");
    ui->lineEdit->setFont(QFont("微软雅黑",9));

    /*设置默认位置*/
    this->resize(600,400);
    ui->plainTextEdit->move(0,0);
    ui->plainTextEdit->resize(this->width(),this->height()-49);

    ui->progressBar->move(0,ui->plainTextEdit->height()+1);
    ui->progressBar->resize(this->width(),5);

    ui->pushButton->move(0,ui->progressBar->height()+ui->plainTextEdit->height()+2);
    ui->pushButton->resize(this->width(),20);

    ui->lineEdit->move(0,ui->progressBar->height()+ui->plainTextEdit->height()+ui->pushButton->height()+3);
    ui->lineEdit->resize(this->width(),21);

    connect(ui->pushButton,SIGNAL(clicked()),this,SLOT(onExcuteButtonClicked()));
    connect(ui->lineEdit,SIGNAL(returnPressed()),this,SLOT(onExcuteButtonClicked()));
}

void SessionConsole::paintEvent(QPaintEvent *e){
    ui->plainTextEdit->move(0,0);
    ui->plainTextEdit->resize(this->width(),this->height()-49);

    ui->progressBar->move(0,ui->plainTextEdit->height()+1);
    ui->progressBar->resize(this->width(),5);

    ui->pushButton->move(0,ui->progressBar->height()+ui->plainTextEdit->height()+2);
    ui->pushButton->resize(this->width(),20);

    ui->lineEdit->move(0,ui->progressBar->height()+ui->plainTextEdit->height()+ui->pushButton->height()+3);
    ui->lineEdit->resize(this->width(),21);
}

void SessionConsole::onExcuteButtonClicked(){
    if(ui->lineEdit->text().length() ==0) return;
    ui->pushButton->setText(QStringLiteral("↓↓↓   执行中...   ↓↓↓"));

    emit startCmd((ui->lineEdit->text()+"\n").toLocal8Bit());
    ui->lineEdit->clear();
}

void SessionConsole::putEcho(const QByteArray& str){

    ui->plainTextEdit->appendPlainText(str);
    ui->plainTextEdit->scroll(0,-1);

    ui->pushButton->setText(QStringLiteral("↑↑↑     执行     ↑↑↑"));
    ui->progressBar->setValue(0);
}
