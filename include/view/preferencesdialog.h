#pragma once

#include <QDialog>

namespace Ui {
class PreferencesDialog;
}

class PreferencesDialog : public QDialog {
    Q_OBJECT

   public:
    explicit PreferencesDialog(QWidget* parent = nullptr);
    ~PreferencesDialog();

   private slots:
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();
    void on_buttonBox_resetRequested();

   private:
    Ui::PreferencesDialog* ui;
};
