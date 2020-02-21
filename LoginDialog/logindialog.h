#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

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

    static const int lineHeight = 30;

private:
    QLineEdit* emailLine = nullptr;
    QLineEdit* passwordLine = nullptr;
    QPushButton* okButton = nullptr;

public slots:
    void saveFields();

signals:
    void fieldsSaved(QString em, QString pass);
};

#endif // LOGINDIALOG_H
