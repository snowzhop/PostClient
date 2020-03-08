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
    bool closeConnection();
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
    static const int MAX_LETTER_SIZE = 1000000;

    int socketFd = -1;
    SSL *ssl = nullptr;
    char* boundary = nullptr;

    char username[MAX_EMAIL_LEN];
    char password[MAX_EMAIL_LEN];
    char mailTo[MAX_EMAIL_LEN];
    char letter[MAX_LETTER_SIZE];

    bool letterEnded = false;
    int attachmentId = 0;

    static int recvStatus(const char* recvString);

    ssize_t sendData(const char* data, size_t size);
    ssize_t recvData(char* data, size_t size);

};

static char base64_encoding_table[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
                                       'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
                                       'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
                                       'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
                                       'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
                                       'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
                                       'w', 'x', 'y', 'z', '0', '1', '2', '3',
                                       '4', '5', '6', '7', '8', '9', '+', '/'
                                      };
static size_t base64_mod_table[] = {0, 2, 1};

char* base64_encode(const char *data, size_t input_length);

#endif //MMIVAS_SMTPCLIENT_H
