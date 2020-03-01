#ifndef LETTERDIALOG_H
#define LETTERDIALOG_H

#include <QDialog>
#include <QMainWindow>
#include <QLineEdit>
#include <QWebEngineView>

class POP3Client;

class LetterDialog : public QMainWindow {
    Q_OBJECT
public:
    LetterDialog(QWidget* parent = nullptr);
    virtual ~LetterDialog();

    void showLetter(POP3Client& pop3Client, const int& letterNumber);

private:
    QWidget* centralWidget = nullptr;
    QLineEdit* dateLine = nullptr;
    QLineEdit* fromLine = nullptr;

    QWebEngineView* webView = nullptr;
};

#endif // LETTERDIALOG_H
