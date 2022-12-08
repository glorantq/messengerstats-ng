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
        index.data(message::ModelData::Pictures).toStringList();
    QList<QString> attachmentNames =
        index.data(message::ModelData::AttachmentNames).toStringList();
    QString sharedLink = index.data(message::ModelData::SharedLink).toString();

    int availableWidthForPictures = opt.rect.width() * !picturePaths.isEmpty();
    int totalPictureHeight =
        calculateTotalPicturesHeight(picturePaths, availableWidthForPictures,
                                     parameters.m_margin) +
        parameters.m_largeMargin;

    if (picturePaths.isEmpty()) {
        totalPictureHeight = 0;
    }

    QSize totalAttachmentSize;

    if (!attachmentNames.isEmpty()) {
        totalAttachmentSize = calculateTotalAttachmentsSize(
            attachmentNames, opt.rect, parameters.m_iconSize,
            parameters.m_margin, parameters.m_largeMargin, opt.font);

        totalAttachmentSize.setHeight(totalAttachmentSize.height() +
                                      parameters.m_largeMargin +
                                      parameters.m_margin);
    }

    QSize sharedLinkSize;

    if (!sharedLink.isEmpty()) {
        QRect actualFit(opt.rect);
        actualFit.setWidth(actualFit.width() - parameters.m_iconSize -
                           parameters.m_largeMargin - parameters.m_margin);

        QRect textBounds = QFontMetrics(opt.font).boundingRect(
            actualFit, Qt::AlignTop | Qt::AlignLeft | Qt::TextWrapAnywhere,
            sharedLink);

        sharedLinkSize = {vMin(opt.rect.width(), parameters.m_iconSize +
                                                     parameters.m_largeMargin +
                                                     textBounds.width()),
                          vMax(parameters.m_iconSize, textBounds.height()) +
                              parameters.m_largeMargin + parameters.m_margin};
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

    opt.rect.setWidth(
        vMax(vMin(opt.rect.width() + parameters.m_largeMargin,
                  vMax(contentBoundingBox.width(), nameBoundingBox.width(),
                       timestampBoundingBox.width(),
                       totalAttachmentSize.width(), sharedLinkSize.width()) +
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
                         totalAttachmentSize.height() +
                         sharedLinkSize.height());

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
            QPixmap picture = getPixmapFromCache(path, maxWidthPerPicture);

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

        opt.rect.setTop(opt.rect.top() + rowHeight + parameters.m_margin +
                        parameters.m_largeMargin);
    }

    if (attachmentNames.count() > 0) {
        QString fileIconKey = QString("file-icon-%1-%2-%3")
                                  .arg(parameters.m_textColor.red())
                                  .arg(parameters.m_textColor.green())
                                  .arg(parameters.m_textColor.blue());

        QPixmap fileIcon;
        if (!QPixmapCache::find(fileIconKey, &fileIcon)) {
            QImage fileIconImage =
                QImage("://resources/images/file.png")
                    .scaled(parameters.m_iconSize, parameters.m_iconSize,
                            Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

            for (int i = 0; i < fileIconImage.width(); i++) {
                for (int j = 0; j < fileIconImage.height(); j++) {
                    QColor pixelColor = fileIconImage.pixelColor(i, j);

                    fileIconImage.setPixelColor(
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

            fileIcon = QPixmap::fromImage(fileIconImage);
            QPixmapCache::insert(fileIconKey, fileIcon);
        }

        for (const auto& name : attachmentNames) {
            QRect iconRectangle(opt.rect);
            iconRectangle.setWidth(parameters.m_iconSize);
            iconRectangle.setHeight(parameters.m_iconSize);

            painter->drawPixmap(iconRectangle, fileIcon);

            QRect textRectangle(opt.rect);
            textRectangle.setLeft(iconRectangle.right() +
                                  parameters.m_largeMargin);
            textRectangle.setRight(textRectangle.right() - parameters.m_margin);

            QRect nameBounds = QFontMetrics(opt.font).boundingRect(
                textRectangle,
                Qt::AlignTop | Qt::AlignLeft | Qt::TextWrapAnywhere, name);
            painter->drawText(
                textRectangle,
                Qt::AlignTop | Qt::AlignLeft | Qt::TextWrapAnywhere, name);

            opt.rect.setTop(opt.rect.top() +
                            vMax(parameters.m_iconSize, nameBounds.height()) +
                            parameters.m_margin);
        }

        opt.rect.setTop(opt.rect.top() + parameters.m_largeMargin);
    }

    if (!sharedLink.isEmpty()) {
        QString linkIconKey = QString("link-icon-%1-%2-%3")
                                  .arg(parameters.m_textColor.red())
                                  .arg(parameters.m_textColor.green())
                                  .arg(parameters.m_textColor.blue());

        QPixmap linkIcon;
        if (!QPixmapCache::find(linkIconKey, &linkIcon)) {
            QImage linkIconImage =
                QImage("://resources/images/link.png")
                    .scaled(parameters.m_iconSize, parameters.m_iconSize,
                            Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

            for (int i = 0; i < linkIconImage.width(); i++) {
                for (int j = 0; j < linkIconImage.height(); j++) {
                    QColor pixelColor = linkIconImage.pixelColor(i, j);

                    linkIconImage.setPixelColor(
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

            linkIcon = QPixmap::fromImage(linkIconImage);
            QPixmapCache::insert(linkIconKey, linkIcon);
        }

        QRect iconRectangle(opt.rect);
        iconRectangle.setWidth(parameters.m_iconSize);
        iconRectangle.setHeight(parameters.m_iconSize);

        painter->drawPixmap(iconRectangle, linkIcon);

        QRect textRectangle(opt.rect);
        textRectangle.setLeft(iconRectangle.right() + parameters.m_largeMargin);
        textRectangle.setRight(textRectangle.right() - parameters.m_margin);

        QRect nameBounds = QFontMetrics(opt.font).boundingRect(
            textRectangle, Qt::AlignTop | Qt::AlignLeft | Qt::TextWrapAnywhere,
            sharedLink);
        painter->drawText(textRectangle,
                          Qt::AlignTop | Qt::AlignLeft | Qt::TextWrapAnywhere,
                          sharedLink);

        opt.rect.setTop(opt.rect.top() +
                        vMax(parameters.m_iconSize, nameBounds.height()) +
                        parameters.m_margin + parameters.m_largeMargin);
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
        index.data(message::ModelData::Pictures).toStringList();

    if (!picturePaths.isEmpty()) {
        height += calculateTotalPicturesHeight(picturePaths, opt.rect.width(),
                                               parameters.m_margin) +
                  parameters.m_largeMargin;
    }

    QList<QString> attachmentNames =
        index.data(message::ModelData::AttachmentNames).toStringList();

    if (!attachmentNames.isEmpty()) {
        height += calculateTotalAttachmentsSize(
                      attachmentNames, opt.rect, parameters.m_iconSize,
                      parameters.m_margin, parameters.m_largeMargin, opt.font)
                      .height() +
                  parameters.m_largeMargin + parameters.m_margin;
    }

    if (!sharedLink.isEmpty()) {
        QRect actualFit(opt.rect);
        actualFit.setWidth(actualFit.width() - parameters.m_iconSize -
                           parameters.m_largeMargin - parameters.m_margin);

        QRect textBounds = QFontMetrics(opt.font).boundingRect(
            actualFit, Qt::AlignTop | Qt::AlignLeft | Qt::TextWrapAnywhere,
            sharedLink);

        height += vMax(parameters.m_iconSize, textBounds.height()) +
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
    const int& maxWidth) const {
    QPixmap cached;
    if (!QPixmapCache::find(path, &cached)) {
        cached = QPixmap(path).scaledToWidth(vMin(maxWidth, 300),
                                             Qt::SmoothTransformation);

        QPixmapCache::insert(path, cached);
    }

    return cached;
}

inline const int renderer::GenericMessageRenderer::calculateTotalPicturesHeight(
    const QStringList& paths,
    const int& totalWidth,
    const int& margin) const {
    if (paths.isEmpty()) {
        return 0;
    }

    int picturesPerRow = vMin(paths.count(), 3);
    int maxWidthPerPicture =
        (totalWidth - (picturesPerRow * margin)) / picturesPerRow - margin;

    int totalPicturesHeight = 0;
    int rowHeight = 0;

    int column = 0;
    for (const auto& path : paths) {
        QPixmap picture = getPixmapFromCache(path, maxWidthPerPicture);

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

const QSize renderer::GenericMessageRenderer::calculateTotalAttachmentsSize(
    const QStringList& names,
    const QRect& fitRectangle,
    const int iconSize,
    const int margin,
    const int largeMargin,
    const QFont& font) const {
    if (names.isEmpty()) {
        return {};
    }

    int maxWidth = 0;
    int totalHeight = 0;

    for (const auto& name : names) {
        QRect actualFit(fitRectangle);
        actualFit.setWidth(actualFit.width() - iconSize - largeMargin - margin);

        QRect textBounds = QFontMetrics(font).boundingRect(
            actualFit, Qt::AlignTop | Qt::AlignLeft | Qt::TextWrapAnywhere,
            name);

        totalHeight += vMax(iconSize, textBounds.height());
        maxWidth = vMax(maxWidth, iconSize + largeMargin + textBounds.width());
    }

    return {maxWidth, totalHeight};
}
