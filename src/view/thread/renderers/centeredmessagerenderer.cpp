#include "view/thread/renderers/centeredmessagerenderer.h"

#include "view/thread/threadlistmodel.h"

// Paints the content of a message centered, with the text colour set to
// semi-transparent. Used for group joins and leaves
void renderer::CenteredMessageRenderer::paint(
    QPainter* painter,
    const QStyleOptionViewItem& option,
    const QModelIndex& index,
    const RendererParameters& parameters) const {
    QStyleOptionViewItem opt(option);

    QRect textClipBounds(opt.rect);
    textClipBounds.setWidth(parameters.m_parent->width() * 0.9);
    textClipBounds.moveCenter(opt.rect.center());

    QString content = index.data(message::ModelData::Content).toString();

    painter->save();
    painter->setPen(QPen(QColor(parameters.m_textColor.red(),
                                parameters.m_textColor.green(),
                                parameters.m_textColor.blue(), 128)));
    drawCenteredText(painter, textClipBounds, content);
    painter->restore();
}

// Provides size hints for centered messages. It's just the wrapped text height
// and some generous amount of padding on the top and bottom
QSize renderer::CenteredMessageRenderer::sizeHint(
    const QStyleOptionViewItem& option,
    const QModelIndex& index,
    const RendererParameters& parameters) const {
    QStyleOptionViewItem opt(option);
    opt.rect.setWidth(parameters.m_parent->width() * 0.9);

    QString content = index.data(message::ModelData::Content).toString();

    QRect contentBounds = fitText(opt.font, opt.rect, content);

    return {option.rect.width(),
            contentBounds.height() + parameters.m_largeMargin * 4};
}
