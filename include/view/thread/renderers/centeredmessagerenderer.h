#pragma once

#include <QCoreApplication>

#include "view/thread/renderers/imessagerenderer.h"

namespace renderer {

// Shows a faded and centered message, only contains the content
class CenteredMessageRenderer : public IMessageRenderer {
    Q_DECLARE_TR_FUNCTIONS(CenteredMessageRenderer)

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
