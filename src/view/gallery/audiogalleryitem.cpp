#include "include/view/gallery/audiogalleryitem.h"
#include "./ui_audiogalleryitem.h"

#include <QMessageBox>

AudioGalleryItem::AudioGalleryItem(QWidget* parent, const QString& path)
    : QWidget(parent), ui(new Ui::AudioGalleryItem) {
    ui->setupUi(this);

    m_mediaPlayer = new QMediaPlayer(this);

    connect(m_mediaPlayer, &QMediaPlayer::playbackStateChanged, this,
            &AudioGalleryItem::onPlaybackStateChanged);
    connect(m_mediaPlayer, &QMediaPlayer::durationChanged, this,
            &AudioGalleryItem::onMediaDurationChanged);
    connect(m_mediaPlayer, &QMediaPlayer::positionChanged, this,
            &AudioGalleryItem::onMediaPositionChanged);
    connect(m_mediaPlayer, &QMediaPlayer::errorOccurred, this,
            &AudioGalleryItem::onMediaError);

    m_audioOutput = new QAudioOutput(this);
    m_mediaPlayer->setAudioOutput(m_audioOutput);

    m_path = path;
}

AudioGalleryItem::~AudioGalleryItem() {
    delete ui;
}

void AudioGalleryItem::onPlaybackStateChanged(
    QMediaPlayer::PlaybackState state) {
    if (state == QMediaPlayer::PlaybackState::PlayingState) {
        ui->playPauseButton->setIcon(
            QIcon("://resources/icon/silk/pause_green.png"));
    } else {
        ui->playPauseButton->setIcon(
            QIcon("://resources/icon/silk/play_green.png"));
    }
}

void AudioGalleryItem::onMediaDurationChanged(qint64 duration) {
    ui->progressSlider->setRange(0, duration);
}

void AudioGalleryItem::onMediaPositionChanged(qint64 position) {
    ui->progressSlider->setValue(position);
}

void AudioGalleryItem::showEvent(QShowEvent* event) {
    ui->playPauseButton->setEnabled(true);
    m_mediaPlayer->setSource(QUrl::fromLocalFile(m_path));
}

void AudioGalleryItem::hideEvent(QHideEvent* event) {
    m_mediaPlayer->stop();
    m_mediaPlayer->setSource(QUrl());
}

void AudioGalleryItem::onMediaError(QMediaPlayer::Error error,
                                    const QString& errorString) {
    ui->playPauseButton->setEnabled(false);

    QMessageBox::warning(this, parentWidget()->windowTitle(),
                         tr("Failed to play audio: %1").arg(errorString));
}

void AudioGalleryItem::on_playPauseButton_clicked() {
    if (m_mediaPlayer->playbackState() ==
        QMediaPlayer::PlaybackState::PlayingState) {
        m_mediaPlayer->pause();
    } else {
        m_mediaPlayer->play();
    }
}

void AudioGalleryItem::on_progressSlider_sliderMoved(int position) {
    m_mediaPlayer->setPosition(position);
}
