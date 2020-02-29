#include "mainwindow.h"
#include "LoginDialog/userdata.h"
#include "LetterDialog/letterdialog.h"
#include "Base64/base64util.h"
#include "ui_main.h"
#include "ui_mainwindow.h"
#include "data.h"

#include <QDebug>
#include <cstring>
#include <QFile>
#include <QStandardPaths>

#include <iostream>

MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent),
    ui(new Ui_Main),
    pop3Client(new POP3Client()),
    smtpClient(new SmtpClient()) {

    ui->setupUi(this);
    this->show();

    auto user = PostClient::getUserInfo(this);

    connectToMailBox(getTestUser());
    showLetters();

    connect(ui->tableWidget, &QTableWidget::cellDoubleClicked, this, &MainWindow::showLetter);
}

MainWindow::~MainWindow() {
    delete ui;
    delete pop3Client;
    delete smtpClient;
}

void MainWindow::showLetter(const int& letterNumber) {
    if (connectionStatus.pop3UserAuth && connectionStatus.pop3Connection) {
        LetterDialog* letterDialog = new LetterDialog(this);
        letterDialog->showLetter(*pop3Client, letterNumber);
        char* response = pop3Client->sendRequest(std::string("RETR ").
                         append(std::to_string(letterNumber)).
                         append("\r\n"));

        QString strResponse(response);
        delete[] response;
        if (PostClient::isPop3ResponseCorrect(strResponse)) {
            auto boundaryBeginPos = strResponse.indexOf("boundary=\"");         // Return value can be -1
            auto boundaryEndPos = strResponse.indexOf("\"", boundaryBeginPos);  // Return value can be -1
            QStringRef boundary(&strResponse, boundaryBeginPos, boundaryEndPos - boundaryBeginPos);
            qDebug() << "boundary = " << boundary
                     << "\tbegin = " << boundaryBeginPos
                     << "\tend = " << boundaryEndPos;
        }
    } else {
        ui->statusBar->showMessage("POP3 connection closed");
    }
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
            for (int i = 1; i <= numOfLetters; i++) {
                response = pop3Client->sendRequest(std::string("RETR ")
                                                   .append(std::to_string(i))
                                                   .append("\r\n"));
                std::string responseStr(response);
                delete[] response;
                std::cout << i << ". ";
                auto* subject = PostClient::findSubject(responseStr);
                QTableWidgetItem* tableItem = new QTableWidgetItem(*subject);
                tableItem->setFlags(tableItem->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(i-2, 1, tableItem);
                delete subject;
            }
        }
    }
}

void printStr(const std::string& str, size_t begin, size_t end) {
    for (size_t i = begin; i <= end; ++i) {
        std::cout << str[i];
    }
    std::cout.flush();
}
