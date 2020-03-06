#ifndef LETTERDIALOG_H
#define LETTERDIALOG_H

#include <QDialog>
#include <QMainWindow>
#include <QLineEdit>
#include <QWebEngineView>
#include <QVBoxLayout>
#include <QTextEdit>

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

    QVBoxLayout* mainLayout = nullptr;

    QTextEdit* createTextEditView(const std::string& text);
    QWebEngineView* createWebView(const std::string& text);

    std::string getQuotedPrintableDecodedPlainLetter(const std::string& letter);
    std::string getQuotedPrintableDecodedHtmlLetter(const std::string& letter);
};


#endif // LETTERDIALOG_H
