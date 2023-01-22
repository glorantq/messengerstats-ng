#pragma once

#include <QAudioOutput>
#include <QMediaPlayer>
#include <QWidget>

namespace Ui {
class AudioGalleryItem;
}

class AudioGalleryItem : public QWidget {
    Q_OBJECT

   public:
    explicit AudioGalleryItem(QWidget* parent, const QString& path);
    ~AudioGalleryItem();

   private slots:
    void onPlaybackStateChanged(QMediaPlayer::PlaybackState state);
    void onMediaDurationChanged(qint64 duration);
    void onMediaPositionChanged(qint64 position);
    void onMediaError(QMediaPlayer::Error error, const QString& errorString);
    void on_playPauseButton_clicked();
    void on_progressSlider_sliderMoved(int position);

   private:
    Ui::AudioGalleryItem* ui;

    QString m_path;

    QMediaPlayer* m_mediaPlayer;
    QAudioOutput* m_audioOutput;

    // QWidget interface
   protected:
    void showEvent(QShowEvent* event) override;
    void hideEvent(QHideEvent* event) override;
};
