#include "letterdialog.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWebEngineView>
#include <QStringRef>

#include <cstring>

LetterDialog::LetterDialog(QWidget* parent) :
    QMainWindow (parent),
    dateLine(new QLineEdit(this)),
    fromLine(new QLineEdit(this)),
    webView(new QWebEngineView) {

    QWidget* centralWidget = new QWidget(this);
    this->setCentralWidget(centralWidget);

    QLabel* dateLabel   = new QLabel(QString("Дата:"), this);
    QLabel* fromLabel   = new QLabel(QString("От кого:"), this);
//    QLabel* letterLabel = new QLabel("Letter:", this);

    dateLine->setReadOnly(true);
    fromLine->setReadOnly(true);

    QVBoxLayout* mainLayout     = new QVBoxLayout(centralWidget);
    QVBoxLayout* dataLayout     = new QVBoxLayout();
    QHBoxLayout* dateLayout     = new QHBoxLayout();
    QHBoxLayout* fromLayout     = new QHBoxLayout();
//    QVBoxLayout* letterLayout   = new QVBoxLayout();

    dateLayout->addWidget(dateLabel);
    dateLayout->addWidget(dateLine);

    fromLayout->addWidget(fromLabel);
    fromLayout->addWidget(fromLine);

//    letterLayout->addWidget(letterLabel);

    dataLayout->addLayout(fromLayout);
    dataLayout->addLayout(dateLayout);
//    dataLayout->addLayout(letterLayout);
    dataLayout->setSizeConstraint(QLayout::SetFixedSize);

    mainLayout->addLayout(dataLayout);
    mainLayout->addWidget(webView);

    webView->setHtml("Хаюшки, мазафака");
}

LetterDialog::~LetterDialog() {

}

void LetterDialog::showLetter(POP3Client& pop3Client, const int& letterNumber) {
    this->show();
    char* request = pop3Client.sendRequest(std::string("RETR ").
                                           append(std::to_string(letterNumber)).
                                           append("\r\n"));

    std::string_view strRequest(request);
    delete[] request;

    auto dateBeginPos = strRequest.find("Date: ") + 6;
    auto dateEndPos = strRequest.find("\r\n", dateBeginPos);

    std::string_view dateStr = strRequest.substr(dateBeginPos, dateEndPos - dateBeginPos);
    dateLine->setText(std::string(dateStr).c_str());

    auto fromBeginPos = strRequest.find("From: ") + 6;
    auto fromEndPos = strRequest.find("\r\n", fromBeginPos);

    if (strRequest[fromBeginPos] == '=') {
        qDebug() << "kodirovochka";
    }
}
