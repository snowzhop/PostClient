#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <SMTP/smtpClient.h>
#include <POP3/pop3Client.h>
#include <LoginDialog/logindialog.h>

namespace Ui {
class MainWindow;
}

class Ui_Main;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    bool connectToPop3Server();

    Ui_Main *ui;
    POP3Client* pop3Client = nullptr;
    SmtpClient* smtpClient = nullptr;
    bool pop3UserConnectionStatus = false;
    bool smtpUserConnectionStatus = false;
};

UserData getUserInfo(QMainWindow* w);
bool checkPop3Response(const QString& response);

#endif // MAINWINDOW_H
