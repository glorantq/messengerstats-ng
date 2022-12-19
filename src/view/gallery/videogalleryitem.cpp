#include "include/view/gallery/videogalleryitem.h"
#include "./ui_videogalleryitem.h"

#include <QKeyEvent>
#include <QMediaMetaData>
#include <QMessageBox>
#include <QMouseEvent>

VideoGalleryItem::VideoGalleryItem(QWidget* parent, const QString& path)
    : QWidget(parent), ui(new Ui::VideoGalleryItem) {
    ui->setupUi(this);

    connect(ui->videoWidget, &CustomVideoWidget::onMousePressed, this,
            &VideoGalleryItem::onPlayerClicked);
    connect(ui->videoWidget, &CustomVideoWidget::onMouseDoublePressed, this,
            &VideoGalleryItem::onPlayerDoubleClicked);

    m_mediaPlayer = new QMediaPlayer(this);

    connect(m_mediaPlayer, &QMediaPlayer::playbackStateChanged, this,
            &VideoGalleryItem::onPlaybackStateChanged);
    connect(m_mediaPlayer, &QMediaPlayer::durationChanged, this,
            &VideoGalleryItem::onMediaDurationChanged);
    connect(m_mediaPlayer, &QMediaPlayer::positionChanged, this,
            &VideoGalleryItem::onMediaPositionChanged);
    connect(m_mediaPlayer, &QMediaPlayer::errorOccurred, this,
            &VideoGalleryItem::onMediaError);

    m_audioOutput = new QAudioOutput(this);

    m_mediaPlayer->setVideoOutput(ui->videoWidget);
    m_mediaPlayer->setAudioOutput(m_audioOutput);

    m_path = path;
}

VideoGalleryItem::~VideoGalleryItem() {
    delete ui;
}

void VideoGalleryItem::onPlaybackStateChanged(
    QMediaPlayer::PlaybackState state) {
    if (state == QMediaPlayer::PlaybackState::PlayingState) {
        ui->playPauseButton->setIcon(
            QIcon("://resources/icon/silk/pause_green.png"));
    } else {
        ui->playPauseButton->setIcon(
            QIcon("://resources/icon/silk/play_green.png"));
    }
}

void VideoGalleryItem::onMediaDurationChanged(qint64 duration) {
    ui->progressSlider->setRange(0, duration);
}

void VideoGalleryItem::onMediaPositionChanged(qint64 position) {
    ui->progressSlider->setValue(position);
}

void VideoGalleryItem::on_playPauseButton_clicked() {
    if (m_mediaPlayer->playbackState() ==
        QMediaPlayer::PlaybackState::PlayingState) {
        m_mediaPlayer->pause();
    } else {
        m_mediaPlayer->play();
    }
}

void VideoGalleryItem::on_progressSlider_sliderMoved(int position) {
    m_mediaPlayer->setPosition(position);
}

void VideoGalleryItem::showEvent(QShowEvent* event) {
    ui->playPauseButton->setEnabled(true);
    m_mediaPlayer->setSource(QUrl::fromLocalFile(m_path));
}

void VideoGalleryItem::hideEvent(QHideEvent* event) {
    m_mediaPlayer->stop();
    m_mediaPlayer->setSource(QUrl());
}

void VideoGalleryItem::on_fullscreenButton_clicked() {
    ui->videoWidget->setFullScreen(true);
}

void VideoGalleryItem::onPlayerClicked() {
    if (m_mediaPlayer->playbackState() ==
        QMediaPlayer::PlaybackState::PlayingState) {
        m_mediaPlayer->pause();
    } else {
        m_mediaPlayer->play();
    }
}

void VideoGalleryItem::onPlayerDoubleClicked() {
    ui->videoWidget->setFullScreen(!ui->videoWidget->isFullScreen());
}

void VideoGalleryItem::onMediaError(QMediaPlayer::Error error,
                                    const QString& errorString) {
    ui->playPauseButton->setEnabled(false);

    QMessageBox::warning(this, parentWidget()->windowTitle(),
                         tr("Failed to play video: %1").arg(errorString));
}

void CustomVideoWidget::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::MouseButton::LeftButton) {
        emit onMousePressed();
    }
}

void CustomVideoWidget::mouseDoubleClickEvent(QMouseEvent* event) {
    if (event->button() == Qt::MouseButton::LeftButton) {
        emit onMouseDoublePressed();
    }
}

void CustomVideoWidget::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Escape && this->isFullScreen()) {
        this->setFullScreen(false);
    }
}
