#ifndef USERDATA_H
#define USERDATA_H

#include <QString>

struct UserData {
    UserData() = default;

    QString email;
    QString password;
    QString smtpServer;
    QString smtpPort;
    QString pop3Server;
    QString pop3Port;
};

#endif // USERDATA_H
