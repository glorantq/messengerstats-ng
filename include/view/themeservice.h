#pragma once

#include <QColor>
#include <QCoreApplication>
#include <QList>
#include <QPair>

namespace theme {

struct BaseDefinition {
    QColor m_backgroundColor;
    QColor m_baseColor;
    QColor m_alternateBaseColor;
    QColor m_textColor;
    QColor m_disabledColor;
};

struct ThemeDefinition {
    BaseDefinition m_base;
    QColor m_primaryColor;
};

class ThemeService {
    Q_DECLARE_TR_FUNCTIONS(ThemeService);

   private:
    static const BaseDefinition s_lightBase;
    static const BaseDefinition s_darkBase;

    static const ThemeDefinition s_fallbackTheme;

    static const QList<QPair<QString, BaseDefinition>> s_bases;
    static const QList<QPair<QString, QColor>> s_colors;

    template <typename T>
    static const T getByName(const QString& key,
                             const QList<QPair<QString, T>>& list) {
        for (const auto& pair : list) {
            if (pair.first == key) {
                return pair.second;
            }
        }

        return {};
    }

    template <typename T>
    static const T getByIndex(const int& index,
                              const QList<QPair<QString, T>>& list) {
        return list[index].second;
    }

    template <typename T>
    static const QStringList getKeys(const QList<QPair<QString, T>>& list) {
        QStringList ret;

        for (const auto& pair : list) {
            ret.push_back(pair.first);
        }

        return ret;
    }

    template <typename T>
    static bool containsKey(const QString& key,
                            const QList<QPair<QString, T>>& list) {
        for (const auto& pair : list) {
            if (pair.first == key) {
                return true;
            }
        }

        return false;
    }

   public:
    static void apply(int baseIndex, int colorIndex);
    static void apply(QString baseName, QString colorName);
    static void applyDefinition(const ThemeDefinition&);

    static const QStringList getBaseNames();
    static const QStringList getColorNames();
};

};  // namespace theme
