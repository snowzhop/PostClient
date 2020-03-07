#ifndef ATT_PUSH_BUTTON_H
#define ATT_PUSH_BUTTON_H

#include <QPushButton>

class AttachmentPushButton : public QPushButton {
    Q_OBJECT
public:
    AttachmentPushButton(QWidget* parent = nullptr);
    virtual ~AttachmentPushButton() override;

    void setAttachment(const QByteArray& attach);

public slots:
    void saveAttachment();

protected:
    virtual bool event(QEvent* e) override;

private:
    QByteArray attachment;
};

#endif
