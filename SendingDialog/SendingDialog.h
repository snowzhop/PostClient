#ifndef SENDING_DIALOG_H
#define SENDING_DIALOG_H

#include <QMainWindow>
#include <QLineEdit>
#include <QTextEdit>
#include <SMTP/smtpClient.h>

class SendingDialog : public QMainWindow {
    Q_OBJECT
public:
    SendingDialog(QWidget* parent = nullptr);
    virtual ~SendingDialog();

    void setEmail(const std::string& email);

private:
    QWidget* centralWidget          = nullptr;
    QLineEdit* receiverEmailEdit    = nullptr;
    QLineEdit* subjectEdit          = nullptr;
    QTextEdit* letterEdit           = nullptr;
    QToolBar* toolBar               = nullptr;

    std::string attachmentPath;

public slots:
    void letterPreparing();
    void finishLetter();

signals:
    void compileLetter(std::string email, std::string subject, std::string text, std::string attachmentPath);

private slots:
    void addAttachment();
};

#endif // SENDING_DIALOG_H
