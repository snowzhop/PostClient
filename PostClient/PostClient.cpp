#include "PostClient.h"
#include "LetterDialog/letterdialog.h"
#include "Base64/base64util.h"
#include "mainwindow.h"

#include <iostream>
#include <cstring>

namespace PostClient {

QString* findSubject(const std::string& letter) {
    std::string::size_type subjectCurrentPos = letter.find("\r\nSubject: ") + std::strlen("\r\nSubject: ");

    QString* result = new QString();

    if (letter[subjectCurrentPos] == '=') {
        while (true) {
            ++subjectCurrentPos;
            for (int i = 0; i < 3; ++i) {
                subjectCurrentPos = letter.find("?", subjectCurrentPos) + 1;
            }

            std::string::size_type subjectEndPos = letter.find("?", subjectCurrentPos);
            std::string tmpSubject(letter.substr(subjectCurrentPos, subjectEndPos - subjectCurrentPos));

            if (letter[subjectCurrentPos-MainWindow::codingOffset] == 'b' ||
                    letter[subjectCurrentPos-MainWindow::codingOffset] == 'B') {
                std::cout << tmpSubject << "|B|B|";
                result->append(decodeBase64(tmpSubject.c_str()));
            } else if (letter[subjectCurrentPos-MainWindow::codingOffset] == 'q' ||
                       letter[subjectCurrentPos-MainWindow::codingOffset] == 'Q') {
//                std::cout << tmpSubject << "|Q|Q|";
                QByteArray tmpStr;
                bool encoded = false;
                for (size_t i = 0; i < tmpSubject.length(); ++i) {
                    if (encoded) {
                        if (tmpSubject[i] == '_') {
                            std::cout << " ";
                            result->append(QString(QByteArray::fromHex(tmpStr)));
                            result->append(" ");
                            tmpStr.clear();
                            encoded = false;
                        } else {
                            std::cout << tmpSubject[i];
                            tmpStr.append(tmpSubject[i]);
                        }
                    } else {
                        if (tmpSubject[i] == '=') {
                            encoded = true;
                        } else {
                            result->append(tmpSubject[i]);
                        }
                    }
                }
                if (tmpStr.size() != 0) {
                    result->append(QString(QByteArray::fromHex(tmpStr)));
                }
                std::cout << "|Q|Q|";
            }
            subjectCurrentPos = subjectEndPos + 4;
            if (letter[subjectCurrentPos] == ' ' &&
                    letter[subjectCurrentPos-1] == '\n' &&
                    letter[subjectCurrentPos-2] == '\r') {
                continue;
            } else {
                break;
            }
        }

    } else {
        std::string::size_type subjectEndPos = letter.find("\r\n", subjectCurrentPos);
        auto tmpSubject = letter.substr(subjectCurrentPos, subjectEndPos - subjectCurrentPos).c_str();
        std::cout << tmpSubject;
        result->append(tmpSubject);
    }
    std::cout << std::endl;
    return result;
}

UserData getUserInfo(QMainWindow* w) {
    LoginDialog loginDialog(w);
    loginDialog.setModal(true);

    UserData user;
    QObject::connect(&loginDialog, &LoginDialog::fieldsSaved,
    [&user](const UserData& d) {
        user = d;
    });

    loginDialog.exec();

    return user;
}

bool isPop3ResponseCorrect(const QString& response) {
    if (response.size() != 0) {
        const size_t len = 3;
        if (response.chopped(len).contains("+OK")) {
            return true;
        }
    }
    return false;
}

}
