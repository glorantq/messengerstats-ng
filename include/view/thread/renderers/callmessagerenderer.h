#pragma once

#include <QCoreApplication>

#include "view/thread/renderers/imessagerenderer.h"

namespace renderer {

// Renderer for call type messages, shows the person's name, the call duration,
// timestamp and a telephone icon
class CallMessageRenderer : public IMessageRenderer {
    Q_DECLARE_TR_FUNCTIONS(CallMessageRenderer)

    // IMessageRenderer interface
   public:
    void paint(QPainter* painter,
               const QStyleOptionViewItem& option,
               const QModelIndex& index,
               const RendererParameters&) const override;
    QSize sizeHint(const QStyleOptionViewItem& option,
                   const QModelIndex& index,
                   const RendererParameters&) const override;
};

};  // namespace renderer
