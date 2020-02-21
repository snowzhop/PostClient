#ifndef MMIVAS_SMTPCLIENT_H
#define MMIVAS_SMTPCLIENT_H

#include <openssl/ssl.h>
//#include <openssl/bio.h>
//#include <openssl/err.h>

class SmtpClient {
public:
    SmtpClient();
    ~SmtpClient();
    int connectToSMTPServer(const char* smtpHostName, const short smtpPort);
    int login(const char* smtpLogin, const char* smtpPassword);
    int createLetter(
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
