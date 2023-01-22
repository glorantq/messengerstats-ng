#include "include/view/gallery/imagegalleryitem.h"
#include "./ui_imagegalleryitem.h"

#include <QMessageBox>

ImageGalleryItem::ImageGalleryItem(QWidget* parent, const QString& imagePath)
    : QWidget(parent), ui(new Ui::ImageGalleryItem) {
    ui->setupUi(this);

    m_path = imagePath;
}

ImageGalleryItem::~ImageGalleryItem() {
    delete ui;
}

void ImageGalleryItem::updatePixmap() {
    if (!m_pixmap.isNull()) {
        QPixmap scaled =
            m_pixmap.scaled(ui->imageLabel->size(), Qt::KeepAspectRatio,
                            Qt::SmoothTransformation);

        ui->imageLabel->setPixmap(scaled);
    }
}

void ImageGalleryItem::resizeEvent(QResizeEvent* event) {
    updatePixmap();
}

void ImageGalleryItem::showEvent(QShowEvent* event) {
    m_pixmap = QPixmap(m_path);
    if (m_pixmap.isNull()) {
        QMessageBox::warning(this, parentWidget()->windowTitle(),
                             tr("Failed to open image!"));
    }

    updatePixmap();
}

void ImageGalleryItem::hideEvent(QHideEvent* event) {
    m_pixmap = QPixmap();
    ui->imageLabel->setPixmap(m_pixmap);
}
