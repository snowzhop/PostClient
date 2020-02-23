#include <netinet/in.h>
#include <netdb.h>

#include <fcntl.h>

#include <iostream>
#include <fstream>
#include <unistd.h>
#include <cstring>

#include "smtpClient.h"
#include "Base64/base64util.h"

#define BASE64_SIZE(x)  (((x)+2) / 3 * 4 + 1)

SmtpClient::SmtpClient() {
    SSL_library_init();
    ERR_load_BIO_strings();
    SSL_load_error_strings();
    SSLeay_add_ssl_algorithms();
    OpenSSL_add_all_algorithms();
    boundary = encodeBase64("--------SIMPLEBOUNDARY");
}

SmtpClient::~SmtpClient() {
    delete[] boundary;
    SSL_free(ssl);
}

int SmtpClient::recvStatus(const char* recvString) {
    char statusStr[4];
    int reply = -1;

    memset(&statusStr, 0, sizeof(statusStr));
    std::strncpy(statusStr, recvString, 3);
    reply = std::atoi(statusStr);

    switch(reply) {
    case 250:
    case 235:
    case 354:
    case 334:
    case 221:
        break; // Fall down
    default: {
        std::cerr << "Received error status: " << reply << std::endl;
        return -1;
    }
    }

    return 0;
}

ssize_t SmtpClient::sendData(const char* data, size_t size) {
    int len = SSL_write(ssl, data, static_cast<int>(size));
    if (len < 0) {
        int err = SSL_get_error(ssl, len);
        switch (err) {
        case SSL_ERROR_WANT_WRITE:
            return 0;
        case SSL_ERROR_WANT_READ:
            return 0;
        case SSL_ERROR_ZERO_RETURN: // Fall down
        case SSL_ERROR_SYSCALL:
        case SSL_ERROR_SSL:
        default:
            return -1;
        }
    }

    return len;
}

ssize_t SmtpClient::recvData(char* data, size_t size) {
    int len = SSL_read(ssl, data, static_cast<int>(size));
    if (len < 0) {
        int err = SSL_get_error(ssl, len);
        switch (err) {
        case SSL_ERROR_WANT_WRITE:
            return 0;
        case SSL_ERROR_WANT_READ:
            return 0;
        case SSL_ERROR_ZERO_RETURN: // Fall down
        case SSL_ERROR_SYSCALL:
        case SSL_ERROR_SSL:
        default:
            return -1;
        }
    }

    return len;
}

int SmtpClient::connectToSMTPServer(const char* smtpHostName, const short smtpPort) {
    sockaddr_in smtpAddr;
    hostent *host = nullptr;

    if (!(host = gethostbyname(smtpHostName))) {
        std::cerr << "Can't resolve hostname " << smtpHostName << std::endl;
//        return -1;
        throw std::runtime_error(std::string("Can't resolve hostname ").append(smtpHostName));
    }

    memset(&smtpAddr, 0, sizeof(smtpAddr));
    smtpAddr.sin_family = AF_INET;
    smtpAddr.sin_port   = htons(static_cast<uint16_t>(smtpPort));
//    smtpAddr.sin_addr   = *((struct in_addr *)host->h_addr_list[0]); TODO Check
    smtpAddr.sin_addr   = *reinterpret_cast<in_addr*>(host->h_addr);

    socketFd = socket(PF_INET, SOCK_STREAM, 0);
    if (socketFd < 0) {
        std::cerr << "Can't initialize socket!" << std::endl;
//        return -1;
        throw std::runtime_error("Can't initialize socket!");
    }
    std::cout << "\nConnect to SMTP server>> Socket was initialized" << std::endl;

    long arg = 0;
    fd_set setOfConnections;
    timeval tv;
    int valopt;
    socklen_t len = 0;

    if ( (arg = fcntl(socketFd, F_GETFL, nullptr)) < 0 ) {
        std::cerr << "Error fcntl(..., F_GETFL): " << strerror(errno) << std::endl;
        throw std::runtime_error(std::string("Error fcntl(..., F_GETFL): ").append(strerror(errno)));
    }

    arg |= O_NONBLOCK;
    if (fcntl(socketFd, F_SETFL, arg) < 0) {
        std::cerr << "Error fcntl(..., F_SETFL): " << strerror(errno) << std::endl;
        throw std::runtime_error(std::string("Error fcntl(..., F_SETFL): ").append(strerror(errno)));
    }

    if (connect(socketFd, reinterpret_cast<sockaddr*>(&smtpAddr), sizeof(sockaddr)) < 0) {
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
                    throw std::runtime_error(std::string("Error connecting: ").append(strerror(errno)));
                } else if (result > 0) {
                    len = sizeof(int);
                    if (getsockopt(socketFd, SOL_SOCKET, SO_ERROR, reinterpret_cast<void*>(&valopt), &len) < 0) {
                        std::cerr << "Error in getsockopt() " << errno << " - " << strerror(errno) << std::endl;
                        throw std::runtime_error(std::string("Error in getsockopt(): ").append(strerror(errno)));
                    }

                    if (valopt) {
                        std::cerr << "Error in delayed connection() " << valopt << " - " << strerror(valopt) << std::endl;
                        throw std::runtime_error(std::string("Error in delayed connection(): ").append(strerror(errno)));
                    }
                    break;
                } else {
                    std::cerr << "Timout in select was cancelling!" << std::endl;
                    std::cerr << "Check it with normal connection" << std::endl;
                    throw std::runtime_error("Timout is select was cancelling!");
                }
            } while(1);
        } else {
            std::cerr << "Error connection " << errno << " - " << strerror(errno) << std::endl;
            throw std::runtime_error(std::string("Error connection: ").append(strerror(errno)));
        }
    }

    if ( (arg = fcntl(socketFd, F_GETFL, nullptr)) < 0 ) {
        std::cerr << "Error fcntl(..., F_GETFL)2: " << strerror(errno) << std::endl;
        throw std::runtime_error(std::string("Error fcntl(..., F_GETFL): ").append(strerror(errno)));
    }
    arg &= (~O_NONBLOCK);
    if (fcntl(socketFd, F_SETFL, arg) < 0) {
        std::cerr << "Error fcntl(..., F_SETFL)2: " << strerror(errno) << std::endl;
        throw std::runtime_error(std::string("Error fcntl(..., F_SETFL): ").append(strerror(errno)));
    }

//    connect() without timeout
//    if (connect(socketFd, reinterpret_cast<sockaddr*>(&smtpAddr), sizeof(sockaddr)) < 0) {
//        close(socketFd);
//        socketFd = -1;
//        std::cerr << "Can't connect to host " << smtpHostName << ":" << smtpPort << std::endl;
//        return -1;
//    }
    std::cout << "Connect to SMTP server>> Connected to host" << std::endl;

    const SSL_METHOD *method = nullptr;
    SSL_CTX* ctx = nullptr;
//    int socketSSL = -1;

    method = TLS_client_method(); // TLS_client_method
    ctx = SSL_CTX_new(method);
    ssl = SSL_new(ctx);

    if (!ssl) {
        close(socketFd);
        socketFd = -1;
        std::cerr << "Error of creating SSL struct!" << std::endl;
        throw std::runtime_error("Error of creating SSL struct!");
    }
    std::cout << "Connect to SMTP server>> SSL was created" << std::endl;

//    socketSSL = SSL_get_fd(ssl);
    SSL_set_fd(ssl, socketFd);

    if(SSL_connect(ssl) < 0) {
        std::cerr << "Error of SSL connection!" << std::endl;
        SSL_free(ssl);
        ssl = nullptr;
        close(socketFd);
        socketFd = -1;
        throw std::runtime_error("Error of SSL connection!");
    }
    std::cout << "Connect to SMTP server>> SSL connection was created" << std::endl;

    return 0;
}

int SmtpClient::login(const char* smtpLogin, const char* smtpPassword) {
    char readData[SMTP_MTU];
    char writeData[SMTP_MTU];

    memset(&readData, 0, SMTP_MTU);
    recvData(readData, SMTP_MTU);
    std::cout << "\nlogin>> " << readData << std::endl;

    // Send: EHLO
    sendData("EHLO Here\r\n",std::strlen("EHLO Here\r\n"));

    // Recv: EHLO
    memset(&readData, 0, SMTP_MTU);
    recvData(readData, SMTP_MTU);
    if (recvStatus(readData) < 0) {
        return -1;
    }
    std::cout << "login>> " << readData << std::endl;

    // Send: AUTH
    sendData("AUTH LOGIN\r\n", std::strlen("AUTH LOGIN\r\n"));

    // Recv: AUTH
    memset(&readData, 0, SMTP_MTU);
    recvData(readData, SMTP_MTU);
    if (recvStatus(readData) < 0) {
        return -1;
    }
    std::cout << "login>> " << readData << std::endl;

    // Send: username
    memset(&writeData, 0, SMTP_MTU);
    strncpy(writeData, encodeBase64(smtpLogin), BASE64_SIZE(std::strlen(smtpLogin)));

    std::cout << "login>> " << smtpLogin << " : " << writeData << std::endl;

    std::strcat(writeData, "\r\n");
    sendData(writeData, strlen(writeData));

    // Recv: username
    memset(&readData, 0, SMTP_MTU);
    recvData(readData, SMTP_MTU);
    if (recvStatus(readData) < 0) {
        return -1;
    }
    std::cout << "login>> " << readData << std::endl;

    // Send: password
    memset(&writeData, 0, SMTP_MTU);
    strncpy(writeData, encodeBase64(smtpPassword), BASE64_SIZE(std::strlen(smtpPassword)));

    std::cout << "login>> " << smtpPassword << " : " << writeData << std::endl;

    std::strcat(writeData, "\r\n");
    sendData(writeData, strlen(writeData));

    // Recv: password
    memset(&readData, 0, SMTP_MTU);
    recvData(readData, SMTP_MTU);
    if (recvStatus(readData) < 0) {
        return -1;
    }
    std::cout << "login>> " << readData << std::endl;

    strcpy(this->username, smtpLogin);
    strcpy(this->password, smtpPassword);

    return 0;
}

size_t SmtpClient::createLetter(const char* toMail,
                                const char* letterSubject,
                                const char* letterText) {

    if (toMail == nullptr) {
        std::cerr << "You must specify TO email address" << std::endl;
        return 0;
    }

    memset(letter, 0, SMTP_MTU);
    sprintf(letter,
            "From: <%s>\r\n"
            "To: <%s>\r\n"
            "Subject: %s\r\n"
            "MIME-version:1.0\r\n"
            "Content-Type:multipart/mixed;\n boundary=\"%s\"\r\n\r\n"
            "This is a multi-part message in MIME format.\r\n"
            "--%s\r\n"
            "Content-Type: text/plain; charset=\"UTF-8\"\r\n"
            "\r\n%s\r\n\r\n"
            "--%s\r\n",
            std::string(username).append("@yandex.ru").c_str(),
            toMail,
            letterSubject,
            boundary,
            boundary,
            letterText,
            boundary);

    strcpy(mailTo, toMail);

    return std::strlen(letter);
}

int SmtpClient::sendLetter() {
    char readData[SMTP_MTU];
    char writeData[SMTP_MTU];

    memset(&writeData, 0, SMTP_MTU);
    sprintf(writeData,
            "MAIL FROM:<%s>\r\n",
            std::string(username).append("@yandex.ru").c_str());
    sendData(writeData, std::strlen(writeData));
    std::cout << "send>> " << writeData << std::endl;

    memset(&readData, 0, SMTP_MTU);
    recvData(readData, SMTP_MTU);
    std::cout << "send>> " << readData << "\nusername : " << username << std::endl;
    if (recvStatus(readData) < 0) {
        return -1;
    }

    memset(&writeData, 0, SMTP_MTU);
    sprintf(writeData, "RCPT TO:<%s>\r\n", mailTo);
    std::cout << "send>> " << writeData << std::endl;
    sendData(writeData, std::strlen(writeData));

    memset(&readData, 0, SMTP_MTU);
    recvData(readData, SMTP_MTU);
    std::cout << "send>> " << readData << std::endl;
    if (recvStatus(readData) < 0) {
        return -1;
    }

    std::cout << "send>> DATA\r\n" << std::endl;
    sendData("DATA\r\n", std::strlen("DATA\r\n"));

    memset(&readData, 0, SMTP_MTU);
    recvData(readData, SMTP_MTU);
    std::cout << "send>> " << readData << std::endl;
    if (recvStatus(readData) < 0) {
        return -1;
    }

    std::cout << "send>> " << letter << std::endl;
    sendData(letter, std::strlen(letter));

    memset(&readData, 0, SMTP_MTU);
    recvData(readData, SMTP_MTU);
    std::cout << "send>> " << readData << std::endl;
    if (recvStatus(readData) < 0) {
        return -1;
    }

    std::cout << "send>> " << "QUIT\r\n" << std::endl;
    sendData("QUIT\r\n", std::strlen("QUIT\r\n"));

    memset(&readData, 0, SMTP_MTU);
    recvData(readData, SMTP_MTU);
    std::cout << "send>> " << readData << std::endl;
    if (recvStatus(readData) < 0) {
        return -1;
    }

    return 0;
}

int SmtpClient::endLetter() {
    strcat(letter, "\r\n.\r\n");
    letterEnded = true;
    return 1;
}

int SmtpClient::addAttachment(const char* filePath) {
    const char *contentType     = "Content-Type: application/octet-stream";
    const char *contentEncode   = "Content-Transfer-Encoding: base64";
    const char *contentDes      = "Content-Disposition: attachment";
    long fileLength = -1;
    size_t attachmentHeaderSize = strlen(contentType) +
                                  (strlen(filePath) * 2) +
                                  strlen(contentEncode) +
                                  strlen(contentDes) + 50;
    std::ifstream fileAttachment(filePath, std::ios::binary);

    fileAttachment.seekg(0, std::ifstream::end);
    fileLength = fileAttachment.tellg();
    fileAttachment.seekg(0, std::ifstream::beg);

    if (fileLength < 0) {
        std::cerr << "Can't get size of the file " << filePath << "!" << std::endl;
        return -1;
    }

    char* binaryAttachment = new char[fileLength];
    char* attachmentHeader = new char[attachmentHeaderSize];
    char* base64BinaryAttachment = nullptr;

    fileAttachment.read(binaryAttachment, fileLength);

    base64BinaryAttachment = encodeBase64(binaryAttachment);
    delete[] binaryAttachment;
    std::cout << "addAttachment>> created Base64 attachment version" << std::endl;

    memset(attachmentHeader, 0, attachmentHeaderSize);
    sprintf(attachmentHeader,
            "%s;\n name=\"%s\"\r\n"
            "%s\r\n"
            "X-Attachment-Id: %d\r\n"
            "%s; filename=\"%s\"\r\n\r\n",
            contentType, filePath,
            contentEncode,
            (++attachmentId),
            contentDes, filePath);

    strcat(letter, attachmentHeader);
    strcat(letter, base64BinaryAttachment);
    strcat(letter, "\r\n");

    std::cout << "addAttachment>> attachment added to letter" << std::endl;

    delete[] attachmentHeader;
    delete[] base64BinaryAttachment;

    return 0;
}
