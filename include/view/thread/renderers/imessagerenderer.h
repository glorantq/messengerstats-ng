#pragma once

#include <QCoreApplication>
#include <QDateTime>
#include <QPainter>
#include <QStyleOptionViewItem>

namespace renderer {

// Struct describing style parameters used for drawing
struct RendererParameters {
    QWidget* m_parent = nullptr;

    // Colour for chat bubbles from other people
    QColor m_otherBubbleColor = QColor(26, 26, 26);

    // Colour for chat bubbles sent by the owner of the data takeout
    QColor m_ownBubbleColor = QColor(0, 132, 255);

    // Colour of all text
    QColor m_textColor = QColor(239, 239, 239);

    // This should be set to the same colour as the window background, to
    // recreate the effect of reaction bubbles "cutting into" the main message
    // bubble
    QColor m_darkOutline = QColor(18, 18, 18);

    // Size of an icon if it is displayed in a message bubble (used for calls,
    // attachments and links)
    int m_iconSize = 24;

    // The margin used when painting and size hinting elements, with
    // m_largeMargin being nicer to write than m_margin * 2 everywhere
    int m_margin = 3;
    int m_largeMargin = m_margin * 2;

    // The radius of rounded rectangles
    int m_cornerRadius = 10;

    int m_textSize = 9;

    double m_thumbnailQuality = 1.;
};

// Superclass for all message renderers. Technically not an interface as it
// contains some helper methods shared between renderers, but I call it an
// interface regardless.
class IMessageRenderer {
    Q_DECLARE_TR_FUNCTIONS(IMessageRenderer)

   public:
    virtual void paint(QPainter* painter,
                       const QStyleOptionViewItem& option,
                       const QModelIndex& index,
                       const RendererParameters&) const = 0;

    virtual QSize sizeHint(const QStyleOptionViewItem& option,
                           const QModelIndex& index,
                           const RendererParameters&) const = 0;

   protected:
    // Formats a timestamp into human-readable text
    inline QString formatTimestamp(unsigned long long timestamp) const {
        return QDateTime::fromMSecsSinceEpoch(timestamp).toString(
            tr("yyyy. MM. dd. hh:mm:ss"));
    }

    inline QString formatDuration(unsigned int duration) const {
        unsigned int seconds = duration % 60;
        unsigned int minutes = duration / 60;

        return tr("%1 minutes, %2 seconds").arg(minutes).arg(seconds);
    }

    // Calculates the size of a string when fit into the provided rectangle
    inline QRect fitText(const QFont& font,
                         const QRect& rect,
                         const QString& text) const {
        return QFontMetrics(font).boundingRect(
            rect, Qt::AlignTop | Qt::AlignLeft | Qt::TextWordWrap, text);
    }

    // Draws the provided text inside the provided rectangle, aligned to the top
    // left corner
    inline void drawText(QPainter*& painter,
                         const QRect& rect,
                         const QString& text) const {
        painter->drawText(rect, Qt::AlignTop | Qt::AlignLeft | Qt::TextWordWrap,
                          text);
    }

    // Draws the provided text inside the provided rectangle, aligned to center
    // both horizontally and vertically
    inline void drawCenteredText(QPainter*& painter,
                                 const QRect& rect,
                                 const QString& text) const {
        painter->drawText(
            rect, Qt::AlignVCenter | Qt::AlignHCenter | Qt::TextWordWrap, text);
    }

    // Variadic maximum function, looks nicer than nesting qMax calls
    template <typename T, typename... Args>
    constexpr inline T vMax(const T& first, const Args&... args) const {
        std::vector<T> v = {args...};

        T max = first;
        for (const auto& e : v) {
            if (e > max) {
                max = e;
            }
        }

        return max;
    }

    // Variadic minimum function, looks nicer than nesting qMax calls
    template <typename T, typename... Args>
    constexpr inline T vMin(const T& first, const Args&... args) const {
        std::vector<T> v = {args...};

        T min = first;
        for (const auto& e : v) {
            if (e < min) {
                min = e;
            }
        }

        return min;
    }
};

};  // namespace renderer
