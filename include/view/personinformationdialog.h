#pragma once

#include <QDialog>

#include "model/messengerdata.h"

namespace Ui {
class PersonInformationDialog;
}

class PersonInformationDialog : public QDialog {
    Q_OBJECT

   public:
    explicit PersonInformationDialog(QWidget* parent,
                                     const QUuid& identifier,
                                     const data::MessengerData* data);
    ~PersonInformationDialog();

   private:
    Ui::PersonInformationDialog* ui;
};
