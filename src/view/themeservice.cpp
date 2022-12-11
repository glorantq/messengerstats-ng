#include "view/themeservice.h"

#include <QApplication>
#include <QPalette>

const theme::BaseDefinition theme::ThemeService::s_darkBase = {
    QColor(18, 18, 18),    QColor(28, 28, 28),    QColor(66, 66, 66),
    QColor(255, 255, 255), QColor(127, 127, 127),
};

const theme::BaseDefinition theme::ThemeService::s_lightBase = {
    QColor(255, 255, 255), QColor(235, 235, 235), QColor(235, 235, 235),
    QColor(46, 46, 46),    QColor(127, 127, 127),
};

const theme::ThemeDefinition theme::ThemeService::s_fallbackTheme = {
    s_darkBase,
    QColor(0, 132, 255),
};

const QList<QPair<QString, theme::BaseDefinition>>
    theme::ThemeService::s_bases = {
        {tr("Dark"), s_darkBase},
        {tr("Light"), s_lightBase},

        {tr("Pastel dark"),
         {
             QColor(29, 28, 26),
             QColor(39, 38, 36),
             QColor(69, 68, 66),
             QColor(243, 241, 245),
             QColor(127, 127, 127),
         }},

        {tr("Pastel light"),
         {
             QColor(254, 252, 243),
             QColor(219, 213, 214),
             QColor(206, 208, 208),
             QColor(46, 46, 46),
             QColor(127, 127, 127),
         }},
};

const QList<QPair<QString, QColor>> theme::ThemeService::s_colors = {
    {tr("Blue"), QColor(0, 132, 255)},
    {tr("Light blue"), QColor(71, 161, 255)},
    {tr("Orange"), QColor(255, 131, 16)},
    {tr("Pink"), QColor(255, 92, 161)},
    {tr("Purple"), QColor(165, 152, 255)},

    {tr("Pastel blue"), QColor(152, 168, 248)},
    {tr("Pastel light blue"), QColor(188, 206, 248)},
    {tr("Pastel orange"), QColor(250, 171, 120)},
    {tr("Pastel pink"), QColor(255, 225, 225)},
    {tr("Pastel purple"), QColor(127, 102, 157)},
};

void theme::ThemeService::apply(int baseIndex, int colorIndex) {
    if (baseIndex >= s_bases.size() || colorIndex >= s_colors.size()) {
        applyDefinition(s_fallbackTheme);
        return;
    }

    ThemeDefinition definition = {getByIndex(baseIndex, s_bases),
                                  getByIndex(colorIndex, s_colors)};

    applyDefinition(definition);
}

void theme::ThemeService::apply(QString baseName, QString colorName) {
    if (!containsKey(baseName, s_bases) || !containsKey(colorName, s_colors)) {
        applyDefinition(s_fallbackTheme);
        return;
    }

    applyDefinition(
        {getByName(baseName, s_bases), getByName(colorName, s_colors)});
}

void theme::ThemeService::applyDefinition(const ThemeDefinition& definition) {
    QPalette darkPalette;
    darkPalette.setColor(QPalette::Window, definition.m_base.m_backgroundColor);
    darkPalette.setColor(QPalette::WindowText, definition.m_base.m_textColor);
    darkPalette.setColor(QPalette::Disabled, QPalette::WindowText,
                         definition.m_base.m_textColor);
    darkPalette.setColor(QPalette::Base, definition.m_base.m_baseColor);
    darkPalette.setColor(QPalette::AlternateBase,
                         definition.m_base.m_alternateBaseColor);
    darkPalette.setColor(QPalette::ToolTipBase, definition.m_base.m_textColor);
    darkPalette.setColor(QPalette::ToolTipText, definition.m_base.m_textColor);
    darkPalette.setColor(QPalette::Text, definition.m_base.m_textColor);
    darkPalette.setColor(QPalette::Disabled, QPalette::Text,
                         definition.m_base.m_disabledColor);
    darkPalette.setColor(QPalette::Dark,
                         definition.m_base.m_baseColor.darker(250));
    darkPalette.setColor(QPalette::Shadow,
                         definition.m_base.m_baseColor.darker(200));
    darkPalette.setColor(QPalette::Button, definition.m_base.m_backgroundColor);
    darkPalette.setColor(QPalette::ButtonText, definition.m_base.m_textColor);
    darkPalette.setColor(QPalette::Disabled, QPalette::ButtonText,
                         definition.m_base.m_disabledColor);
    darkPalette.setColor(QPalette::BrightText, Qt::red);
    darkPalette.setColor(QPalette::Link, definition.m_primaryColor);
    darkPalette.setColor(QPalette::Highlight, definition.m_primaryColor);
    darkPalette.setColor(QPalette::Disabled, QPalette::Highlight,
                         definition.m_base.m_disabledColor.lighter(150));
    darkPalette.setColor(QPalette::HighlightedText,
                         definition.m_base.m_textColor);
    darkPalette.setColor(QPalette::Disabled, QPalette::HighlightedText,
                         definition.m_base.m_disabledColor);
    darkPalette.setColor(QPalette::PlaceholderText,
                         definition.m_base.m_textColor);

    QApplication::setPalette(darkPalette);
}

const QStringList theme::ThemeService::getBaseNames() {
    return getKeys(s_bases);
}

const QStringList theme::ThemeService::getColorNames() {
    return getKeys(s_colors);
}
