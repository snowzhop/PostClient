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

    bool ok = false;
    char* answer = nullptr;
    short port = user.pop3Port.toShort(&ok);
    if (ok) {
        answer = pop3Client->connectToServer(user.pop3Server.toUtf8().constData(), port);
        if (checkPop3Response(QString(answer))) {
            qDebug() << "+OK";
        } else {
            qDebug() << "-ERR";
        }
    }
    port = user.smtpPort.toShort(&ok);
    if (ok) {
        int res = smtpClient->connectToSMTPServer(user.smtpServer.toUtf8().constData(), port);
        qDebug() << "res = " << res;
    }
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

bool checkPop3Response(const QString& response) {
    const size_t len = 3;
    if (response.chopped(len).contains("+OK")) {
        return true;
    }
    return false;
}
