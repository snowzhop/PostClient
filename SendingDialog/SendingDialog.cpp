#include "SendingDialog/SendingDialog.h"

#include <QLineEdit>
#include <QTextEdit>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QToolBar>
#include <QPushButton>
#include <QFileDialog>

#include <QDebug>

SendingDialog::SendingDialog(QWidget* parent) :
    QMainWindow (parent),
    centralWidget(new QWidget(this)),
    receiverEmailEdit(new QLineEdit(this)),
    subjectEdit(new QLineEdit(this)),
    letterEdit(new QTextEdit(this)) {

    this->setCentralWidget(centralWidget);
    this->setAttribute(Qt::WA_DeleteOnClose);

    QVBoxLayout* mainLayout     = new QVBoxLayout(centralWidget);
    QHBoxLayout* receiverLayout = new QHBoxLayout();
    QHBoxLayout* subjectLayout  = new QHBoxLayout();

    QLabel* receiverLabel   = new QLabel("Кому:", this);
    QLabel* subjectLabel    = new QLabel("Тема:", this);

    receiverLayout->addWidget(receiverLabel);
    receiverLayout->addWidget(receiverEmailEdit);

    subjectLayout->addWidget(subjectLabel);
    subjectLayout->addWidget(subjectEdit);

    mainLayout->addLayout(receiverLayout);
    mainLayout->addLayout(subjectLayout);
    mainLayout->addWidget(letterEdit);

    QPushButton* sendButton         = new QPushButton("Отправить", this);
    QPushButton* attachmentButton   = new QPushButton("Вложения", this);

    toolBar = new QToolBar(centralWidget);
    toolBar->addWidget(sendButton);
    toolBar->addSeparator();
    toolBar->addWidget(attachmentButton);
    toolBar->setMovable(false);

    this->addToolBar(Qt::LeftToolBarArea, toolBar);

    connect(sendButton, &QPushButton::clicked, this, &SendingDialog::finishLetter);
    connect(attachmentButton, &QPushButton::clicked, this, &SendingDialog::addAttachment);
}

SendingDialog::~SendingDialog() {
    delete receiverEmailEdit;
    delete subjectEdit;
    delete letterEdit;
}

void SendingDialog::letterPreparing() {
    this->show();
}

void SendingDialog::finishLetter() {
    emit compileLetter(receiverEmailEdit->text().toStdString(),
                       subjectEdit->text().toStdString(),
                       letterEdit->toPlainText().toStdString(),
                       attachmentPath);
    qDebug() << "letter finished";
    this->close();
}

void SendingDialog::addAttachment() {
    attachmentPath = QFileDialog::getOpenFileName(this, "Select file", ".").toStdString();
    qDebug() << "attachment: " << attachmentPath.c_str();
}
