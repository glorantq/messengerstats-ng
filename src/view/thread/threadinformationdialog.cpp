#include "include/view/thread/threadinformationdialog.h"
#include "./ui_threadinformationdialog.h"

#include <QFontDatabase>

#include "model/messengerdata.h"
#include "view/thread/nicknameitem.h"

ThreadInformationDialog::ThreadInformationDialog(QWidget* parent,
                                                 data::Thread* thread)
    : QDialog(parent), ui(new Ui::ThreadInformationDialog), m_thread(thread) {
    ui->setupUi(this);

    ui->pathLineEdit->setText(
        thread->getOwnerDownload()->getRootDirectory().filePath(
            thread->getThreadPath()));
    ui->pathLineEdit->setFont(
        QFont(QFontDatabase::applicationFontFamilies(1), 9));

    ui->displayNameLabel->setText(thread->getDisplayName());

    QString typeString;

    switch (thread->getThreadType()) {
        case data::RegularGroup:
            typeString = tr("Regular group");
            break;
        case data::Regular:
            typeString = tr("Regular direct message");
            break;
        case data::UnknownThreadType:
        default:
            typeString = tr("Unknown");
            break;
    }

    ui->typeLabel->setText(typeString);

    auto setDefaultIcon = [&]() {
        if (thread->getThreadType() == data::ThreadType::RegularGroup) {
            ui->iconLabel->setPixmap(
                QPixmap("://resources/images/default-group.png"));
        }
    };

    if (thread->getThreadIcon()) {  // If the thread has an icon, use it
        QPixmap iconPixmap(*thread->getThreadIcon());

        if (!iconPixmap.isNull()) {
            ui->iconLabel->setPixmap(iconPixmap);
        } else {
            setDefaultIcon();
        }
    } else {
        // Else, if this is a group use the default group avatar
        setDefaultIcon();
    }

    for (const auto& participant : thread->getParticipants()) {
        QListWidgetItem* item = new QListWidgetItem(ui->participantsList);
        item->setText(participant.m_name);
        item->setData(Qt::UserRole, participant.m_identifier);

        ui->participantsList->addItem(item);
    }

    QPalette listPalette = palette();
    listPalette.setColor(QPalette::Highlight,
                         palette().color(QPalette::AlternateBase));
    listPalette.setColor(QPalette::Base, palette().color(QPalette::Window));
    listPalette.setColor(QPalette::AlternateBase,
                         palette().color(QPalette::Window));
    ui->participantsList->setPalette(listPalette);
    ui->nicknamesList->setPalette(listPalette);

    ui->participantsLabel->setText(
        QString("%1").arg(thread->getParticipants().count()));
    ui->messagesLabel->setText(
        QString("%1").arg(thread->getMessages().count()));
}

ThreadInformationDialog::~ThreadInformationDialog() {
    delete ui;
}

void ThreadInformationDialog::on_participantsList_currentItemChanged(
    QListWidgetItem* current,
    QListWidgetItem* previous) {
    QUuid identifier = current->data(Qt::UserRole).toUuid();

    ui->nicknamesList->clear();
    for (const auto& nickname : m_thread->getNicknames()[identifier]) {
        QListWidgetItem* item = new QListWidgetItem(ui->nicknamesList);

        NicknameItem* nicknameWidget =
            new NicknameItem(ui->nicknamesList, m_thread, nickname);

        item->setSizeHint(
            QSize(nicknameWidget->width(), nicknameWidget->height()));
        ui->nicknamesList->addItem(item);
        ui->nicknamesList->setItemWidget(item, nicknameWidget);
    }
}
