#pragma once

#include <QAudioOutput>
#include <QMediaPlayer>
#include <QVideoWidget>
#include <QWidget>

namespace Ui {
class VideoGalleryItem;
}

class CustomVideoWidget : public QVideoWidget {
    Q_OBJECT

   public:
    CustomVideoWidget(QWidget* parent) : QVideoWidget(parent) {}

   signals:
    void onMousePressed();
    void onMouseDoublePressed();

    // QWidget interface
   protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
};

class VideoGalleryItem : public QWidget {
    Q_OBJECT

   public:
    explicit VideoGalleryItem(QWidget* parent, const QString& path);
    ~VideoGalleryItem();

   private slots:
    void onPlaybackStateChanged(QMediaPlayer::PlaybackState state);
    void onMediaDurationChanged(qint64 duration);
    void onMediaPositionChanged(qint64 position);
    void on_playPauseButton_clicked();
    void on_progressSlider_sliderMoved(int position);
    void on_fullscreenButton_clicked();
    void onPlayerClicked();
    void onPlayerDoubleClicked();
    void onMediaError(QMediaPlayer::Error error, const QString& errorString);

   private:
    Ui::VideoGalleryItem* ui;

    QString m_path;

    QMediaPlayer* m_mediaPlayer;
    QAudioOutput* m_audioOutput;

    // QWidget interface
   protected:
    void showEvent(QShowEvent* event) override;
    void hideEvent(QHideEvent* event) override;
};
