#include "mainwindow.h"
#include "LoginDialog/userdata.h"
#include "ui_main.h"
#include "ui_mainwindow.h"
#include <QDebug>

MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent),
    ui(new Ui_Main),
    pop3Client(new POP3Client()),
    smtpClient(new SmtpClient()) {
    ui->setupUi(this);

    auto user = getUserInfo(this);

    connectToMailBox(user);
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

bool isPop3ResponseCorrect(const QString& response) {
    if (response.size() != 0) {
        const size_t len = 3;
        if (response.chopped(len).contains("+OK")) {
            return true;
        }
    }
    return false;
}
