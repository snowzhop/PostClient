#include "mainwindow.h"
#include "LoginDialog/userdata.h"
#include "ui_main.h"
#include "ui_mainwindow.h"
#include "data.h"

#include <QDebug>

#include <iostream>

MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent),
    ui(new Ui_Main),
    pop3Client(new POP3Client()),
    smtpClient(new SmtpClient()) {
    ui->setupUi(this);
    this->show();

    auto user = getUserInfo(this);

    connectToMailBox(getTestUser());
    showLetters();
}

MainWindow::~MainWindow() {
    delete ui;
    delete pop3Client;
    delete smtpClient;
}

UserData getUserInfo(QMainWindow* w) {
    LoginDialog loginDialog(w);
    loginDialog.setModal(true);

    UserData user;
    QObject::connect(&loginDialog, &LoginDialog::fieldsSaved,
    [&user](const UserData& d) {
        user = d;
    });

    loginDialog.exec();

    return user;
}

void MainWindow::connectToMailBox(const UserData& user) {
    // POP3
    if (!connectToPop3Server(user.pop3Server, user.pop3Port)) {
        ui->statusBar->showMessage("Can't connect to POP3 server");
    } else {
        ui->statusBar->showMessage("Connected to POP3 server", ui->_5_SECONDS_IN_MS);
        qDebug() << "Connected to POP3 server";
    }

    if (!connectToPop3User(user.email, user.password)) {
        ui->statusBar->showMessage("Can't auth user (POP3)");
    } else {
        ui->statusBar->showMessage("User was authenticated(POP3)", ui->_5_SECONDS_IN_MS);
        qDebug() << "User was authenticated(POP3)";
    }

    // SMTP
    if (!connectToSmtpServer(user.smtpServer, user.smtpPort)) {
        ui->statusBar->showMessage("Can't connect to SMTP server");
    } else {
        ui->statusBar->showMessage("Connected to SMTP server", ui->_5_SECONDS_IN_MS);
    }

    if (!connectToSmtpUser(user.email, user.password)) {
        ui->statusBar->showMessage("Can't auth user (SMTP)");
    } else {
        ui->statusBar->showMessage("User was authenticated (SMTP)", ui->_5_SECONDS_IN_MS);
    }
}

bool MainWindow::connectToPop3Server(const QString& serverAddr, const QString& serverPort) {
    bool ok = false;
    char* answer = nullptr;
    short port = serverPort.toShort(&ok);
    if (ok) {
        answer = pop3Client->connectToServer(serverAddr.toUtf8().constData(), port);
        if (isPop3ResponseCorrect(QString(answer))) {
            status.pop3Connection = true;
            return true;
        }
    }
    return false;
}

bool MainWindow::connectToPop3User(const QString &email, const QString &password) {
    if (status.pop3Connection) {
        char* answer = nullptr;
        auto req = std::string("USER ").append(email.toUtf8().constData());
        answer = pop3Client->sendRequest(req.append("\r\n"));
        if (!isPop3ResponseCorrect(QString(answer))) {
            return false;
        }
        req = std::string("PASS ").append(password.toUtf8().constData());
        answer = pop3Client->sendRequest(req.append("\r\n"));
        if (!isPop3ResponseCorrect(QString(answer))) {
            return false;
        }
        status.pop3UserAuth = true;
        return true;
    }
    return false;
}

bool MainWindow::connectToSmtpServer(const QString &serverAddr, const QString &serverPort) {
    bool ok = false;
    short port = serverPort.toShort(&ok);
    if (ok) {
        try {
            smtpClient->connectToSMTPServer(serverAddr.toUtf8().constData(), port);
        } catch (const std::runtime_error& ex) {
            qDebug() << ex.what();
            return false;
        }
        status.smtpConnection = true;
        return true;
    }
    return false;
}

bool MainWindow::connectToSmtpUser(const QString &email, const QString &password) {
    if (status.smtpConnection) {
        try {
            smtpClient->login(email.toUtf8().constData(), password.toUtf8().constData());
        } catch (const std::runtime_error&) {
            return false;
        }
        return true;
    }
    return false;
}

void MainWindow::showLetters() {
    if (status.pop3Connection) {
        char* response = pop3Client->sendRequest("LIST\r\n");
        if (isPop3ResponseCorrect(response)) {
            std::string strResponse(response);
            delete[] response;
            size_t startPos = 4;
            size_t endPos;
            for (endPos = 0; endPos < strResponse.size(); ++endPos) {
                if (strResponse[endPos] == ' ') {
                    break;
                }
            }
            if (endPos == strResponse.size() - 1) {
                ui->statusBar->showMessage("Wrong response from server");
                return;
            }
            auto numOfLetters = std::atoi(strResponse.substr(startPos, endPos-1).c_str());
            for (int i = 1; i <= numOfLetters; i++) {
                response = pop3Client->sendRequest(std::string("RETR ")
                                                   .append(std::to_string(i))
                                                   .append("\r\n"));
                std::string responseStr(response);
                delete[] response;
                std::string::size_type subjectBeginPos = responseStr.find("\r\nSubject: ") + lenOfSubjectStr;
                std::string::size_type subjectEndPos = responseStr.find("\r\nTo:", subjectBeginPos);
                std::cout << "\t";
                for (std::string::size_type i = subjectBeginPos; i < subjectEndPos; ++i) {
                    std::cout << responseStr[i];
                }
                std::cout << std::endl;
            }
        }
    }
}

bool isPop3ResponseCorrect(const QString& response) {
    if (response.size() != 0) {
        const size_t len = 3;
        if (response.chopped(len).contains("+OK")) {
            return true;
        }
    }
    return false;
}
