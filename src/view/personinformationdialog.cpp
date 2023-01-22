#include "include/view/personinformationdialog.h"
#include "./ui_personinformationdialog.h"

#include <QFontDatabase>

PersonInformationDialog::PersonInformationDialog(
    QWidget* parent,
    const QUuid& identifier,
    const data::MessengerData* data)
    : QDialog(parent), ui(new Ui::PersonInformationDialog) {
    ui->setupUi(this);

    ui->identifierLabel->setFont(
        QFontDatabase::applicationFontFamilies(1).first());
    ui->identifierLabel->setText(identifier.toString(QUuid::WithoutBraces));

    ui->nameLabel->setText(data->getNameForUUID(identifier));

    for (const auto& thread : data->getThreads()) {
        if (!thread->getParticipants().contains({identifier, ""})) {
            continue;
        }

        QListWidgetItem* item = new QListWidgetItem(ui->threadsListWidget);
        item->setText(thread->getDisplayName());

        QIcon icon("://resources/images/default-avatar.jpg");

        auto setDefaultIcon = [&]() {
            if (thread->getThreadType() == data::ThreadType::RegularGroup) {
                icon = QIcon("://resources/images/default-group.png");
            }
        };

        if (thread->getThreadIcon()) {  // If the thread has an icon use it
            QPixmap iconPixmap(*thread->getThreadIcon());

            if (!iconPixmap.isNull()) {
                icon = QIcon(iconPixmap);
            } else {
                // Else, if this is a group use the default group avatar
                setDefaultIcon();
            }
        } else {
            setDefaultIcon();
        }

        item->setIcon(icon);

        ui->threadsListWidget->addItem(item);
    }
}

PersonInformationDialog::~PersonInformationDialog() {
    delete ui;
}
