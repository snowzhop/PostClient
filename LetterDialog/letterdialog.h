#ifndef LETTERDIALOG_H
#define LETTERDIALOG_H

#include "POP3/pop3Client.h"

#include <QDialog>
#include <QMainWindow>
#include <QLineEdit>
#include <QWebEngineView>

class LetterDialog : public QMainWindow {
    Q_OBJECT
public:
    LetterDialog(QWidget* parent = nullptr);
    virtual ~LetterDialog();

    void showLetter(POP3Client& pop3Client, const int& letterNumber);

private:
    QLineEdit* dateLine = nullptr;
    QLineEdit* fromLine = nullptr;

    QWebEngineView* webView = nullptr;
};

#endif // LETTERDIALOG_H
