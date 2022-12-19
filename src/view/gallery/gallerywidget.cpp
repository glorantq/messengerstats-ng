#include "include/view/gallery/gallerywidget.h"
#include "./ui_gallerywidget.h"

#include <QDateTime>
#include <QDesktopServices>
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>

#include "view/gallery/audiogalleryitem.h"
#include "view/gallery/imagegalleryitem.h"
#include "view/gallery/videogalleryitem.h"
#include "view/settings.h"

GalleryWidget::GalleryWidget(QWidget* parent,
                             const QList<GalleryItem>& items,
                             bool allowMessageJumping)
    : QWidget(parent), ui(new Ui::GalleryWidget) {
    ui->setupUi(this);

    setUpGallery(items, allowMessageJumping);
}

GalleryWidget::GalleryWidget(QWidget* parent)
    : QWidget(parent), ui(new Ui::GalleryWidget) {
    ui->setupUi(this);
}

GalleryWidget::~GalleryWidget() {
    delete ui;
}

void GalleryWidget::setUpGallery(const QList<GalleryItem>& items,
                                 bool allowMessageJumping) {
    if (!items.isEmpty()) {
        QWidget* placeholderWidget = ui->stackedWidget->widget(0);

        for (const auto& item : items) {
            QWidget* toAdd = nullptr;

            if (item.m_type == GalleryItemType::ImageItem) {
                toAdd = new ImageGalleryItem(ui->stackedWidget, item.m_path);
            } else if (item.m_type == GalleryItemType::VideoItem) {
                toAdd = new VideoGalleryItem(ui->stackedWidget, item.m_path);
            } else if (item.m_type == GalleryItemType::AudioItem) {
                toAdd = new AudioGalleryItem(ui->stackedWidget, item.m_path);
            }

            if (toAdd != nullptr) {
                ui->stackedWidget->addWidget(toAdd);

                m_items.push_back(item);
            }
        }

        if (ui->stackedWidget->count() > 1) {
            ui->stackedWidget->removeWidget(placeholderWidget);
        }
    }

    if (!allowMessageJumping) {
        ui->jumpToMessageButton->setEnabled(false);
        ui->jumpToMessageButton->setVisible(false);
    }

    int minColumnWidth =
        qMax(ui->buttonContainer->width(), ui->informationLabel->width());

    QGridLayout* actionsLayout = (QGridLayout*)ui->actionsGrid->layout();
    actionsLayout->setColumnMinimumWidth(0, minColumnWidth);
    actionsLayout->setColumnMinimumWidth(2, minColumnWidth);
}

void GalleryWidget::on_leftButton_clicked() {
    transitionToIndex(ui->stackedWidget->currentIndex() - 1);
}

void GalleryWidget::on_rightButton_clicked() {
    transitionToIndex(ui->stackedWidget->currentIndex() + 1);
}

void GalleryWidget::transitionToIndex(int index) {
    if (index < 0 || index >= ui->stackedWidget->count()) {
        return;
    }

    bool slidePage =
        QSettings().value(SETTINGS_KEY_ANIMATE_PAGE_TRANSITION, true).toBool();

    if (slidePage) {
        ui->stackedWidget->slideInIdx(index);
    } else {
        ui->stackedWidget->setCurrentIndex(index);
    }
}

void GalleryWidget::on_stackedWidget_currentChanged(int arg1) {
    ui->itemIndexLabel->setText(
        QString("%1 / %2").arg(arg1 + 1).arg(ui->stackedWidget->count()));

    ui->leftButton->setEnabled(arg1 > 0);
    ui->rightButton->setEnabled(arg1 < ui->stackedWidget->count() - 1);

    const GalleryItem& item = m_items.at(arg1);
    ui->informationLabel->setText(
        QString("%1<br>(<small>%2</small>)")
            .arg(item.m_sender)
            .arg(QDateTime::fromMSecsSinceEpoch(item.m_timestamp)
                     .toString(tr("yyyy. MM. dd. hh:mm:ss"))));
}

void GalleryWidget::on_openNativeButton_clicked() {
    if (!m_items.isEmpty()) {
        const GalleryItem& item = m_items.at(ui->stackedWidget->currentIndex());
        QDesktopServices::openUrl(QUrl::fromLocalFile(item.m_path));
    }
}

void GalleryWidget::on_saveButton_clicked() {
    if (!m_items.isEmpty()) {
        const GalleryItem& item = m_items.at(ui->stackedWidget->currentIndex());
        const QString& originalPath = item.m_path;

        QString selectedPath =
            QFileDialog::getSaveFileName(this, tr("Save media"), originalPath);

        if (!selectedPath.isEmpty()) {
            QFile originalFile(originalPath);

            if (!originalFile.copy(selectedPath)) {
                QMessageBox::warning(this, parentWidget()->windowTitle(),
                                     tr("Failed to copy file!"));
            }
        }
    }
}

void GalleryWidget::on_jumpToMessageButton_clicked() {
    if (!m_items.isEmpty()) {
        const GalleryItem& item = m_items.at(ui->stackedWidget->currentIndex());
        emit onNavigateToMessage(item.m_messageIndex);
    }
}
