#include "view/thread/renderers/genericmessagerenderer.h"

#include "view/thread/threadlistmodel.h"

#include <QImageReader>
#include <QPainterPath>
#include <QPixmap>
#include <QPixmapCache>

// At this point for this I'm highly debating writing a custom layout engine
// instead of doing things like this, because it's fast becoming a mess

// Paints the standard conversation chat bubbles. See the comment about size
// hinting for these types of messages. All of that applies here as well.
void renderer::GenericMessageRenderer::paint(
    QPainter* painter,
    const QStyleOptionViewItem& option,
    const QModelIndex& index,
    const RendererParameters& parameters) const {
    QStyleOptionViewItem opt(option);
    int originalRight = opt.rect.right();

    opt.rect.setWidth(vMax(300, parameters.m_parent->width() / 3));
    opt.rect = opt.rect.adjusted(parameters.m_margin, parameters.m_margin,
                                 -parameters.m_margin, -parameters.m_margin);

    QFont nameFont(opt.font);
    nameFont.setBold(true);

    QString content = index.data(message::ModelData::Content).toString();
    QString senderName = index.data(message::ModelData::Author).toString();
    QString timestampString =
        formatTimestamp(index.data(message::ModelData::Timestamp).toLongLong());
    QString reactionsText =
        index.data(message::ModelData::Reactions).toString();
    QList<QString> picturePaths =
        index.data(message::ModelData::AllImageMedia).toStringList();
    QList<QString> attachmentNames =
        index.data(message::ModelData::AttachmentNames).toStringList();
    QString sharedLink = index.data(message::ModelData::SharedLink).toString();
    QList<QString> videoNames =
        index.data(message::ModelData::VideoNames).toStringList();
    QList<QString> audioNames =
        index.data(message::ModelData::AudioNames).toStringList();

    QList<IconTextPair> iconTextPairs{};

    {
        for (const auto& attachmentName : attachmentNames) {
            iconTextPairs.push_back(
                {"file-icon", "://resources/images/file.png", attachmentName});
        }

        if (!sharedLink.isEmpty()) {
            iconTextPairs.push_back(
                {"link-icon", "://resources/images/link.png", sharedLink});
        }

        for (const auto& videoName : videoNames) {
            iconTextPairs.push_back(
                {"video-icon", "://resources/images/video.png", videoName});
        }

        for (const auto& audioName : audioNames) {
            iconTextPairs.push_back(
                {"audio-icon", "://resources/images/audio.png", audioName});
        }
    }

    int availableWidthForPictures = opt.rect.width() * !picturePaths.isEmpty();
    int totalPictureHeight =
        calculateTotalPicturesHeight(picturePaths, availableWidthForPictures,
                                     parameters.m_margin,
                                     parameters.m_thumbnailQuality) +
        parameters.m_largeMargin;

    if (picturePaths.isEmpty()) {
        totalPictureHeight = 0;
    } else if (picturePaths.count() == 1) {
        int imageWidth =
            getPixmapFromCache(picturePaths.first(), availableWidthForPictures,
                               parameters.m_thumbnailQuality)
                .width() /
            parameters.m_thumbnailQuality;

        availableWidthForPictures = imageWidth - parameters.m_margin;
    }

    QSize totalIconTextPairSize;

    if (!iconTextPairs.isEmpty()) {
        totalIconTextPairSize = calculateTotalIconTextPairSize(
            iconTextPairs, opt.rect, parameters.m_iconSize, parameters.m_margin,
            parameters.m_largeMargin, opt.font);

        totalIconTextPairSize.setHeight(totalIconTextPairSize.height() +
                                        parameters.m_largeMargin +
                                        parameters.m_margin);
    }

    QRect contentBoundingBox = fitText(opt.font, opt.rect, content);

    if (content.isEmpty()) {
        contentBoundingBox.setHeight(0);
    }

    QRect nameBoundingBox = fitText(nameFont, opt.rect, senderName);

    QFont timestampFont(opt.font);
    timestampFont.setPointSize(timestampFont.pointSize() - 2);

    QRect timestampBoundingBox =
        fitText(timestampFont, opt.rect, timestampString);

    opt.rect.setWidth(vMax(
        vMin(opt.rect.width() + parameters.m_largeMargin,
             vMax(contentBoundingBox.width(), nameBoundingBox.width(),
                  timestampBoundingBox.width(), totalIconTextPairSize.width()) +
                 parameters.m_largeMargin),
        availableWidthForPictures));

    QColor bubbleColor = parameters.m_otherBubbleColor;

    QUuid senderIdentifier =
        index.data(message::ModelData::AuthorIdentifier).toUuid();
    QUuid ownerIdentifier =
        index.data(message::ModelData::OwnerIdentifier).toUuid();
    if (senderIdentifier == ownerIdentifier) {
        opt.rect.setLeft(originalRight - opt.rect.right());
        opt.rect.setRight(originalRight - parameters.m_margin);

        bubbleColor = parameters.m_ownBubbleColor;
    }

    QRect bubbleRect(
        opt.rect.adjusted(-parameters.m_margin, -parameters.m_margin,
                          parameters.m_margin, parameters.m_margin));
    bubbleRect.setHeight(contentBoundingBox.height() +
                         parameters.m_largeMargin + nameBoundingBox.height() +
                         parameters.m_margin + timestampBoundingBox.height() +
                         parameters.m_largeMargin + totalPictureHeight +
                         totalIconTextPairSize.height());

    QPainterPath path;
    path.addRoundedRect(bubbleRect, parameters.m_cornerRadius,
                        parameters.m_cornerRadius);
    painter->fillPath(path, bubbleColor);

    opt.rect.moveLeft(opt.rect.left() + parameters.m_margin);
    painter->setPen(parameters.m_textColor);

    painter->setFont(nameFont);
    drawText(painter, opt.rect, senderName);

    opt.rect.setTop(opt.rect.top() + nameBoundingBox.height());

    painter->setFont(opt.font);
    drawText(painter, opt.rect, content);

    opt.rect.setTop(opt.rect.top() + contentBoundingBox.height() +
                    parameters.m_largeMargin);

    if (picturePaths.count() > 0) {
        int picturesPerRow = vMin(picturePaths.count(), 3);
        int maxWidthPerPicture = (availableWidthForPictures -
                                  (picturesPerRow * parameters.m_margin)) /
                                     picturesPerRow -
                                 parameters.m_margin;
        int rowHeight = 0;

        int column = 0;
        for (const auto& path : picturePaths) {
            QPixmap picture = getPixmapFromCache(path, maxWidthPerPicture,
                                                 parameters.m_thumbnailQuality);

            int drawHeight =
                ((float)picture.height() / (float)picture.width()) *
                maxWidthPerPicture;

            rowHeight = vMax(drawHeight, rowHeight);

            painter->drawPixmap(
                opt.rect.left() +
                    (column * (maxWidthPerPicture + parameters.m_margin)),
                opt.rect.top(), maxWidthPerPicture, drawHeight, picture);

            if (++column == picturesPerRow) {
                opt.rect.setTop(opt.rect.top() + rowHeight +
                                parameters.m_margin);

                column = 0;
                rowHeight = 0;
            }
        }

        opt.rect.setTop(opt.rect.top() + rowHeight + parameters.m_largeMargin);
    }

    for (const auto& pair : iconTextPairs) {
        QString iconKey = QString("%1-%2-%3-%4")
                              .arg(pair.m_iconName)
                              .arg(parameters.m_textColor.red())
                              .arg(parameters.m_textColor.green())
                              .arg(parameters.m_textColor.blue());

        QPixmap icon;
        if (!QPixmapCache::find(iconKey, &icon)) {
            QImage iconImage =
                QImage(pair.m_iconPath)
                    .scaled(parameters.m_iconSize, parameters.m_iconSize,
                            Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

            for (int i = 0; i < iconImage.width(); i++) {
                for (int j = 0; j < iconImage.height(); j++) {
                    QColor pixelColor = iconImage.pixelColor(i, j);

                    iconImage.setPixelColor(
                        i, j,
                        QColor(
                            (pixelColor.red() * parameters.m_textColor.red()) /
                                255,
                            (pixelColor.green() *
                             parameters.m_textColor.green()) /
                                255,
                            (pixelColor.blue() *
                             parameters.m_textColor.blue()) /
                                255,
                            pixelColor.alpha()));
                }
            }

            icon = QPixmap::fromImage(iconImage);
            QPixmapCache::insert(iconKey, icon);
        }

        QRect iconRectangle(opt.rect);
        iconRectangle.setWidth(parameters.m_iconSize);
        iconRectangle.setHeight(parameters.m_iconSize);

        painter->drawPixmap(iconRectangle, icon);

        QRect textRectangle(opt.rect);
        textRectangle.setLeft(iconRectangle.right() + parameters.m_largeMargin);
        textRectangle.setRight(textRectangle.right() - parameters.m_margin);

        QRect nameBounds = QFontMetrics(opt.font).boundingRect(
            textRectangle, Qt::AlignTop | Qt::AlignLeft | Qt::TextWrapAnywhere,
            pair.m_text);
        painter->drawText(textRectangle,
                          Qt::AlignTop | Qt::AlignLeft | Qt::TextWrapAnywhere,
                          pair.m_text);

        opt.rect.setTop(opt.rect.top() +
                        vMax(parameters.m_iconSize, nameBounds.height()) +
                        parameters.m_margin);
    }

    if (!iconTextPairs.isEmpty()) {
        opt.rect.setTop(opt.rect.top() + parameters.m_largeMargin);
    }

    painter->save();
    painter->setPen(QColor(parameters.m_textColor.red(),
                           parameters.m_textColor.green(),
                           parameters.m_textColor.blue(), 128));
    painter->setFont(timestampFont);
    drawText(painter, opt.rect, timestampString);
    painter->restore();

    opt.rect.setTop(opt.rect.top() + timestampBoundingBox.height() +
                    parameters.m_margin);

    if (!reactionsText.isEmpty()) {
        QRect reactionsBounds = fitText(opt.font, opt.rect, reactionsText);

        QRect reactionsDrawBounds(opt.rect);
        reactionsDrawBounds.setLeft(opt.rect.right() - reactionsBounds.width());

        painter->save();

        QPainterPath path;
        path.addRoundedRect(reactionsDrawBounds.adjusted(
                                -parameters.m_margin, -parameters.m_margin,
                                parameters.m_margin, parameters.m_margin),
                            parameters.m_cornerRadius,
                            parameters.m_cornerRadius);
        painter->fillPath(path, parameters.m_otherBubbleColor);
        painter->setPen(QPen(parameters.m_darkOutline, 2));
        painter->drawPath(path);

        painter->restore();

        drawText(painter, reactionsDrawBounds, reactionsText);
    }
}

// Provides sizing for the standard conversation chat bubbles. This is the most
// complex of all, it handles everything that makes up a message: the author's
// name, content, timestamp, reactions and any additional attachments, such as
// files and pictures
QSize renderer::GenericMessageRenderer::sizeHint(
    const QStyleOptionViewItem& option,
    const QModelIndex& index,
    const RendererParameters& parameters) const {
    QStyleOptionViewItem opt(option);

    opt.rect.setWidth(vMax(300, parameters.m_parent->width() / 3));
    opt.rect = opt.rect.adjusted(parameters.m_margin, parameters.m_margin,
                                 -parameters.m_margin, -parameters.m_margin);

    int height = 0;

    QString content = index.data(message::ModelData::Content).toString();
    QString senderName = index.data(message::ModelData::Author).toString();
    QString timestampString =
        formatTimestamp(index.data(message::ModelData::Timestamp).toLongLong());
    QString reactionsText =
        index.data(message::ModelData::Reactions).toString();
    QString sharedLink = index.data(message::ModelData::SharedLink).toString();
    QList<QString> videoNames =
        index.data(message::ModelData::VideoNames).toStringList();
    QList<QString> attachmentNames =
        index.data(message::ModelData::AttachmentNames).toStringList();
    QList<QString> audioNames =
        index.data(message::ModelData::AudioNames).toStringList();

    QList<IconTextPair> iconTextPairs{};

    {
        for (const auto& attachmentName : attachmentNames) {
            iconTextPairs.push_back(
                {"file-icon", "://resources/images/file.png", attachmentName});
        }

        if (!sharedLink.isEmpty()) {
            iconTextPairs.push_back(
                {"link-icon", "://resources/images/link.png", sharedLink});
        }

        for (const auto& videoName : videoNames) {
            iconTextPairs.push_back(
                {"video-icon", "://resources/images/video.png", videoName});
        }

        for (const auto& audioName : audioNames) {
            iconTextPairs.push_back(
                {"audio-icon", "://resources/images/audio.png", audioName});
        }
    }

    QFont nameFont(opt.font);
    nameFont.setBold(true);

    QRect nameBoundingBox = fitText(nameFont, opt.rect, senderName);

    height += nameBoundingBox.height();

    QRect contentBoundingBox = fitText(opt.font, opt.rect, content);

    if (content.isEmpty()) {
        contentBoundingBox.setHeight(0);
    }

    height += contentBoundingBox.height() + parameters.m_largeMargin;

    if (content.isEmpty()) {
        contentBoundingBox.setHeight(0);
    }

    QList<QString> picturePaths =
        index.data(message::ModelData::AllImageMedia).toStringList();

    if (!picturePaths.isEmpty()) {
        height += calculateTotalPicturesHeight(picturePaths, opt.rect.width(),
                                               parameters.m_margin,
                                               parameters.m_thumbnailQuality) +
                  parameters.m_margin;
    }

    if (!iconTextPairs.isEmpty()) {
        height += calculateTotalIconTextPairSize(
                      iconTextPairs, opt.rect, parameters.m_iconSize,
                      parameters.m_margin, parameters.m_largeMargin, opt.font)
                      .height() +
                  parameters.m_largeMargin + parameters.m_margin;
    }

    QFont timestampFont(opt.font);
    timestampFont.setPointSize(timestampFont.pointSize() - 2);

    QRect timestampBoundingBox =
        fitText(timestampFont, opt.rect, timestampString);

    height += timestampBoundingBox.height() + parameters.m_largeMargin;

    if (!reactionsText.isEmpty()) {
        QRect reactionsBounds = fitText(opt.font, opt.rect, reactionsText);

        height += reactionsBounds.height() + parameters.m_margin;
    }

    return QSize(option.rect.width(), height + parameters.m_margin);
}

inline const QPixmap renderer::GenericMessageRenderer::getPixmapFromCache(
    const QString& path,
    const int& maxWidth,
    const double& quality) const {
    QString thumbnailPath =
        QString("thumbnail-%1-%2-%3").arg(quality).arg(maxWidth).arg(path);

    QPixmap original;
    QPixmap thumbnail;

    if (!QPixmapCache::find(thumbnailPath, &thumbnail)) {
        if (!QPixmapCache::find(path, &original)) {
            original = QPixmap(path);

            QPixmapCache::insert(path, original);
        }

        thumbnail = original.scaledToWidth(
            vMin(maxWidth, 300, original.width()) * quality,
            Qt::SmoothTransformation);

        QPixmapCache::insert(thumbnailPath, thumbnail);
    }

    return thumbnail;
}

inline const int renderer::GenericMessageRenderer::calculateTotalPicturesHeight(
    const QStringList& paths,
    const int& totalWidth,
    const int& margin,
    const double& quality) const {
    if (paths.isEmpty()) {
        return 0;
    }

    int picturesPerRow = vMin(paths.count(), 3);
    int maxWidthPerPicture =
        (totalWidth - (picturesPerRow * margin)) / picturesPerRow - margin;
    bool singlePicture = paths.count() == 1;

    int totalPicturesHeight = 0;
    int rowHeight = 0;

    int column = 0;
    for (const auto& path : paths) {
        QPixmap picture = getPixmapFromCache(path, maxWidthPerPicture, quality);

        if (singlePicture) {
            maxWidthPerPicture = (picture.width() / quality) - margin;
        }

        if (picture.width() == 0 || picture.height() == 0) {
            continue;
        }

        int drawHeight = ((float)picture.height() / (float)picture.width()) *
                         maxWidthPerPicture;

        rowHeight = vMax(drawHeight, rowHeight);

        if (++column == picturesPerRow) {
            totalPicturesHeight += rowHeight + margin;

            column = 0;
            rowHeight = 0;
        }
    }

    return totalPicturesHeight + rowHeight + margin;
}

const QSize renderer::GenericMessageRenderer::calculateTotalIconTextPairSize(
    const QList<IconTextPair>& items,
    const QRect& fitRectangle,
    const int iconSize,
    const int margin,
    const int largeMargin,
    const QFont& font) const {
    if (items.isEmpty()) {
        return {};
    }

    int maxWidth = 0;
    int totalHeight = 0;

    for (const auto& item : items) {
        QRect actualFit(fitRectangle);
        actualFit.setWidth(actualFit.width() - iconSize - largeMargin - margin);

        QRect textBounds = QFontMetrics(font).boundingRect(
            actualFit, Qt::AlignTop | Qt::AlignLeft | Qt::TextWrapAnywhere,
            item.m_text);

        totalHeight += vMax(iconSize, textBounds.height());
        maxWidth = vMax(maxWidth, iconSize + largeMargin + textBounds.width());
    }

    return {maxWidth, totalHeight};
}
