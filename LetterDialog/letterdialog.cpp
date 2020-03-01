#include "letterdialog.h"
#include "PostClient/PostClient.h"
#include "Base64/base64util.h"
#include "POP3/pop3Client.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
//#include <QWebEngineView>
#include <QStringRef>
#include <QDebug>

#include <cstring>
#include <iostream>

LetterDialog::LetterDialog(QWidget* parent) :
    QMainWindow (parent),
    centralWidget(new QWidget(this)),
    dateLine(new QLineEdit(this)),
    fromLine(new QLineEdit(this)),
    webView(new QWebEngineView) {

    this->setCentralWidget(centralWidget);
    this->setAttribute(Qt::WA_DeleteOnClose);

    QLabel* dateLabel   = new QLabel(QString("Дата:"), this);
    QLabel* fromLabel   = new QLabel(QString("От кого:"), this);

    dateLine->setReadOnly(true);
    fromLine->setReadOnly(true);

    QVBoxLayout* mainLayout     = new QVBoxLayout(centralWidget);
    QVBoxLayout* dataLayout     = new QVBoxLayout();
    QHBoxLayout* dateLayout     = new QHBoxLayout();
    QHBoxLayout* fromLayout     = new QHBoxLayout();

    dateLayout->addWidget(dateLabel);
    dateLayout->addWidget(dateLine);

    fromLayout->addWidget(fromLabel);
    fromLayout->addWidget(fromLine);

    dataLayout->addLayout(fromLayout);
    dataLayout->addLayout(dateLayout);
    dataLayout->setSizeConstraint(QLayout::SetFixedSize);

    mainLayout->addLayout(dataLayout);
    mainLayout->addWidget(webView);

    webView->setHtml("Hello there");
}

LetterDialog::~LetterDialog() {
    delete dateLine;
    delete fromLine;
    delete webView;
}

void LetterDialog::showLetter(POP3Client& pop3Client, const int& letterNumber) {
    qDebug() << "letterNumber: " << letterNumber;
    this->show();
    char* response = pop3Client.sendRequest(std::string("RETR ").
                                            append(std::to_string(letterNumber)).
                                            append("\r\n"));

    if (PostClient::isPop3ResponseCorrect(QString(response))) {
        std::string strResponse(response);
        delete[] response;

        auto beginPos = strResponse.find("Date: ") + 6;
        auto endPos = strResponse.find("\r\n", beginPos);

        dateLine->setText(QString(strResponse.substr(beginPos, endPos - beginPos).c_str()));

        beginPos = strResponse.find("From: ") + 6;

        if (strResponse[beginPos] == '=') {
            qDebug() << "Encoding";
            for (int i = 0; i < 3; ++i) {
                beginPos = strResponse.find("?", beginPos) + 1;
            }
            auto endPos = strResponse.find("?", beginPos);
            if (strResponse[beginPos - 2] == 'B' || strResponse[beginPos - 2] == 'b') {
                qDebug() << "Base64";
                std::string result;
                result.append(QByteArray::fromBase64(
                                  strResponse.substr(beginPos, endPos - beginPos).c_str()));  // Added name

                beginPos = strResponse.find("<", endPos) - 1;
                endPos = strResponse.find(">", beginPos) + 1;
                result.append(strResponse.substr(beginPos, endPos - beginPos).c_str());  // Added email

                fromLine->setText(QString(result.c_str()));
            } else if (strResponse[beginPos - 1] == 'Q' || strResponse[beginPos - 1] == 'q') {
                qDebug() << "Quoted-printable";
                fromLine->setText(strResponse.substr(beginPos, endPos - beginPos).c_str());
            }
        } else {
            auto endPos = strResponse.find("\r\n", beginPos);
            fromLine->setText(QString(strResponse.substr(beginPos, endPos - beginPos).c_str()));
        }

        beginPos = strResponse.find("boundary=\"");
        if (beginPos != std::string::npos) {
            beginPos +=  + std::strlen("boundary=\"");
            endPos = strResponse.find("\"", beginPos);
            std::string boundaryStr = strResponse.substr(beginPos, endPos - beginPos);
            qDebug() << "boundary = " << boundaryStr.c_str();

        } else {
            qDebug() << "Without boundary";

            beginPos = strResponse.find("Content-Type: ") + std::strlen("Content-Type: ");
            endPos = strResponse.find(";", beginPos);
            auto contentType = strResponse.substr(beginPos, endPos - beginPos);
            qDebug() << contentType.c_str();

            beginPos = strResponse.find("Content-Transfer-Encoding: ") + std::strlen("Content-Transfer-Encoding: ");
            endPos = strResponse.find("\r\n", beginPos);
            auto transferEncoding = strResponse.substr(beginPos, endPos - beginPos);

            if (QString(transferEncoding.c_str()).toLower() == "quoted-printable") {
                qDebug() << "quoted-printable";
            } else if (QString(transferEncoding.c_str()).toLower() == "base64") {
                qDebug() << "base64";
            } else {
                beginPos = strResponse.find("\r\n\r\n") + std::strlen("\r\n\r\n");
                endPos = strResponse.find("\r\n.\r\n", beginPos);
                auto result = strResponse.substr(beginPos, endPos - beginPos);
                webView->setHtml(QString(result.c_str()));
            }

        }
    } else {
        qDebug() << "Error response: " << response;
        delete[] response;
    }
}
