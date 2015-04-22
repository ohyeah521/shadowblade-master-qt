#ifndef SESSION_CONSOLE_H
#define SESSION_CONSOLE_H

#include "ui_session_console.h"

#include <QWidget>
#include <QPaintEvent>
#include <QThread>

class SessionConsole: public QWidget
{
    Q_OBJECT
public:
    explicit SessionConsole(QWidget *parent = 0);

private:
    Ui::Form* ui;
    void paintEvent(QPaintEvent* e);
signals:
    /*当命令编辑完成发送startCmd*/
    void startCmd(const QByteArray& data);

public slots:
    void putEcho(const QByteArray& str);

private slots:
    void onExcuteButtonClicked();
};

#endif // SESSION_CONSOLE_H
