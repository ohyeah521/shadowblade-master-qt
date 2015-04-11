#ifndef SENDSMSDIALOG_H
#define SENDSMSDIALOG_H

#include <QDialog>
#include <vector>
using std::vector;

namespace Ui {
class SendSmsDialog;
}

class SendSmsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SendSmsDialog(QWidget *parent = 0);
    ~SendSmsDialog();

    QString getContent();
    vector<QString> getPhoneNumberList();
    bool isSendNumberList();

private:
    void initView();
private slots:
    void onAdd();
    void onImport();
    void onClear();
    void onRemove();
    void on_pushButton_clicked();

private:
    Ui::SendSmsDialog *ui;
    vector<QString> mPhoneNumberList;
};

#endif // SENDSMSDIALOG_H
