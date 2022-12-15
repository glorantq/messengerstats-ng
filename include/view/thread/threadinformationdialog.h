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

   private slots:
    void on_participantsList_currentItemChanged(QListWidgetItem* current,
                                                QListWidgetItem* previous);

   private:
    Ui::ThreadInformationDialog* ui;

    data::Thread* m_thread;
};
