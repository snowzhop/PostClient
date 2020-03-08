#ifndef SENDING_DIALOG_H
#define SENDING_DIALOG_H

#include <QMainWindow>
#include <QLineEdit>
#include <QTextEdit>

class SendingDialog : public QMainWindow {
    Q_OBJECT
public:
    SendingDialog(QWidget* parent = nullptr);
    virtual ~SendingDialog();

private:
    QWidget* centralWidget          = nullptr;
    QLineEdit* receiverEmailEdit    = nullptr;
    QLineEdit* subjectEdit          = nullptr;
    QTextEdit* letterEdit           = nullptr;
    QToolBar* toolBar               = nullptr;

    QByteArray attachment;

private slots:
    void sendLetter();
    void addAttachment();
};

#endif // SENDING_DIALOG_H
