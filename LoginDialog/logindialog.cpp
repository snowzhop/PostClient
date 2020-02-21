#include "logindialog.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QDebug>

LoginDialog::LoginDialog(QMainWindow* parent) :
    QDialog(parent),
    emailLine(new QLineEdit(this)),
    passwordLine(new QLineEdit(this)),
    okButton(new QPushButton("ok", this)) {

    QLabel* emailLabel      = new QLabel("Email");
    QLabel* passwordLabel   = new QLabel("Password");

    emailLine->setPlaceholderText("example@example.domain");
    emailLine->setMaximumHeight(lineHeight);
    passwordLine->setPlaceholderText("password");
    passwordLine->setMaximumHeight(lineHeight);
    passwordLine->setEchoMode(QLineEdit::EchoMode::Password);

    QVBoxLayout* mainLayout     = new QVBoxLayout();
    QHBoxLayout* emailLayout    = new QHBoxLayout();
    QHBoxLayout* passwordLayout = new QHBoxLayout();

    emailLayout->addWidget(emailLabel);
    emailLayout->addWidget(emailLine);

    passwordLayout->addWidget(passwordLabel);
    passwordLayout->addWidget(passwordLine);

    mainLayout->addLayout(emailLayout);
    mainLayout->addLayout(passwordLayout);
    mainLayout->addWidget(okButton);

    connect(okButton, &QPushButton::clicked, this, &LoginDialog::saveFields);
    connect(okButton, &QPushButton::clicked, this, &QWidget::close);

    this->setLayout(mainLayout);
}

LoginDialog::~LoginDialog() {}

void LoginDialog::saveFields() {
    emit fieldsSaved(emailLine->text(), passwordLine->text());
}
