#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

struct UserInfo {
    QString email;
    QString password;
};

UserInfo getUserInfo(QMainWindow* w) {
    LoginDialog loginDialog(w);
    loginDialog.setModal(true);

    UserInfo user;
    QObject::connect(&loginDialog, &LoginDialog::fieldsSaved,
    [&user](const QString& em, const QString& pass) {
        user.email = em;
        user.password = pass;
    });

    loginDialog.exec();

    return user;
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    pop3Client(new POP3Client()),
    smtpClient(new SmtpClient()) {
    ui->setupUi(this);

    auto user = getUserInfo(this);

    qDebug() << user.email << "\t" << user.password;
}

MainWindow::~MainWindow() {
    delete ui;
    delete pop3Client;
    delete smtpClient;
}

void MainWindow::testMethod() {

}
