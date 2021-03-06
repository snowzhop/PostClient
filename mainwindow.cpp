#include "mainwindow.h"
#include "LoginDialog/userdata.h"
#include "LetterDialog/letterdialog.h"
#include "SendingDialog/SendingDialog.h"
#include "Base64/base64util.h"
#include "PostClient/PostClient.h"
#include "ui_main.h"
#include "ui_mainwindow.h"
#include "data.h"

#include <QTimer>

#include <QDebug>
#include <cstring>
#include <QFile>

#include <iostream>

MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent),
    ui(new Ui_Main),
    pop3Client(new POP3Client()),
    smtpClient(new SmtpClient()) {

    ui->setupUi(this);
    this->show();

    auto user = PostClient::getUserInfo(this);
    user = getTestUser();
    connectionStatus.user = user;

//    connectToMailBox(getTestUser());
    showLetters();

    connect(ui->tableWidget, &QTableWidget::cellDoubleClicked, this, &MainWindow::showLetter);
    connect(ui->sendingButton, &QPushButton::clicked, this, &MainWindow::createLetter);
}

MainWindow::~MainWindow() {
    delete ui;
    delete pop3Client;
    delete smtpClient;
}

void MainWindow::showLetter(const int& letterNumber) {
    LetterDialog* letterDialog = new LetterDialog(this);
    letterDialog->showLetter(letterBox[static_cast<size_t>(letterNumber)], letterNumber);
    connect(letterDialog, &LetterDialog::deleteLetterSignal, this, &MainWindow::deleteLetter);
    connect(letterDialog, &LetterDialog::replySignal, this, &MainWindow::reply);
}

void MainWindow::sendLetter(std::string email, std::string subject, std::string text, std::string attachmentPath) {
    connectToSmtpServer(connectionStatus.user.smtpServer, connectionStatus.user.smtpPort);
    connectToSmtpUser(connectionStatus.user.email, connectionStatus.user.password);
    smtpClient->createLetter(email.c_str(), subject.c_str(), text.c_str());
    if (attachmentPath.length() != 0) {
        qDebug() << "attachment added";
        smtpClient->addAttachment(attachmentPath.c_str());
    }
    smtpClient->endLetter();
    smtpClient->sendLetter();
    ui->statusBar->showMessage("Letter sended", ui->_5_SECONDS_IN_MS);
}

void MainWindow::createLetter() {
    SendingDialog* sendingDialog = new SendingDialog(this);
    sendingDialog->letterPreparing();
    connect(sendingDialog, &SendingDialog::compileLetter, this, &MainWindow::sendLetter);
}

void MainWindow::deleteLetter(const int& letterNumber) {
    if (!connectionStatus.pop3Connection && !connectionStatus.pop3UserAuth) {
        qDebug() << "letterBox.size() before deleting: " << letterBox.size();
        connectToPop3Server(connectionStatus.user.pop3Server, connectionStatus.user.pop3Port);
        connectToPop3User(connectionStatus.user.email, connectionStatus.user.password);
        char* response = pop3Client->sendRequest(std::string("DELE ").
                         append(std::to_string(letterNumber+1)).
                         append("\r\n"));
        closePop3Connection();
        qDebug() << response;
        ui->statusBar->showMessage(QString(response), ui->_5_SECONDS_IN_MS);
        qDebug() << "letter DeLeTeD: " << letterNumber;
        delete[] response;
        letterBox.erase(letterBox.begin()+letterNumber);
        qDebug() << "letterBox.size() after deleting: " << letterBox.size();
        ui->tableWidget->removeRow(letterNumber);
        ui->tableWidget->update();
    }
}

void MainWindow::reply(const std::string& email) {
    SendingDialog* sendingDialog = new SendingDialog(this);
    sendingDialog->setEmail(email);
    sendingDialog->letterPreparing();
    connect(sendingDialog, &SendingDialog::compileLetter, this, &MainWindow::sendLetter);
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
        if (PostClient::isPop3ResponseCorrect(QString(answer))) {
            connectionStatus.pop3Connection = true;
            return true;
        }
    }
    return false;
}

bool MainWindow::connectToPop3User(const QString &email, const QString &password) {
    if (connectionStatus.pop3Connection) {
        char* answer = nullptr;
        auto req = std::string("USER ").append(email.toUtf8().constData());
        answer = pop3Client->sendRequest(req.append("\r\n"));
        if (!PostClient::isPop3ResponseCorrect(QString(answer))) {
            return false;
        }
        req = std::string("PASS ").append(password.toUtf8().constData());
        answer = pop3Client->sendRequest(req.append("\r\n"));
        if (!PostClient::isPop3ResponseCorrect(QString(answer))) {
            return false;
        }
        connectionStatus.pop3UserAuth = true;
        return true;
    }
    return false;
}

bool MainWindow::closePop3Connection() {
    if (connectionStatus.pop3Connection && connectionStatus.pop3UserAuth) {
        char* response = pop3Client->sendRequest("QUIT\r\n");
        connectionStatus.pop3UserAuth = false;
        connectionStatus.pop3Connection = false;

        return PostClient::isPop3ResponseCorrect(response);
    }
    return true;
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
        connectionStatus.smtpConnection = true;
        return true;
    }
    return false;
}

bool MainWindow::connectToSmtpUser(const QString &email, const QString &password) {
    if (connectionStatus.smtpConnection) {
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
    connectToPop3Server(connectionStatus.user.pop3Server, connectionStatus.user.pop3Port);
    connectToPop3User(connectionStatus.user.email, connectionStatus.user.password);
    if (connectionStatus.pop3Connection) {
        char* response = pop3Client->sendRequest("LIST\r\n");
        if (PostClient::isPop3ResponseCorrect(response)) {
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
            ui->tableWidget->setRowCount(numOfLetters);
            ui->tableWidget->setColumnCount(1);
            letterBox.reserve(static_cast<size_t>(numOfLetters));
            for (int i = 1; i <= numOfLetters; i++) {
                response = pop3Client->sendRequest(std::string("RETR ")
                                                   .append(std::to_string(i))
                                                   .append("\r\n"));
                if (PostClient::isPop3ResponseCorrect(QString(response))) {
                    std::string responseStr(response);
                    delete[] response;
                    std::cout << i << ". ";
                    auto* subject = PostClient::findSubject(responseStr);
                    QTableWidgetItem* tableItem = new QTableWidgetItem(*subject);
                    tableItem->setFlags(tableItem->flags() ^ Qt::ItemIsEditable);
                    ui->tableWidget->setItem(i-2, 1, tableItem);
                    delete subject;
                    letterBox.push_back(responseStr);
                } else {
                    qDebug() << "Error response: " << response;
                    ui->statusBar->showMessage(QString(response));
                }
            }
        }

        if (closePop3Connection()) {
            qDebug() << "connection closed";
        } else {
            qDebug() << "connection not closed";
        }
    }
}

void printStr(const std::string& str, size_t begin, size_t end) {
    for (size_t i = begin; i <= end; ++i) {
        std::cout << str[i];
    }
    std::cout.flush();
}
