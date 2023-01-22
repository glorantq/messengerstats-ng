#include "include/view/message/messageinformationdialog.h"
#include "./ui_messageinformationdialog.h"

#include <QDesktopServices>
#include <QFileDialog>
#include <QFileIconProvider>
#include <QMessageBox>
#include <QPainter>

#include "model/thread.h"

MessageInformationDialog::MessageInformationDialog(QWidget* parent,
                                                   data::Message* message)
    : QDialog(parent),
      ui(new Ui::MessageInformationDialog),
      m_message(message) {
    ui->setupUi(this);

    m_attachmentItemDelegate = new AttachmentItemDelegate;
    ui->attachmentsListWidget->setItemDelegate(m_attachmentItemDelegate);

    m_reactionsItemDelegate = new ReactionsItemDelegate;
    ui->reactionsListWidget->setItemDelegate(m_reactionsItemDelegate);

    QPalette listPalette = palette();
    listPalette.setColor(QPalette::Highlight,
                         palette().color(QPalette::AlternateBase));
    listPalette.setColor(QPalette::Base, palette().color(QPalette::Window));
    listPalette.setColor(QPalette::AlternateBase,
                         palette().color(QPalette::Window));
    ui->attachmentsListWidget->setPalette(listPalette);
    ui->reactionsListWidget->setPalette(listPalette);

    ui->sentByLabel->setText(message->getSender().m_name);
    ui->sharedLinkLineEdit->setText(message->getSharedLink());
    ui->contentTextEdit->setText(message->getContent());
    ui->timestampLabel->setText(
        QDateTime::fromMSecsSinceEpoch(message->getTimestamp())
            .toString(tr("yyyy. MM. dd. hh:mm:ss")));
    ui->threadNameLabel->setText(message->getThread()->getDisplayName());

    auto setDefaultIcon = [&]() {
        if (message->getThread()->getThreadType() ==
            data::ThreadType::RegularGroup) {
            ui->threadIcon->setPixmap(
                QPixmap("://resources/images/default-group.png"));
        }
    };

    if (message->getThread()
            ->getThreadIcon()) {  // If the thread has an icon use it
        QPixmap iconPixmap(*message->getThread()->getThreadIcon());

        if (!iconPixmap.isNull()) {
            ui->threadIcon->setPixmap(iconPixmap);
        } else {
            // Else, if this is a group use the default group avatar
            setDefaultIcon();
        }
    } else {
        setDefaultIcon();
    }

    QString typeString = tr("Unknown");

    switch (message->getType()) {
        case data::MessageType::UnknownMessageType:
            typeString = tr("Unknown");
            break;

        case data::MessageType::Generic:
            typeString = tr("Generic");
            break;

        case data::MessageType::Share:
            typeString = tr("Share");
            break;

        case data::MessageType::Call:
            typeString = tr("Call");
            break;

        case data::MessageType::Subscribe:
            typeString = tr("Subscribe");
            break;

        case data::MessageType::Unsubscribe:
            typeString = tr("Unsubscribe");
            break;

        case data::MessageType::ClassifiedAsSystem:
            typeString = tr("Classified as system");
            break;

        case data::MessageType::NicknameChange:
            typeString = tr("Nickname change");
            break;
    }

    ui->typeLabel->setText(typeString);

    QList<GalleryItem> galleryItems{};

    for (const auto& path : message->getPictures()) {
        galleryItems.push_back({
            GalleryItemType::ImageItem,
            path,
            message->getSender().m_name,
            message->getTimestamp(),
            0,
        });
    }

    for (const auto& path : message->getGifs()) {
        galleryItems.push_back({
            GalleryItemType::ImageItem,
            path,
            message->getSender().m_name,
            message->getTimestamp(),
            0,
        });
    }

    for (const auto& path : message->getVideos()) {
        galleryItems.push_back({
            GalleryItemType::VideoItem,
            path,
            message->getSender().m_name,
            message->getTimestamp(),
            0,
        });
    }

    for (const auto& path : message->getAudioFiles()) {
        galleryItems.push_back({
            GalleryItemType::AudioItem,
            path,
            message->getSender().m_name,
            message->getTimestamp(),
            0,
        });
    }

    if (!message->getSticker().isEmpty()) {
        galleryItems.push_back({
            GalleryItemType::ImageItem,
            message->getSticker(),
            message->getSender().m_name,
            message->getTimestamp(),
            0,
        });
    }

    ui->galleryWidget->setUpGallery(galleryItems);

    QFileIconProvider iconProvider;
    for (const auto& path : message->getAttachments()) {
        QListWidgetItem* item = new QListWidgetItem(ui->attachmentsListWidget);
        QFileInfo fileInfo(path);

        item->setData(Qt::DisplayRole, fileInfo.fileName());

        QIcon icon = iconProvider.icon(fileInfo);
        if (icon.isNull()) {
            icon = QIcon("://resources/icon/silk/page_white.png");
        }

        item->setData(Qt::DecorationRole, icon);

        item->setData(AttachmentListData::FilePath, path);

        ui->attachmentsListWidget->addItem(item);
    }

    for (const auto& reaction : message->getReactions()) {
        QListWidgetItem* item = new QListWidgetItem(ui->reactionsListWidget);
        item->setData(ReactionListData::ReactionContent, reaction.m_reaction);
        item->setData(ReactionListData::ActorName, reaction.m_actor.m_name);

        ui->reactionsListWidget->addItem(item);
    }

    if (message->getSharedLink().isEmpty()) {
        ui->sharedLinkContainer->setVisible(false);
    }

    if (message->getContent().isEmpty()) {
        ui->tabWidget->removeTab(ui->tabWidget->indexOf(ui->contentTab));
    }

    if (message->getAttachments().isEmpty()) {
        ui->tabWidget->removeTab(ui->tabWidget->indexOf(ui->attachmentsTab));
    }

    if (message->getReactions().isEmpty()) {
        ui->tabWidget->removeTab(ui->tabWidget->indexOf(ui->reactionsTab));
    }

    if (galleryItems.isEmpty()) {
        ui->tabWidget->removeTab(ui->tabWidget->indexOf(ui->galleryTab));
    }
}

MessageInformationDialog::~MessageInformationDialog() {
    delete ui;

    m_attachmentItemDelegate->deleteLater();
    m_reactionsItemDelegate->deleteLater();
}

void MessageInformationDialog::on_openSharedLinkButton_clicked() {
    QDesktopServices::openUrl(QUrl(ui->sharedLinkLineEdit->text()));
}

void MessageInformationDialog::on_openFileButton_clicked() {
    auto selectedItems =
        ui->attachmentsListWidget->selectionModel()->selectedRows();
    if (selectedItems.count() == 0) {
        return;
    }

    QModelIndex selected = selectedItems.first();
    QString filePath = selected.data(AttachmentListData::FilePath).toString();

    onOpenSelectedAttachment(filePath);
}

void MessageInformationDialog::on_saveFileButton_clicked() {
    auto selectedItems =
        ui->attachmentsListWidget->selectionModel()->selectedRows();
    if (selectedItems.count() == 0) {
        return;
    }

    QModelIndex selected = selectedItems.first();
    QString filePath = selected.data(AttachmentListData::FilePath).toString();

    QString selectedPath =
        QFileDialog::getSaveFileName(this, tr("Save media"), filePath);

    if (!selectedPath.isEmpty()) {
        QFile originalFile(filePath);

        if (!originalFile.copy(selectedPath)) {
            QMessageBox::warning(this, parentWidget()->windowTitle(),
                                 tr("Failed to copy file!"));
        }
    }
}

void MessageInformationDialog::onOpenSelectedAttachment(QString path) {
    if (QMessageBox::warning(this, windowTitle(),
                             tr("Do you really want to open this file? Make "
                                "sure you trust it's safe before doing so."),
                             QMessageBox::StandardButton::Yes |
                                 QMessageBox::StandardButton::No) ==
        QMessageBox::StandardButton::Yes) {
        QDesktopServices::openUrl(QUrl::fromLocalFile(path));
    }
}

void MessageInformationDialog::on_attachmentsListWidget_itemDoubleClicked(
    QListWidgetItem* item) {
    if (item != nullptr) {
        onOpenSelectedAttachment(
            item->data(AttachmentListData::FilePath).toString());
    }
}

void AttachmentItemDelegate::paint(QPainter* painter,
                                   const QStyleOptionViewItem& option,
                                   const QModelIndex& index) const {
    QString text = index.data(Qt::DisplayRole).toString();
    QIcon icon = index.data(Qt::DecorationRole).value<QIcon>();

    QStyleOptionViewItem opt(option);
    painter->save();
    painter->setRenderHints(QPainter::RenderHint::Antialiasing |
                            QPainter::RenderHint::TextAntialiasing);

    QStyledItemDelegate::paint(painter, opt, QModelIndex());

    opt.font.setHintingPreference(QFont::PreferNoHinting);
    opt.fontMetrics = QFontMetrics(opt.font);

    opt.rect.moveRight(-3);
    opt.rect.moveLeft(3);

    if (!icon.isNull()) {
        icon.paint(painter, opt.rect.x(),
                   opt.rect.y() + opt.rect.height() / 2 -
                       opt.decorationSize.height() / 2,
                   opt.decorationSize.width(), opt.decorationSize.height(),
                   Qt::AlignLeft | Qt::AlignTop);
    }

    opt.rect.setLeft(opt.rect.left() + opt.decorationSize.width() + 3);
    painter->drawText(
        opt.rect, Qt::AlignLeft | Qt::AlignTop | Qt::TextWrapAnywhere, text);

    painter->restore();
}

QSize AttachmentItemDelegate::sizeHint(const QStyleOptionViewItem& option,
                                       const QModelIndex& index) const {
    QString text = index.data(Qt::DisplayRole).toString();

    QStyleOptionViewItem opt(option);
    opt.font.setHintingPreference(QFont::PreferNoHinting);
    opt.fontMetrics = QFontMetrics(opt.font);

    opt.rect.moveRight(-3);
    opt.rect.moveLeft(3);
    opt.rect.moveLeft(opt.decorationSize.width() + 3);

    QRect textSize = opt.fontMetrics.boundingRect(
        opt.rect, Qt::AlignLeft | Qt::AlignTop | Qt::TextWrapAnywhere, text);

    return {option.rect.width(),
            qMax(textSize.height(), opt.decorationSize.height())};
}

void ReactionsItemDelegate::paint(QPainter* painter,
                                  const QStyleOptionViewItem& option,
                                  const QModelIndex& index) const {
    QString name = index.data(ReactionListData::ActorName).toString();
    QString reaction = index.data(ReactionListData::ReactionContent).toString();

    QStyleOptionViewItem opt(option);
    painter->save();
    painter->setRenderHints(QPainter::RenderHint::Antialiasing |
                            QPainter::RenderHint::TextAntialiasing);

    QStyledItemDelegate::paint(painter, opt, QModelIndex());

    opt.font.setHintingPreference(QFont::PreferNoHinting);
    opt.fontMetrics = QFontMetrics(opt.font);

    painter->setFont(opt.font);

    QRect reactionSize = opt.fontMetrics.boundingRect(
        opt.rect, Qt::AlignLeft | Qt::AlignTop | Qt::TextWordWrap, reaction);

    painter->drawText(opt.rect, Qt::AlignLeft | Qt::AlignTop | Qt::TextWordWrap,
                      reaction);

    opt.rect.moveLeft(reactionSize.width());

    painter->setPen(opt.palette.color(QPalette::Disabled, QPalette::Text));
    painter->drawText(opt.rect, Qt::AlignLeft | Qt::AlignTop | Qt::TextWordWrap,
                      QString(" - %1").arg(name));

    painter->restore();
}

QSize ReactionsItemDelegate::sizeHint(const QStyleOptionViewItem& option,
                                      const QModelIndex& index) const {
    QString name = index.data(ReactionListData::ActorName).toString();
    QString reaction = index.data(ReactionListData::ReactionContent).toString();

    QStyleOptionViewItem opt(option);
    opt.font.setHintingPreference(QFont::PreferNoHinting);
    opt.fontMetrics = QFontMetrics(opt.font);

    QRect textSize = opt.fontMetrics.boundingRect(
        opt.rect, Qt::AlignLeft | Qt::AlignTop | Qt::TextWordWrap,
        QString("%1 - %2").arg(reaction).arg(name));

    return {textSize.width(), textSize.height()};
}

void MessageInformationDialog::on_openPersonButton_clicked() {
    emit onPersonInformationRequested(m_message->getSender().m_identifier);
}
