#include "view/thread/messageitemdelegate.h"

#include <QPainter>
#include <QPainterPath>

#include "view/thread/threadlistmodel.h"

// TODO: Share type messages (the generic renderer can probably do it)

void MessageItemDelegate::paint(QPainter* painter,
                                const QStyleOptionViewItem& option,
                                const QModelIndex& index) const {
    data::MessageType messageType =
        (data::MessageType)index.data(message::ModelData::Type).toInt();

    painter->setRenderHints(QPainter::RenderHint::Antialiasing |
                            QPainter::RenderHint::TextAntialiasing |
                            QPainter::RenderHint::SmoothPixmapTransform);

    QStyleOptionViewItem opt(option);
    opt.font.setHintingPreference(QFont::PreferNoHinting);

#ifdef QT_DEBUG
    // When debugging paint the clipping area of each item a dark shade of red,
    // to see if offsets or sizing need to be adjusted
    painter->fillRect(opt.rect, QBrush(QColor(50, 20, 20)));
#endif

    std::shared_ptr<renderer::IMessageRenderer> associatedRenderer =
        m_rendererMapping.value(messageType, m_fallbackRenderer);

    if (associatedRenderer != nullptr) {
        associatedRenderer->paint(painter, opt, index, m_rendererParameters);
    } else {
        qCritical() << "Failed to obtain a renderer from the mappings, and the "
                       "fallback is nullptr!";
    }
}

QSize MessageItemDelegate::sizeHint(const QStyleOptionViewItem& option,
                                    const QModelIndex& index) const {
    data::MessageType messageType =
        (data::MessageType)index.data(message::ModelData::Type).toInt();

    QStyleOptionViewItem opt(option);
    opt.font.setHintingPreference(QFont::PreferNoHinting);

    std::shared_ptr<renderer::IMessageRenderer> associatedRenderer =
        m_rendererMapping.value(messageType, m_fallbackRenderer);

    if (associatedRenderer != nullptr) {
        return associatedRenderer->sizeHint(opt, index, m_rendererParameters);
    }

    qCritical() << "Failed to obtain a renderer from the mappings, and the "
                   "fallback is nullptr!";
    return {};
}
