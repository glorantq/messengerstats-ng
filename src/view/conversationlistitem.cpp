#include "view/conversationlistitem.h"
#include "./ui_conversationlistitem.h"

ConversationListItem::ConversationListItem(QWidget* parent,
                                           data::Thread* thread)
    : QWidget(parent), ui(new Ui::ConversationListItem) {
    ui->setupUi(this);

    ui->userNameLabel->setText(thread->getDisplayName());

    auto setDefaultIcon = [&]() {
        if (thread->getThreadType() == data::ThreadType::RegularGroup) {
            ui->profilePictureLabel->setPixmap(
                QPixmap("://resources/images/default-group.png"));
        }
    };

    if (thread->getThreadIcon()) {  // If the thread has an icon use it
        QPixmap iconPixmap(*thread->getThreadIcon());

        if (!iconPixmap.isNull()) {
            ui->profilePictureLabel->setPixmap(iconPixmap);
        } else {
            // Else, if this is a group use the default group avatar
            setDefaultIcon();
        }
    } else {
        setDefaultIcon();
    }

    // If the thread is a group, display the number of participants as well as
    // the number of messages
    if (thread->getThreadType() == data::ThreadType::RegularGroup) {
        ui->lastMessageLabel->setText(
            tr("%1 messages • %2 participants")
                .arg(thread->getMessages().count())
                .arg(thread->getParticipants().count()));
    } else {
        ui->lastMessageLabel->setText(
            tr("%1 messages").arg(thread->getMessages().count()));
    }
}

ConversationListItem::~ConversationListItem() {
    delete ui;
}
