#ifndef MMIVAS_SMTPCLIENT_H
#define MMIVAS_SMTPCLIENT_H

#include <openssl/ssl.h>
//#include <openssl/bio.h>
//#include <openssl/err.h>

//namespace SMTP {
//    enum class Status : int {
//        OK = 0,
//        SYS_ERR = -1,
//        HOST_NAME_RESOLVE_ERR = -2,
//        SOCK_ERR = -3,
//        CONN_ERR = -4,
//        GETSOCKOPT_ERR = -5,
//        DELAYED_CONN_ERR = -6,
//        TIMEOUT = -7,
//        SSL_ERR = -8,
//        SSL_CONN_ERR = -9
//    };
//}

class SmtpClient {
public:
    SmtpClient();
    ~SmtpClient();
    void connectToSMTPServer(const char* smtpHostName, const short smtpPort);
    void login(const char* smtpLogin, const char* smtpPassword);
    size_t createLetter(
        const char* toMail,
        const char* letterSubject,
        const char* letterText);
    int endLetter();
    int addAttachment(const char* filePath);


    int sendLetter();

    int letterSize();

private:
    static const short SMTP_MTU = 1000;
    static const short MAX_EMAIL_LEN = 256;

    int socketFd = -1;
    SSL *ssl = nullptr;
    char* boundary = nullptr;

    char username[MAX_EMAIL_LEN];
    char password[MAX_EMAIL_LEN];
    char mailTo[MAX_EMAIL_LEN];
    char letter[SMTP_MTU];

    bool letterEnded = false;
    int attachmentId = 0;

    static int recvStatus(const char* recvString);

    ssize_t sendData(const char* data, size_t size);
    ssize_t recvData(char* data, size_t size);

};


#endif //MMIVAS_SMTPCLIENT_H
