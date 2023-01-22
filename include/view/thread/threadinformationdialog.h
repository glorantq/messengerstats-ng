#pragma once

#include <QDialog>
#include <QListWidgetItem>

#include "model/thread.h"

namespace Ui {
class ThreadInformationDialog;
}

class ThreadInformationDialog : public QDialog {
    Q_OBJECT

   public:
    explicit ThreadInformationDialog(QWidget* parent, data::Thread* thread);
    ~ThreadInformationDialog();

   signals:
    void onPersonInformationRequested(const QUuid);

   private slots:
    void on_participantsList_currentItemChanged(QListWidgetItem* current,
                                                QListWidgetItem* previous);
    void on_participantsList_customContextMenuRequested(const QPoint& pos);

    void on_participantsList_itemDoubleClicked(QListWidgetItem* item);

   private:
    Ui::ThreadInformationDialog* ui;

    data::Thread* m_thread;
};
