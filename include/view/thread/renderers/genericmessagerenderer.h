#pragma once

#include <QCoreApplication>

#include "view/thread/renderers/imessagerenderer.h"

namespace renderer {

// Responsible for regular chat messages, drawing the sender's name, content,
// reactions, timestamp and any additional attachments a message might have
class GenericMessageRenderer : public IMessageRenderer {
    Q_DECLARE_TR_FUNCTIONS(GenericMessageRenderer)

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
