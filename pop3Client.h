#ifndef MMIVAS_POP3CLIENT_H
#define MMIVAS_POP3CLIENT_H

#include <string>
#include <openssl/ssl.h>

class POP3Client {
public:
    POP3Client();
    
    char* connectToServer(const std::string& serverName, const short pop3Port);
    char* sendRequest(const std::string& request);
    static char* decodeResponse(const char* response);
    
private:
    const int _20_MB_BUFFER_SIZE = 20971520;
    int SSLSend(const char* buffer, const size_t size);
    int SSLRecv(char* buffer, size_t size);
    bool SSLReadChar(char* ch);
    int SSLReadMultiLine(char* buffer);
    SSL* ssl;
    int socketFd = -1;
};


#endif //MMIVAS_POP3CLIENT_H
