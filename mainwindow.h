#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <SMTP/smtpClient.h>
#include <POP3/pop3Client.h>
#include <LoginDialog/logindialog.h>

class Ui_Main;

namespace Ui {
class MainWindow;
}

struct ConnectionStatus {
    bool pop3Connection = false;
    bool pop3UserAuth   = false;
    bool smtpConnection = false;
    bool smtpUserAuth   = false;

    std::string email;
    std::string password;
};

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    static const size_t lenOfSubjectStr = 10;
    static const char codingOffset = 2;

public slots:
    void sendLetter(std::string email, std::string subject, std::string text, std::string attachmentPath);

private slots:
    void showLetter(const int& letterNumber);
    void createLetter();

private:
    void connectToMailBox(const UserData& user);

    bool connectToPop3Server(const QString& serverAddr, const QString& serverPort);
    bool connectToPop3User(const QString& email, const QString& password);
    bool closePop3Connection();
    void showLetters();
    int getListOfLetters();

    bool connectToSmtpServer(const QString& serverAddr, const QString& serverPort);
    bool connectToSmtpUser(const QString& email, const QString& password);

    Ui_Main *ui;
    POP3Client* pop3Client = nullptr;
    SmtpClient* smtpClient = nullptr;

    ConnectionStatus connectionStatus;
    std::vector<std::string> letterBox;

};

#endif // MAINWINDOW_H
