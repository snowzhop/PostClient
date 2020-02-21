#include "mainwindow.h"
#include "LoginDialog/userdata.h"
#include "ui_mainwindow.h"
#include <QDebug>

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

MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    pop3Client(new POP3Client()),
    smtpClient(new SmtpClient()) {
    ui->setupUi(this);

    auto user = getUserInfo(this);

    bool ok;
    char* res = nullptr;
    short port = user.pop3Port.toShort(&ok);
    if (ok) {
        res = pop3Client->connectToServer(user.pop3Server.toUtf8().constData(), port);
    }
    if (res) {
        qDebug() << "Done?\n" << res;
    }
}

MainWindow::~MainWindow() {
    delete ui;
    delete pop3Client;
    delete smtpClient;
}

void MainWindow::testMethod() {

}
