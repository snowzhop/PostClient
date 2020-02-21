#-------------------------------------------------
#
# Project created by QtCreator 2020-02-17T22:41:30
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

LIBS += -lssl \
    -lcrypto


TARGET = PostClient
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        Base64/base64util.cpp \
        LoginDialog/logindialog.cpp \
        POP3/pop3Client.cpp \
        SMTP/smtpClient.cpp \
        main.cpp \
        mainwindow.cpp

HEADERS += \
        Base64/base64util.h \
        LoginDialog/logindialog.h \
        LoginDialog/userdata.h \
        POP3/pop3Client.h \
        SMTP/smtpClient.h \
        mainwindow.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
