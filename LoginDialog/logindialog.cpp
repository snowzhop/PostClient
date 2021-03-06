#include "logindialog.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QSizePolicy>
#include <QDebug>

LoginDialog::LoginDialog(QMainWindow* parent) :
    QDialog(parent),
    emailLine(new QLineEdit(this)),
    passwordLine(new QLineEdit(this)),
    smtpServerLine(new QLineEdit(this)),
    smtpPortLine(new QLineEdit(this)),
    pop3ServerLine(new QLineEdit(this)),
    pop3PortLine(new QLineEdit(this)),
    okButton(new QPushButton("ok", this)) {

    okButton->setIcon(QIcon(":/images/login_64px.png"));

    QLabel* smtpServerLabel = new QLabel("SMTP server", this);
    QLabel* smtpPortLabel   = new QLabel("SMTP port", this);
    QLabel* pop3ServerLabel = new QLabel("POP3 server", this);
    QLabel* pop3PortLabel   = new QLabel("POP3 port", this);
    QLabel* emailLabel      = new QLabel("Email", this);
    QLabel* passwordLabel   = new QLabel("Password", this);

    // Email
    emailLine->setPlaceholderText("example@example.domain");
    emailLine->setMinimumWidth(loginDialogWidth);
    // Password
    passwordLine->setPlaceholderText("password");
    passwordLine->setEchoMode(QLineEdit::EchoMode::Password);
    passwordLine->setMinimumWidth(loginDialogWidth);
    // SMTP
    smtpServerLine->setPlaceholderText("smtp.ser.ver");
    smtpServerLine->setMinimumWidth(loginDialogWidth);
    smtpPortLine->setPlaceholderText("000");
    smtpPortLine->setMinimumWidth(loginDialogWidth);
    // POP3
    pop3ServerLine->setPlaceholderText("pop3.ser.ver");
    pop3ServerLine->setMinimumWidth(loginDialogWidth);
    pop3PortLine->setPlaceholderText("000");
    pop3PortLine->setMinimumWidth(loginDialogWidth);

    QVBoxLayout* mainLayout         = new QVBoxLayout();
    QHBoxLayout* emailLayout        = new QHBoxLayout();
    QHBoxLayout* passwordLayout     = new QHBoxLayout();
    QHBoxLayout* smtpServerLayout   = new QHBoxLayout();
    QHBoxLayout* smtpPortLayout     = new QHBoxLayout();
    QHBoxLayout* pop3ServerLayout   = new QHBoxLayout();
    QHBoxLayout* pop3PortLayout     = new QHBoxLayout();

    smtpServerLayout->addWidget(smtpServerLabel, 0, Qt::AlignmentFlag::AlignLeft);
    smtpServerLayout->addWidget(smtpServerLine, 0, Qt::AlignmentFlag::AlignRight);
    smtpPortLayout->addWidget(smtpPortLabel, 0, Qt::AlignmentFlag::AlignLeft);
    smtpPortLayout->addWidget(smtpPortLine, 0, Qt::AlignmentFlag::AlignRight);

    pop3ServerLayout->addWidget(pop3ServerLabel, 0, Qt::AlignmentFlag::AlignLeft);
    pop3ServerLayout->addWidget(pop3ServerLine, 0, Qt::AlignmentFlag::AlignRight);
    pop3PortLayout->addWidget(pop3PortLabel, 0, Qt::AlignmentFlag::AlignLeft);
    pop3PortLayout->addWidget(pop3PortLine, 0, Qt::AlignmentFlag::AlignRight);

    emailLayout->addWidget(emailLabel, 0, Qt::AlignmentFlag::AlignLeft);
    emailLayout->addWidget(emailLine, 0, Qt::AlignmentFlag::AlignRight);

    passwordLayout->addWidget(passwordLabel, 0, Qt::AlignmentFlag::AlignLeft);
    passwordLayout->addWidget(passwordLine, 0, Qt::AlignmentFlag::AlignRight);

    mainLayout->addLayout(emailLayout);
    mainLayout->addLayout(passwordLayout);
    mainLayout->addLayout(smtpServerLayout);
    mainLayout->addLayout(smtpPortLayout);
    mainLayout->addLayout(pop3ServerLayout);
    mainLayout->addLayout(pop3PortLayout);
    mainLayout->addWidget(okButton);
    mainLayout->setSizeConstraint(QLayout::SizeConstraint::SetFixedSize);

    connect(okButton, &QPushButton::clicked, this, &LoginDialog::saveFields);
    connect(okButton, &QPushButton::clicked, this, &QWidget::close);


    this->setLayout(mainLayout);
}

LoginDialog::~LoginDialog() {
    delete emailLine;
    delete passwordLine;
    delete smtpServerLine;
    delete smtpPortLine;
    delete pop3ServerLine;
    delete pop3PortLine;
    delete okButton;
}

void LoginDialog::saveFields() {
    UserData data;
    data.email      = emailLine->text();
    data.password   = passwordLine->text();
    data.smtpServer = smtpServerLine->text();
    data.smtpPort   = smtpPortLine->text();
    data.pop3Server = pop3ServerLine->text();
    data.pop3Port   = pop3PortLine->text();

    emit fieldsSaved(data);
}
