#include "view/thread/renderers/genericmessagerenderer.h"

#include "view/thread/threadlistmodel.h"

#include <QPainterPath>

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
        vMin(opt.rect.width() + parameters.m_largeMargin,
             vMax(contentBoundingBox.width(), nameBoundingBox.width(),
                  timestampBoundingBox.width()) +
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
    bubbleRect.setHeight(contentBoundingBox.height() +
                         parameters.m_largeMargin + nameBoundingBox.height() +
                         parameters.m_margin + timestampBoundingBox.height() +
                         parameters.m_largeMargin);

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
