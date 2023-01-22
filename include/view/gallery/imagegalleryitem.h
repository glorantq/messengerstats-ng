#pragma once

#include <QWidget>

namespace Ui {
class ImageGalleryItem;
}

class ImageGalleryItem : public QWidget {
    Q_OBJECT

   public:
    explicit ImageGalleryItem(QWidget* parent, const QString& imagePath);
    ~ImageGalleryItem();

   private:
    Ui::ImageGalleryItem* ui;

    QString m_path;
    QPixmap m_pixmap;

    void updatePixmap();

    // QWidget interface
   protected:
    void resizeEvent(QResizeEvent* event) override;
    void showEvent(QShowEvent* event) override;
    void hideEvent(QHideEvent* event) override;
};
