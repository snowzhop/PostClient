#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <SMTP/smtpClient.h>
#include <POP3/pop3Client.h>
#include <LoginDialog/logindialog.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void testMethod();

private:
    Ui::MainWindow *ui;
    POP3Client* pop3Client = nullptr;
    SmtpClient* smtpClient = nullptr;
};

#endif // MAINWINDOW_H
