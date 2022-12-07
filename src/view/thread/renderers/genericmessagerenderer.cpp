#include "view/thread/renderers/genericmessagerenderer.h"

#include "view/thread/threadlistmodel.h"

#include <QImageReader>
#include <QPainterPath>
#include <QPixmap>
#include <QPixmapCache>

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

    int availableWidthForPictures = opt.rect.width() * !picturePaths.isEmpty();
    int totalPictureHeight =
        calculateTotalPicturesHeight(picturePaths, availableWidthForPictures,
                                    parameters.m_margin) +
        parameters.m_largeMargin;

    if (picturePaths.isEmpty()) {
        totalPictureHeight = 0;
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
                       timestampBoundingBox.width()) +
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
                         parameters.m_largeMargin + totalPictureHeight);

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
