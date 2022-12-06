#pragma once

#include <QAbstractListModel>
#include <QRect>
#include <QStyledItemDelegate>
#include <QWidget>

#include "model/thread.h"

namespace Ui {
class ThreadPage;
}

class ThreadPage : public QWidget {
    Q_OBJECT

   public:
    explicit ThreadPage(QWidget* parent = nullptr,
                        data::Thread* thread = nullptr);
    ~ThreadPage();

   signals:
    void onBackPressed();

   private slots:
    void on_backButton_clicked();
    void on_messagesListView_doubleClicked(const QModelIndex& index);
    void on_chatContextMenuRequested(const QPoint& position);

   private:
    Ui::ThreadPage* ui;

    data::Thread* m_thread = nullptr;
};
