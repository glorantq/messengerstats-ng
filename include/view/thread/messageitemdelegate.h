#pragma once

#include "model/message.h"
#include "view/settings.h"
#include "view/thread/renderers/callmessagerenderer.h"
#include "view/thread/renderers/centeredmessagerenderer.h"
#include "view/thread/renderers/fallbackmessagerenderer.h"
#include "view/thread/renderers/genericmessagerenderer.h"
#include "view/thread/renderers/imessagerenderer.h"

#include <QMap>
#include <QPainter>
#include <QSettings>
#include <QStyledItemDelegate>

#include <memory>

class MessageItemDelegate : public QStyledItemDelegate {
    Q_OBJECT
   private:
    renderer::RendererParameters m_rendererParameters{};

    const QMap<data::MessageType, std::shared_ptr<renderer::IMessageRenderer>>
        m_rendererMapping{
            {data::MessageType::Generic,
             std::make_shared<renderer::GenericMessageRenderer>()},
            {data::MessageType::Call,
             std::make_shared<renderer::CallMessageRenderer>()},
            {data::MessageType::Subscribe,
             std::make_shared<renderer::CenteredMessageRenderer>()},
            {data::MessageType::Unsubscribe,
             std::make_shared<renderer::CenteredMessageRenderer>()},
            {data::MessageType::ClassifiedAsSystem,
             std::make_shared<renderer::CenteredMessageRenderer>()},
            {data::MessageType::NicknameChange,
             std::make_shared<renderer::CenteredMessageRenderer>()},
            {data::MessageType::Share,
             std::make_shared<renderer::GenericMessageRenderer>()},
        };

    const std::shared_ptr<renderer::IMessageRenderer> m_fallbackRenderer =
        std::make_shared<renderer::FallbackMessageRenderer>();

    // QAbstractItemDelegate interface
   public:
    MessageItemDelegate(QWidget* parent) {
        m_rendererParameters.m_parent = parent;

        // These colours have sensible defaults set, but they are overridden
        // here by the palette of the parent widget to blend in with the
        // application's style, to allow easy theme switching (and because it
        // makes sense to use the parent's palette instead of defining an entire
        // new colour set just for messages)

        m_rendererParameters.m_ownBubbleColor =
            parent->palette().color(QPalette::Highlight);
        m_rendererParameters.m_textColor =
            parent->palette().color(QPalette::Text);
        m_rendererParameters.m_darkOutline =
            parent->palette().color(QPalette::Window);
        m_rendererParameters.m_otherBubbleColor =
            parent->palette().color(QPalette::Base);

        QSettings settings;

        int textSize =
            settings
                .value(SETTINGS_KEY_TEXT_SIZE, m_rendererParameters.m_textSize)
                .toInt();
        m_rendererParameters.m_textSize = textSize;

        double thumbnailQuality =
            settings
                .value(SETTINGS_KEY_THUMBNAIL_QUALITY,
                       m_rendererParameters.m_thumbnailQuality)
                .toDouble();
        m_rendererParameters.m_thumbnailQuality = thumbnailQuality;
    }

    // Main rendering method of the delegate set on the list, it's funny because
    // this delegate just delegates forward to other methods for the actual
    // painting and size hinting. Items are rendered at their hinted heights and
    // the full width of the view; this must be kept in mind when writing
    // drawing code, so some positioning has to take place here (for example:
    // chat bubbles can be aligned both to the right or the left, depending on
    // the center)
    void paint(QPainter* painter,
               const QStyleOptionViewItem& option,
               const QModelIndex& index) const override;

    // The same as above, delegates forward based on the type of message. Size
    // hinting is used to tell the list view the amount of space to reserve for
    // each item. The option parameter contains the clipping rectangle of the
    // full available area. This is only really used for height calculation
    // here; the original width of the clipping area is always used so clicks
    // can be handled correctly, and the actual positioning of the elements is
    // done at render-time. Returning a correct height from this is important
    // though.
    QSize sizeHint(const QStyleOptionViewItem& option,
                   const QModelIndex& index) const override;

   public slots:
    // When this slot is called, we update the renderer parameters with the
    // newly changed colours to make on-the-fly theme changes possible
    void onAppColorSchemeChanged();
};
