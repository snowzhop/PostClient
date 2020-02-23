#include <netinet/in.h>
#include <netdb.h>

#include <fcntl.h>

#include <iostream>
#include <cstring>
#include <unistd.h>

#include "pop3Client.h"
#include "Base64/base64util.h"

POP3Client::POP3Client() {
    SSL_library_init();
    ERR_load_BIO_strings();
    SSL_load_error_strings();
    SSLeay_add_ssl_algorithms();
    OpenSSL_add_all_algorithms();
}

char* POP3Client::connectToServer(const std::string& pop3ServerName, const short pop3Port) {
    sockaddr_in serverAddr;
    hostent *host = nullptr;

    if (!(host = gethostbyname(pop3ServerName.c_str()))) {
        std::cerr << "Can't resolve hostname " << pop3ServerName << std::endl;
        return nullptr;
    }

    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port   = htons(static_cast<uint16_t>(pop3Port));
    serverAddr.sin_addr   = *reinterpret_cast<in_addr*>(host->h_addr);

    socketFd = socket(PF_INET, SOCK_STREAM, 0);
    if (socketFd < 0) {
        std::cerr << "Can't initialize socket!" << std::endl;
        return nullptr;
    }
    std::cout << "SocketFd was initialized" << std::endl;

    long arg = 0;
    fd_set setOfConnections;
    timeval tv;
    int valopt;
    socklen_t len = 0;

    if ( (arg = fcntl(socketFd, F_GETFL, nullptr)) < 0 ) {
        std::cerr << "Error fcntl(..., F_GETFL): " << strerror(errno) << std::endl;
        return nullptr;
    }

    arg |= O_NONBLOCK;
    if (fcntl(socketFd, F_SETFL, arg) < 0) {
        std::cerr << "Error fcntl(..., F_SETFL): " << strerror(errno) << std::endl;
        return nullptr;
    }

    if (connect(socketFd, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(sockaddr)) < 0) {
        if (errno == EINPROGRESS) {
            std::cout << "EINPROGRESS in connect() - selecting" << std::endl;
            do {
                tv.tv_sec = 7;
                tv.tv_usec = 0;
                FD_ZERO(&setOfConnections);
                FD_SET(socketFd, &setOfConnections);
                auto result = select(socketFd+1, nullptr, &setOfConnections, nullptr, &tv);
                if (result < 0 && errno != EINTR) {
                    std::cerr << "Error connecting " << errno << " - " << strerror(errno) << std::endl;
                    return nullptr;
                } else if (result > 0) {
                    len = sizeof(int);
                    if (getsockopt(socketFd, SOL_SOCKET, SO_ERROR, reinterpret_cast<void*>(&valopt), &len) < 0) {
                        std::cerr << "Error in getsockopt() " << errno << " - " << strerror(errno) << std::endl;
                        return nullptr;
                    }

                    if (valopt) {
                        std::cerr << "Error in delayed connection() " << valopt << " - " << strerror(valopt) << std::endl;
                        return nullptr;
                    }
                    break;
                } else {
                    std::cerr << "Timout in select was cancelling!" << std::endl;
                    std::cerr << "Check it with normal connection" << std::endl;
                    return nullptr;
                }
            } while(1);
        } else {
            std::cerr << "Error connection " << errno << " - " << strerror(errno) << std::endl;
            return nullptr;
        }
    }

    if ( (arg = fcntl(socketFd, F_GETFL, nullptr)) < 0 ) {
        std::cerr << "Error fcntl(..., F_GETFL)2: " << strerror(errno) << std::endl;
        return nullptr;
    }
    arg &= (~O_NONBLOCK);
    if (fcntl(socketFd, F_SETFL, arg) < 0) {
        std::cerr << "Error fcntl(..., F_SETFL)2: " << strerror(errno) << std::endl;
        return nullptr;
    }

//    connect() without timeout
//    if (connect(socketFd, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(sockaddr)) < 0) {
//        close(socketFd);
//        socketFd = -1;
//        std::cerr << "Can't connect to host " << pop3ServerName << ":" << pop3Port << std::endl;
//        return nullptr;
//    }

    const SSL_METHOD *method = nullptr;
    SSL_CTX* ctx = nullptr;
//    int socketSSL = -1;

    method = TLS_client_method();
    ctx = SSL_CTX_new(method);
    ssl = SSL_new(ctx);

    if (!ssl) {
        close(socketFd);
        socketFd = -1;
        std::cerr << "Error of SSL connection!" << std::endl;
        return nullptr;
    }
    std::cout << "SSL was created." << std::endl;

    SSL_set_fd(ssl, socketFd);

    if(SSL_connect(ssl) < 0) {
        std::cerr << "Error creating SSL connection!" << std::endl;
        SSL_free(ssl);
        ssl = nullptr;
        close(socketFd);
        socketFd = -1;
        return nullptr;
    }
    std::cout << "SSL connection was created" << std::endl;

    char* serverAnswer = new char[256];
    SSLRecv(serverAnswer);

    return serverAnswer;
}

char* POP3Client::sendRequest(const std::string& request) {
    char* buffer = new char[_20_MB_BUFFER_SIZE];
    size_t size = request.length();
    int receivedSize = 0;
    std::strncpy(buffer, request.c_str(), size);
    SSLSend(buffer, size);

    memset(buffer, 0, _20_MB_BUFFER_SIZE);
    if (request == "LIST\r\n" ||
            (request[0] == 'R' && request[1] == 'E' && request[2] == 'T' && request[3] == 'R')) {
        receivedSize = SSLReadMultiLine(buffer);
    } else {
        receivedSize = SSLRecv(buffer);
    }

    return receivedSize ? buffer : nullptr;
}

int POP3Client::SSLSend(const char* buffer, const size_t size) {
    int len = SSL_write(ssl, buffer, static_cast<int>(size));
    if (len < 0) {
        int err = SSL_get_error(ssl, len);
        switch (err) {
        case SSL_ERROR_WANT_WRITE:
            return -1;
        case SSL_ERROR_WANT_READ:
            return -1;
        case SSL_ERROR_ZERO_RETURN: // Fall down
        case SSL_ERROR_SYSCALL:
        case SSL_ERROR_SSL:
        default:
            return -1;
        }
    }

    return len;
}

int POP3Client::SSLRecv(char* buffer) {
//    int len = SSL_read(ssl, buffer, size);
    std::string storage;
    char tmp[2] = "";
//    int len = 0;
    int total = 0;
    while (SSLReadChar(&(tmp[1])) && !(tmp[0] == '\r' && tmp[1] == '\n')) {
        total++;
        tmp[0] = tmp[1];
        storage += tmp[0];
    }
//    if (len < 0) {
//        int err = SSL_get_error(ssl, len);
//        switch (err) {
//            case SSL_ERROR_WANT_WRITE:
//                return 0;
//            case SSL_ERROR_WANT_READ:
//                return 0;
//            case SSL_ERROR_ZERO_RETURN: // Fall down
//            case SSL_ERROR_SYSCALL:
//            case SSL_ERROR_SSL:
//            default:
//                return -1;
//        }
//    }
    std::strcpy(buffer, storage.c_str());
    return total;
}

char* POP3Client::decodeResponse(const char* response) {
    char* decodedServerAnswer = new char[strlen(response)];
    size_t offset = 8;
    memcpy(decodedServerAnswer, response, offset);
    char* decodedAnswer = decodeBase64(response + offset);
    memcpy(decodedServerAnswer + offset, decodedAnswer, strlen(decodedAnswer));

    delete[] decodedAnswer;
    return decodedServerAnswer;
}

bool POP3Client::SSLReadChar(char* ch) {
    return SSL_read(ssl, ch, 1) >= 0;
}

int POP3Client::SSLReadMultiLine(char* buffer) {
    char* bufferChar = new char;
    int charCounter = 0;

    while (SSLReadChar(bufferChar)) {
        buffer[charCounter] = *bufferChar;
        if (charCounter > 3) {
            if (buffer[charCounter]     == '\n' &&  // 10
                    buffer[charCounter - 1] == '\r' &&  // 13
                    buffer[charCounter - 2] == '.'  &&
                    buffer[charCounter - 3] == '\n' &&
                    buffer[charCounter - 4] == '\r') {   // 46
                break;
            }
        }
        charCounter++;
    }

    return charCounter;
}
