#include "view/preferencesdialog.h"
#include "./ui_preferencesdialog.h"

#include <QCoreApplication>

PreferencesDialog::PreferencesDialog(QWidget* parent)
    : QDialog(parent), ui(new Ui::PreferencesDialog) {
    ui->setupUi(this);

    setWindowTitle(windowTitle().arg(QCoreApplication::applicationName()));
}

PreferencesDialog::~PreferencesDialog() {
    delete ui;
}

void PreferencesDialog::on_okButton_clicked() {
    accept();
}
