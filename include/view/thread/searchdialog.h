#pragma once

#include <QDialog>
#include <QListWidgetItem>
#include <QRegularExpression>
#include <QStyledItemDelegate>

#include "model/thread.h"

namespace Ui {
class SearchDialog;
}

enum SearchModelData {
    MessageIndex = Qt::UserRole,
    MessageContent,
    MatchStart,
    MatchEnd,
    MatchLength,
    SenderName,
    Timestamp,
    MessagePointer
};

class SearchResultDelegate : public QStyledItemDelegate {
    Q_OBJECT
   private:
    QColor m_disabledColor;

   public:
    SearchResultDelegate(QWidget* parent) {
        m_disabledColor =
            parent->palette().color(QPalette::Disabled, QPalette::Text);
    }

    // QAbstractItemDelegate interface
   public:
    void paint(QPainter* painter,
               const QStyleOptionViewItem& option,
               const QModelIndex& index) const override;
    QSize sizeHint(const QStyleOptionViewItem& option,
                   const QModelIndex& index) const override;

   private:
    inline QString formatExtraText(const QString& senderName,
                                   unsigned long long timestamp) const;
};

class SearchDialog : public QDialog {
    Q_OBJECT

   public:
    explicit SearchDialog(QWidget* parent, data::Thread* thread);
    ~SearchDialog();

   signals:
    void onScrollToMessageIndex(int index);
    void onOpenMessageInformation(data::Message*);

   private slots:
    void on_buttonBox_rejected();
    void on_searchButton_pressed();
    void on_messagesList_customContextMenuRequested(const QPoint& pos);
    void on_messagesList_itemDoubleClicked(QListWidgetItem* item);

   private:
    Ui::SearchDialog* ui;
    SearchResultDelegate* m_resultDelegate;

    data::Thread* m_thread;
    QString matchGroupBoxPattern;

    void performSearch(const QRegularExpression&);
};
