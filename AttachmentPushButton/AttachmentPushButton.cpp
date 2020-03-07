#include "AttachmentPushButton/AttachmentPushButton.h"

#include <QEvent>
#include <QDebug>
#include <QFile>
#include <QFileDialog>
#include <QTextStream>

AttachmentPushButton::AttachmentPushButton(QWidget* parent) : QPushButton (parent) {
    this->setAttribute(Qt::WA_Hover, true);
    this->setIcon(QIcon(":/images/attach_64px.png"));

    connect(this, &AttachmentPushButton::clicked, this, &AttachmentPushButton::saveAttachment);
}

AttachmentPushButton::~AttachmentPushButton() {}

bool AttachmentPushButton::event(QEvent *e) {
    switch (e->type()) {
    case QEvent::HoverEnter: {
        this->setIcon(QIcon(":/images/down-arrow64px.png"));
        break;
    }
    case QEvent::HoverLeave: {
        this->setIcon(QIcon(":/images/attach_64px.png"));
        break;
    }
    default:
    {}
    }

    return QWidget::event(e);
}

void AttachmentPushButton::setAttachment(const QByteArray& attach) {
    attachment = attach;
}

void AttachmentPushButton::saveAttachment() {
    auto dir = QFileDialog::getExistingDirectory(this,
               "Open Directory",
               "/home",
               QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    QFile attachmentFile(dir + "/" + this->text());
    if (attachmentFile.open(QIODevice::WriteOnly)) {
        attachmentFile.write(attachment);
        attachmentFile.close();
    }
    qDebug() << "Attachment " << this->text() << " saved c:";
}
