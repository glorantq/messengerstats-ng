#include "include/view/thread/nicknameitem.h"
#include "./ui_nicknameitem.h"

NicknameItem::NicknameItem(QWidget* parent,
                           data::Thread* thread,
                           const data::Nickname& nickname)
    : QWidget(parent), ui(new Ui::NicknameItem) {
    ui->setupUi(this);

    ui->nicknameLabel->setText(nickname.m_nickname);
    ui->timestampLabel->setText(
        QString("%1 (%2)")
            .arg(QDateTime::fromMSecsSinceEpoch(nickname.m_timestamp)
                     .toString(tr("yyyy. MM. dd. hh:mm:ss")))
            .arg(thread->getNameForUUID(nickname.m_performer)));
}

NicknameItem::~NicknameItem() {
    delete ui;
}
