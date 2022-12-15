#include "view/preferencesdialog.h"
#include "./ui_preferencesdialog.h"

#include <QCoreApplication>
#include <QMessageBox>
#include <QPushButton>
#include <QSettings>

#include "view/settings.h"
#include "view/themeservice.h"
#include "view/thread/renderers/imessagerenderer.h"

PreferencesDialog::PreferencesDialog(QWidget* parent)
    : QDialog(parent), ui(new Ui::PreferencesDialog) {
    ui->setupUi(this);

    setWindowTitle(windowTitle().arg(QCoreApplication::applicationName()));

    QSettings settings;
    renderer::RendererParameters defaultRendererParameters{};

    // Initialise appearance tab
    {
        int textSize = settings
                           .value(SETTINGS_KEY_TEXT_SIZE,
                                  defaultRendererParameters.m_textSize)
                           .toUInt();

        ui->textSizeSpinBox->setValue(textSize);

        int baseIndex =
            settings.value(SETTINGS_KEY_BACKGROUND_THEME, 0).toInt();
        QStringList themeNames = theme::ThemeService::getBaseNames();

        for (const auto& theme : themeNames) {
            ui->backgroundComboBox->addItem(theme);
        }

        ui->backgroundComboBox->setCurrentIndex(baseIndex);

        int colorIndex = settings.value(SETTINGS_KEY_COLOR_SCHEME, 0).toInt();
        QStringList colorNames = theme::ThemeService::getColorNames();

        for (const auto& color : colorNames) {
            ui->colorSchemeComboBox->addItem(color);
        }

        ui->colorSchemeComboBox->setCurrentIndex(colorIndex);
    }

    // Initialise advanced tab
    {
        unsigned int imageCacheSize =
            settings.value(SETTINGS_KEY_IMAGE_CACHE_SIZE, 1024).toUInt();

        double thumbnailQuality =
            settings
                .value(SETTINGS_KEY_THUMBNAIL_QUALITY,
                       defaultRendererParameters.m_thumbnailQuality)
                .toDouble();

        ui->imageQualitySpinBox->setValue(thumbnailQuality * 100);
        ui->pixmapCacheSpinBox->setValue(imageCacheSize);
    }

    connect(ui->buttonBox->button(QDialogButtonBox::RestoreDefaults),
            &QPushButton::clicked, this,
            &PreferencesDialog::on_buttonBox_resetRequested);
}

PreferencesDialog::~PreferencesDialog() {
    delete ui;
}

void PreferencesDialog::on_buttonBox_accepted() {
    QSettings settings;

    // Save appearance
    {
        settings.setValue(SETTINGS_KEY_TEXT_SIZE, ui->textSizeSpinBox->value());

        int baseIndex = ui->backgroundComboBox->currentIndex();
        int colorIndex = ui->colorSchemeComboBox->currentIndex();
        settings.setValue(SETTINGS_KEY_BACKGROUND_THEME, baseIndex);
        settings.setValue(SETTINGS_KEY_COLOR_SCHEME, colorIndex);

        theme::ThemeService::apply(baseIndex, colorIndex);
    }

    // Save advanced
    {
        settings.setValue(SETTINGS_KEY_IMAGE_CACHE_SIZE,
                          ui->pixmapCacheSpinBox->value());

        settings.setValue(SETTINGS_KEY_THUMBNAIL_QUALITY,
                          ((double)ui->imageQualitySpinBox->value()) / 100.);
    }

    accept();
}

void PreferencesDialog::on_buttonBox_rejected() {
    reject();
}

void PreferencesDialog::on_buttonBox_resetRequested() {
    if (QMessageBox::question(this, windowTitle(),
                              tr("Do you really want to reset all settings to "
                                 "their respective defaults?")) ==
        QMessageBox::Yes) {
        renderer::RendererParameters defaultRendererParameters{};

        ui->backgroundComboBox->setCurrentIndex(0);
        ui->colorSchemeComboBox->setCurrentIndex(0);
        ui->textSizeSpinBox->setValue(defaultRendererParameters.m_textSize);

        ui->pixmapCacheSpinBox->setValue(1024);
        ui->imageQualitySpinBox->setValue(
            defaultRendererParameters.m_thumbnailQuality * 100);
    }
}
