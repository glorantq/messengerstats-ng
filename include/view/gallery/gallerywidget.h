#pragma once

#include <QWidget>

namespace Ui {
class GalleryWidget;
}

enum GalleryItemType {
    ImageItem,
    VideoItem,
    AudioItem,
};

struct GalleryItem {
    GalleryItemType m_type;
    QString m_path;

    QString m_sender;
    unsigned long long m_timestamp;

    int m_messageIndex;
};

class GalleryWidget : public QWidget {
    Q_OBJECT

   public:
    explicit GalleryWidget(QWidget* parent,
                           const QList<GalleryItem>& items,
                           bool allowMessageJumping = false);

    explicit GalleryWidget(QWidget* parent = nullptr);
    ~GalleryWidget();

    void setUpGallery(const QList<GalleryItem>& items,
                      bool allowMessageJumping = false);

   signals:
    void onNavigateToMessage(int index);

   private slots:
    void on_leftButton_clicked();
    void on_rightButton_clicked();
    void on_stackedWidget_currentChanged(int arg1);
    void on_openNativeButton_clicked();
    void on_saveButton_clicked();
    void on_jumpToMessageButton_clicked();

   private:
    Ui::GalleryWidget* ui;

    QList<GalleryItem> m_items{};

    void transitionToIndex(int index);
};
