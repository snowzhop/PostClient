#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include "userdata.h"

#include <QDialog>
#include <QMainWindow>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

class LoginDialog : public QDialog {
    Q_OBJECT
public:
    LoginDialog(QMainWindow* parent = nullptr);
    virtual ~LoginDialog();

private:
    static const int loginDialogWidth = 200;

    QLineEdit* emailLine        = nullptr;
    QLineEdit* passwordLine     = nullptr;
    QLineEdit* smtpServerLine   = nullptr;
    QLineEdit* smtpPortLine     = nullptr;
    QLineEdit* pop3ServerLine   = nullptr;
    QLineEdit* pop3PortLine     = nullptr;
    QPushButton* okButton       = nullptr;

public slots:
    void saveFields();

signals:
    void fieldsSaved(const UserData& data);
};

#endif // LOGINDIALOG_H
