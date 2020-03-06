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
#include <QStack>
#include <QTextEdit>
#include <QPushButton>

#include <QFile>
#include <QTextStream>

#include <cstring>
#include <iostream>

LetterDialog::LetterDialog(QWidget* parent) :
    QMainWindow (parent),
    centralWidget(new QWidget(this)),
    dateLine(new QLineEdit(this)),
    fromLine(new QLineEdit(this)),
    toolBar(new QToolBar(centralWidget)) {

    this->setCentralWidget(centralWidget);
    this->setAttribute(Qt::WA_DeleteOnClose);

    QLabel* dateLabel   = new QLabel(QString("Дата:"), this);
    QLabel* fromLabel   = new QLabel(QString("От кого:"), this);

    dateLine->setReadOnly(true);
    fromLine->setReadOnly(true);

    mainLayout                  = new QVBoxLayout(centralWidget);
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

    toolBar->setMovable(false);
    replyButton = new QPushButton("reply", toolBar);
    toolBar->addWidget(replyButton);
    toolBar->addSeparator();
    this->addToolBar(Qt::LeftToolBarArea, toolBar);

    mainLayout->addLayout(dataLayout);
    this->setMinimumSize(700, 500);
}

LetterDialog::~LetterDialog() {
    delete dateLine;
    delete fromLine;
    delete replyButton;
    delete mainLayout;
}

void LetterDialog::showLetter(POP3Client& pop3Client, const int& letterNumber) {
    qDebug() << "letterNumber: " << letterNumber;
    this->show();
    char* response = pop3Client.sendRequest(std::string("RETR ").
                                            append(std::to_string(letterNumber)).
                                            append("\r\n"));

    if (PostClient::isPop3ResponseCorrect(QString(response))) {
        std::string* strResponse = new std::string(response);
        delete[] response;

        auto endOfHeader = strResponse->find("\r\n\r\n");
        std::string header = strResponse->substr(0, endOfHeader);
        std::string letter = strResponse->substr(endOfHeader + 4);
        delete strResponse;

        // Date parsing
        auto beginPos = header.find("Date: ") + 6;
        auto endPos = header.find("\r\n", beginPos);

        dateLine->setText(QString(header.substr(beginPos, endPos - beginPos).c_str()));

        // "From" parsing
        beginPos = header.find("From: ") + 6;

        if (header[beginPos] == '=') {
            qDebug() << "Encoding";
            for (int i = 0; i < 3; ++i) {
                beginPos = header.find("?", beginPos) + 1;
            }
            auto endPos = header.find("?", beginPos);
            if (header[beginPos - 2] == 'B' || header[beginPos - 2] == 'b') {
                qDebug() << "Base64";
                std::string result;
                result.append(QByteArray::fromBase64(
                                  header.substr(beginPos, endPos - beginPos).c_str()));  // Added name

                beginPos = header.find("<", endPos) - 1;
                endPos = header.find(">", beginPos) + 1;
                result.append(header.substr(beginPos, endPos - beginPos).c_str());  // Added email

                fromLine->setText(QString(result.c_str()));
            } else if (header[beginPos - 1] == 'Q' || header[beginPos - 1] == 'q') {
                qDebug() << "Quoted-printable";
                fromLine->setText(header.substr(beginPos, endPos - beginPos).c_str());
            }
        } else {
            auto endPos = header.find("\r\n", beginPos);
            fromLine->setText(QString(header.substr(beginPos, endPos - beginPos).c_str()));
        }

        // Main letter parsing
        beginPos = header.find("Content-Type: ") + std::strlen("Content-Type: ");
        endPos = header.find("/", beginPos);
        if (header.substr(beginPos, endPos - beginPos) == "multipart") {
            qDebug() << "Multipart";

            if ((beginPos = letter.find("Content-Type: text/html;")) != std::string::npos) {
                beginPos = letter.find("Content-Transfer-Encoding: ", beginPos) + std::strlen("Content-Transfer-Encoding: ");
                endPos = letter.find("\r\n", beginPos);
                QString transferEncoding = QString(letter.substr(beginPos, endPos - beginPos).c_str()).toLower();

                beginPos = letter.find("\r\n\r\n", beginPos) + 4;
                endPos = letter.find("\r\n\r\n", beginPos);

                if (transferEncoding == "base64") {
                    qDebug() << "base64";

                    auto* view = createWebView(QByteArray::fromBase64(
                                                   letter.substr(beginPos, endPos - beginPos).c_str()).toStdString());

                    mainLayout->addWidget(view);

                } else if (transferEncoding == "quoted-printable") {
                    qDebug() << "quoted-printable";

                    mainLayout->addWidget(
                        createWebView(
                            getQuotedPrintableDecodedHtmlLetter(letter.substr(beginPos, endPos - beginPos))));

                } else {
                    qDebug() << "Without encoding";
                    mainLayout->addWidget(createWebView(letter.substr(beginPos, endPos - beginPos)));
                }
            } else if ((beginPos = letter.find("Content-Type: text/plain;")) != std::string::npos) {
                beginPos = letter.find("Content-Transfer-Encoding: ", beginPos) + std::strlen("Content-Transfer-Encoding: ");
                endPos = letter.find("\r\n", beginPos);
                QString transferEncoding = QString(letter.substr(beginPos, endPos - beginPos).c_str()).toLower();

                beginPos = letter.find("\r\n\r\n", beginPos) + 4;
                endPos = letter.find("\r\n\r\n", beginPos);

                if (transferEncoding == "base64") {
                    qDebug() << "base64";

                    auto* view = createTextEditView(QByteArray::fromBase64(
                                                        letter.substr(beginPos, endPos - beginPos).c_str()).toStdString());

                    mainLayout->addWidget(view);
                } else if (transferEncoding == "quoted-printable") {
                    qDebug() << "quoted-printable";

                    auto text = getQuotedPrintableDecodedPlainLetter(letter.substr(beginPos, endPos - beginPos));
                    mainLayout->addWidget(createTextEditView(text));
                } else {
                    qDebug() << "Without encoding";

                    mainLayout->addWidget(createTextEditView(letter.substr(beginPos, endPos - beginPos)));
                }
            }

        } else {
            qDebug() << "Not multipart";

            beginPos = header.find("Content-Transfer-Encoding: ") + std::strlen("Content-Transfer-Encoding: ");
            endPos = header.find("\r\n", beginPos);
            QString transferEncoding = QString(header.substr(beginPos, endPos - beginPos).c_str()).toLower();

            if (transferEncoding == "base64") {
                qDebug() << "base64";

                if (header.find("Content-Type: text/plain") != std::string::npos) {
                    qDebug() << "text/plain";
                    mainLayout->addWidget(
                        createTextEditView(
                            std::string(QByteArray::fromBase64(letter.substr(0, letter.find("\r\n.\r\n")).c_str()))));

                }
            } else if (transferEncoding == "quoted-printable") {
                qDebug() << "quoted-printable";
                std::string result;

                if (header.find("Content-Type: text/plain") != std::string::npos) {
                    qDebug() << "text/plain";
                    result = getQuotedPrintableDecodedPlainLetter(letter.substr(0, letter.find("\r\n.\r\n")));

                    mainLayout->addWidget(createTextEditView(result));
                } else {
                    qDebug() << "text/html";
                    result = letter.substr(0, letter.find("\r\n.\r\n"));

                    mainLayout->addWidget(createWebView(result));
                }
            } else {
                QWebEngineView* view = new QWebEngineView(this);
                view->setHtml(QString( letter.substr(0, letter.find("\r\n.\r\n")).c_str() ));

                mainLayout->addWidget(view);
            }

        }

        // Attachment

        for (beginPos = letter.find("Content-Disposition: attachment");
                beginPos != std::string::npos;
                beginPos = letter.find("Content-Disposition: attachment", endPos)) {

            beginPos = letter.find("filename=\"", beginPos) + std::strlen("filename=\"");
            endPos = letter.find("\"", beginPos);
            std::string filename = letter.substr(beginPos, endPos - beginPos);
            if (filename[0] == '=') {
                qDebug() << "Encoded filename";
                size_t filenameBeginPos = 0;
                for (int i = 0; i < 3; ++i) {
                    filenameBeginPos = filename.find("?", filenameBeginPos);
                    ++filenameBeginPos;
                }
                if (filename[filenameBeginPos-2] == 'B' || filename[filenameBeginPos-2] == 'b') {
                    size_t filenameEndPos = filename.find("?=", filenameBeginPos);
                    filename = QByteArray::fromBase64(filename.substr(filenameBeginPos, filenameEndPos - filenameBeginPos).c_str()).toStdString();
                }
            } else {
                qDebug() << "Not encoded filename";
            }

        }
    } else {
        qDebug() << "Error response: " << response;
        delete[] response;
    }
}

QTextEdit* LetterDialog::createTextEditView(const std::string &text) {
    QTextEdit* result = new QTextEdit(this);
    result->setText(text.c_str());
    result->setReadOnly(true);
    return result;
}

QWebEngineView* LetterDialog::createWebView(const std::string &text) {
    QWebEngineView* result = new QWebEngineView(this);
    result->setHtml(text.c_str());
    return result;
}

QPushButton* LetterDialog::createAttachmentPushButton(const std::string& filename) {
    QPushButton* result = new QPushButton(filename.c_str(), this);



    return result;
}

std::string LetterDialog::getQuotedPrintableDecodedPlainLetter(const std::string& letter) {
    std::string result;
    QByteArray tmpStr;
    for (size_t i = 0; i < letter.length();) {
        if (letter[i] == '=') {
            tmpStr.append(letter[++i]);
            tmpStr.append(letter[++i]);
            result.append(QByteArray::fromHex(tmpStr));
            tmpStr.clear();
        } else {
            result.append(1, letter[i]);
        }
        ++i;
    }
    return result;
}

std::string LetterDialog::getQuotedPrintableDecodedHtmlLetter(const std::string& letter) {
    std::string result = "";
    QByteArray tmpStr;
    bool encoding = [&letter]() {
        return letter[0] != '<';
    }
    ();

    for (size_t i = 0; i < letter.length();) {
        if (encoding) {
            if (letter[i] == '<') {
                encoding = false;
                result.append(1, letter[i]);
            } else if (letter[i] == '=') {
                tmpStr.append(letter[++i]);
                tmpStr.append(letter[++i]);
                result.append(QByteArray::fromHex(tmpStr));
                tmpStr.clear();
            } else {
                result.append(1, letter[i]);
            }
        } else {
            result.append(1, letter[i]);
            if (letter[i] == '>') {
                encoding = true;
            }
        }
        ++i;
    }

    return result;
}
