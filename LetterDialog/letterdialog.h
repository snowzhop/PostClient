#ifndef LETTERDIALOG_H
#define LETTERDIALOG_H

#include <QDialog>
#include <QMainWindow>
#include <QLineEdit>
#include <QWebEngineView>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QToolBar>

class POP3Client;

class LetterDialog : public QMainWindow {
    Q_OBJECT
public:
    LetterDialog(QWidget* parent = nullptr);
    virtual ~LetterDialog();

    void showLetter(const std::string& letter, const int& letterNumber);

private:
    QWidget* centralWidget      = nullptr;
    QLineEdit* dateLine         = nullptr;
    QLineEdit* fromLine         = nullptr;
    QToolBar* toolBar           = nullptr;
    QPushButton* replyButton    = nullptr;

    QVBoxLayout* mainLayout     = nullptr;

    QTextEdit* createTextEditView(const std::string& text);
    QWebEngineView* createWebView(const std::string& text);
    QPushButton* createAttachmentPushButton(const std::string& text);

    std::string getQuotedPrintableDecodedPlainLetter(const std::string& letter);
    std::string getQuotedPrintableDecodedHtmlLetter(const std::string& letter);

    int letterNumber;

signals:
    void deleteLetterSignal(const int& letterNumber);
    void replySignal(const std::string& email);
};


#endif // LETTERDIALOG_H
