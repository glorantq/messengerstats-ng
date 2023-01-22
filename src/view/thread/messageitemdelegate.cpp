#include "view/thread/messageitemdelegate.h"

#include <QPainter>
#include <QPainterPath>

#include "view/thread/threadlistmodel.h"

void MessageItemDelegate::paint(QPainter* painter,
                                const QStyleOptionViewItem& option,
                                const QModelIndex& index) const {
    data::MessageType messageType =
        (data::MessageType)index.data(message::ModelData::Type).toInt();

    painter->setRenderHints(QPainter::RenderHint::Antialiasing |
                            QPainter::RenderHint::TextAntialiasing);

    QStyleOptionViewItem opt(option);
    opt.font.setHintingPreference(QFont::PreferNoHinting);
    opt.font.setPointSize(m_rendererParameters.m_textSize);
    opt.fontMetrics = QFontMetrics(opt.font);

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
    opt.font.setPointSize(m_rendererParameters.m_textSize);
    opt.fontMetrics = QFontMetrics(opt.font);

    std::shared_ptr<renderer::IMessageRenderer> associatedRenderer =
        m_rendererMapping.value(messageType, m_fallbackRenderer);

    if (associatedRenderer != nullptr) {
        return associatedRenderer->sizeHint(opt, index, m_rendererParameters);
    }

    qCritical() << "Failed to obtain a renderer from the mappings, and the "
                   "fallback is nullptr!";
    return {};
}

void MessageItemDelegate::onAppColorSchemeChanged() {
    m_rendererParameters.m_ownBubbleColor =
        m_rendererParameters.m_parent->palette().color(QPalette::Highlight);
    m_rendererParameters.m_textColor =
        m_rendererParameters.m_parent->palette().color(QPalette::Text);
    m_rendererParameters.m_darkOutline =
        m_rendererParameters.m_parent->palette().color(QPalette::Window);
    m_rendererParameters.m_otherBubbleColor =
        m_rendererParameters.m_parent->palette().color(QPalette::Base);
}
