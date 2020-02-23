#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <SMTP/smtpClient.h>
#include <POP3/pop3Client.h>
#include <LoginDialog/logindialog.h>

namespace Ui {
class MainWindow;
}

struct Status {
    bool pop3Connection = false;
    bool pop3UserAuth   = false;
    bool smtpConnection = false;
    bool smtpUserAuth   = false;
};

class Ui_Main;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void connectToMailBox(const UserData& user);

    bool connectToPop3Server(const QString& serverAddr, const QString& serverPort);
    bool connectToPop3User(const QString& email, const QString& password);
    int getListOfLetters();

    bool connectToSmtpServer(const QString& serverAddr, const QString& serverPort);
    bool connectToSmtpUser(const QString& email, const QString& password);
    void sendLetter();

    Ui_Main *ui;
    POP3Client* pop3Client = nullptr;
    SmtpClient* smtpClient = nullptr;
    Status status;
};

UserData getUserInfo(QMainWindow* w);
bool isPop3ResponseCorrect(const QString& response);

#endif // MAINWINDOW_H
