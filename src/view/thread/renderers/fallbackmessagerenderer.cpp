#include "view/thread/renderers/fallbackmessagerenderer.h"

// Handles painting for message types that don't have a renderer implemented.
// This is again, the same as the centered message renderer, just the content is
// different and it's drawn in red
void renderer::FallbackMessageRenderer::paint(
    QPainter* painter,
    const QStyleOptionViewItem& option,
    const QModelIndex& index,
    const RendererParameters& parameters) const {
    QStyleOptionViewItem opt(option);

    QRect textClipBounds(opt.rect);
    textClipBounds.setWidth(parameters.m_parent->width() * 0.9);
    textClipBounds.moveCenter(opt.rect.center());

    data::MessageType messageType =
        (data::MessageType)index.data(message::ModelData::Type).toInt();
    QString typeString = messageTypeToString(messageType);

    QString fallbackString =
        tr("No rendering defined for this message type! (%1)").arg(typeString);

    painter->save();
    painter->setPen(QPen(QColor(255, 105, 97)));
    drawCenteredText(painter, textClipBounds, fallbackString);
    painter->restore();
}

// Provides sizing for the fallback message. Same as a centered message really;
// only the content is different
QSize renderer::FallbackMessageRenderer::sizeHint(
    const QStyleOptionViewItem& option,
    const QModelIndex& index,
    const RendererParameters& parameters) const {
    QStyleOptionViewItem opt(option);
    opt.rect.setWidth(parameters.m_parent->width() * 0.9);

    data::MessageType messageType =
        (data::MessageType)index.data(message::ModelData::Type).toInt();
    QString typeString = messageTypeToString(messageType);

    QString fallbackString =
        tr("No rendering defined for this message type! (%1)").arg(typeString);

    QRect contentBounds = fitText(opt.font, opt.rect, fallbackString);

    return {option.rect.width(),
            contentBounds.height() + parameters.m_largeMargin * 4};
}

QString renderer::FallbackMessageRenderer::messageTypeToString(
    data::MessageType& messageType) const {
    QString typeString = tr("unknown");

    switch (messageType) {
        case data::MessageType::UnknownMessageType:
            typeString = tr("unknown");
            break;

        case data::MessageType::Generic:
            typeString = tr("generic");
            break;

        case data::MessageType::Share:
            typeString = tr("share");
            break;

        case data::MessageType::Call:
            typeString = tr("call");
            break;

        case data::MessageType::Subscribe:
            typeString = tr("subscribe");
            break;

        case data::MessageType::Unsubscribe:
            typeString = tr("ubsubscribe");
            break;

        case data::MessageType::ClassifiedAsSystem:
            typeString = tr("classified as system");
            break;

        case data::MessageType::NicknameChange:
            typeString = tr("nickanme change");
            break;
    }

    return typeString;
}
