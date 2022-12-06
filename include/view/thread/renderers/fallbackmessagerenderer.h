#pragma once

#include <QCoreApplication>

#include "view/thread/renderers/imessagerenderer.h"
#include "view/thread/threadlistmodel.h"

namespace renderer {

// The renderer used when no other renderer is available for a type, draws a
// centered message in red that draws my attention to the issue
class FallbackMessageRenderer : public IMessageRenderer {
    Q_DECLARE_TR_FUNCTIONS(FallbackMessageRenderer)

    // IMessageRenderer interface
   public:
    void paint(QPainter* painter,
               const QStyleOptionViewItem& option,
               const QModelIndex& index,
               const RendererParameters&) const override;
    QSize sizeHint(const QStyleOptionViewItem& option,
                   const QModelIndex& index,
                   const RendererParameters&) const override;

   private:
    // Utility function to convert a message type enum into a human-readable
    // string
    inline QString messageTypeToString(data::MessageType&) const;
};

};  // namespace renderer
