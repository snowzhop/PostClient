#ifndef POSTLIENT_H
#define POSTLIENT_H

#include <QString>
#include <QMainWindow>
#include <LoginDialog/userdata.h>

namespace PostClient {

QString* findSubject(const std::string& letter);
UserData getUserInfo(QMainWindow* w);
bool isPop3ResponseCorrect(const QString& response);

}

#endif // POSTLIENT_H
