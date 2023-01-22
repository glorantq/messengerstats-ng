#pragma once

#include <QDialog>
#include <QListWidgetItem>
#include <QStyledItemDelegate>

#include "model/message.h"

namespace Ui {
class MessageInformationDialog;
}

enum AttachmentListData {
    FilePath = Qt::UserRole,
};

enum ReactionListData {
    ActorName = Qt::UserRole,
    ReactionContent,
};

class AttachmentItemDelegate : public QStyledItemDelegate {
    // QAbstractItemDelegate interface
   public:
    void paint(QPainter* painter,
               const QStyleOptionViewItem& option,
               const QModelIndex& index) const override;
    QSize sizeHint(const QStyleOptionViewItem& option,
                   const QModelIndex& index) const override;
};

class ReactionsItemDelegate : public QStyledItemDelegate {
    // QAbstractItemDelegate interface
   public:
    void paint(QPainter* painter,
               const QStyleOptionViewItem& option,
               const QModelIndex& index) const override;
    QSize sizeHint(const QStyleOptionViewItem& option,
                   const QModelIndex& index) const override;
};

class MessageInformationDialog : public QDialog {
    Q_OBJECT

   public:
    explicit MessageInformationDialog(QWidget* parent, data::Message* message);
    ~MessageInformationDialog();

   signals:
    void onPersonInformationRequested(const QUuid identifier);

   private slots:
    void on_openSharedLinkButton_clicked();
    void on_openFileButton_clicked();
    void on_saveFileButton_clicked();
    void onOpenSelectedAttachment(QString path);
    void on_attachmentsListWidget_itemDoubleClicked(QListWidgetItem* item);
    void on_openPersonButton_clicked();

   private:
    Ui::MessageInformationDialog* ui;

    AttachmentItemDelegate* m_attachmentItemDelegate;
    ReactionsItemDelegate* m_reactionsItemDelegate;

    data::Message* m_message;
};
