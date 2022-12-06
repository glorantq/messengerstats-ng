#include "view/thread/renderers/callmessagerenderer.h"

#include "view/thread/threadlistmodel.h"

#include <QPainterPath>

// Paints a call message, very similar to a regular message (has a chat bubble),
// but contains a telephone icon
void renderer::CallMessageRenderer::paint(
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
    QString durationString =
        formatDuration(index.data(message::ModelData::CallDuration).toUInt());

    QRect contentBoundingBox = fitText(opt.font, opt.rect, content);

    if (content.isEmpty()) {
        contentBoundingBox.setHeight(0);
    }

    QRect nameBoundingBox = fitText(nameFont, opt.rect, senderName);

    QFont timestampFont(opt.font);
    timestampFont.setPointSize(timestampFont.pointSize() - 2);

    QRect timestampBoundingBox =
        fitText(timestampFont, opt.rect, timestampString);

    QRect durationBoundingBox = fitText(opt.font, opt.rect, durationString);

    opt.rect.setWidth(
        vMin(opt.rect.width(),
             vMax(contentBoundingBox.width(), nameBoundingBox.width(),
                  timestampBoundingBox.width(), durationBoundingBox.width()) +
                 parameters.m_largeMargin + parameters.m_iconSize +
                 parameters.m_largeMargin));

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
    bubbleRect.setHeight(
        vMax(parameters.m_iconSize + parameters.m_largeMargin,
             contentBoundingBox.height() + parameters.m_largeMargin +
                 nameBoundingBox.height() + parameters.m_margin +
                 timestampBoundingBox.height() + parameters.m_largeMargin +
                 durationBoundingBox.height() + parameters.m_margin));

    QPainterPath path;
    path.addRoundedRect(bubbleRect, parameters.m_cornerRadius,
                        parameters.m_cornerRadius);
    painter->fillPath(path, bubbleColor);

    painter->setPen(parameters.m_textColor);

    QRect pixmapRect(opt.rect);
    pixmapRect.setWidth(parameters.m_iconSize);
    pixmapRect.setHeight(parameters.m_iconSize);
    pixmapRect.moveLeft(opt.rect.left() + parameters.m_margin);
    pixmapRect.moveTop(opt.rect.top() + parameters.m_largeMargin);

    // TODO: This right here should really be cached
    QImage callIconImage =
        QImage("://resources/images/call.png")
            .scaled(parameters.m_iconSize, parameters.m_iconSize,
                    Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    for (int i = 0; i < callIconImage.width(); i++) {
        for (int j = 0; j < callIconImage.height(); j++) {
            QColor pixelColor = callIconImage.pixelColor(i, j);

            callIconImage.setPixelColor(
                i, j,
                QColor(
                    (pixelColor.red() * parameters.m_textColor.red()) / 255,
                    (pixelColor.green() * parameters.m_textColor.green()) / 255,
                    (pixelColor.blue() * parameters.m_textColor.blue()) / 255,
                    pixelColor.alpha()));
        }
    }

    painter->drawPixmap(pixmapRect, QPixmap::fromImage(callIconImage));

    opt.rect.moveLeft(pixmapRect.right() + parameters.m_largeMargin);

    painter->setFont(nameFont);
    drawText(painter, opt.rect, senderName);

    opt.rect.setTop(opt.rect.top() + nameBoundingBox.height());

    painter->setFont(opt.font);
    drawText(painter, opt.rect, content);

    opt.rect.setTop(opt.rect.top() + contentBoundingBox.height() +
                    parameters.m_margin);

    drawText(painter, opt.rect, durationString);

    opt.rect.setTop(opt.rect.top() + durationBoundingBox.height() +
                    parameters.m_largeMargin);

    painter->save();
    painter->setPen(QColor(parameters.m_textColor.red(),
                           parameters.m_textColor.green(),
                           parameters.m_textColor.blue(), 128));
    painter->setFont(timestampFont);
    drawText(painter, opt.rect, timestampString);
    painter->restore();
}

// Very similar to the size hinting of regular messages, just contains an extra
// line of text and some added with by the icon
QSize renderer::CallMessageRenderer::sizeHint(
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
    QString durationString =
        formatDuration(index.data(message::ModelData::CallDuration).toUInt());

    QFont nameFont(opt.font);
    nameFont.setBold(true);

    QRect nameBoundingBox = fitText(nameFont, opt.rect, senderName);

    height += nameBoundingBox.height();

    QRect contentBoundingBox = fitText(opt.font, opt.rect, content);

    if (content.isEmpty()) {
        contentBoundingBox.setHeight(0);
    }

    height += contentBoundingBox.height() + parameters.m_largeMargin;

    QRect durationBoundingBox = fitText(opt.font, opt.rect, durationString);

    height += durationBoundingBox.height() + parameters.m_margin;

    QFont timestampFont(opt.font);
    timestampFont.setPointSize(timestampFont.pointSize() - 2);

    QRect timestampBoundingBox =
        fitText(timestampFont, opt.rect, timestampString);

    height += timestampBoundingBox.height() + parameters.m_largeMargin;

    height = vMax(parameters.m_iconSize + parameters.m_largeMargin,
                  height + parameters.m_margin);

    return QSize(option.rect.width(), height);
}
