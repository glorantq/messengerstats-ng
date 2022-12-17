#pragma once

#include <QCoreApplication>

#include "view/thread/renderers/imessagerenderer.h"

namespace renderer {

struct IconTextPair {
    QString m_iconName;
    QString m_iconPath;
    QString m_text;
};

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

   private:
    inline const QPixmap getPixmapFromCache(const QString& path,
                                            const int& maxWidth,
                                            const double& quality) const;
    inline const int calculateTotalPicturesHeight(const QStringList& paths,
                                                  const int& totalWidth,
                                                  const int& margin,
                                                  const double& quality) const;
    inline const QSize calculateTotalIconTextPairSize(
        const QList<IconTextPair>& items,
        const QRect& fitRectangle,
        const int iconSize,
        const int margin,
        const int largeMargin,
        const QFont& font) const;
};

};  // namespace renderer
