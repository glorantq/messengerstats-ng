#include "view/mainwindow.h"

#include <QApplication>
#include <QFontDatabase>
#include <QLocale>
#include <QPixmapCache>
#include <QSettings>
#include <QStyleFactory>
#include <QTranslator>

#include "generated/version.h"
#include "view/settings.h"
#include "view/themeservice.h"

// TODO: Global contact list
// TODO: Global search

int main(int argc, char* argv[]) {
    qSetMessagePattern(
        "%{time hh:mm:ss} [%{type}] %{file}:%{line} > %{message}");

    QCoreApplication::setApplicationName("MessengerStats NG");
    QCoreApplication::setOrganizationName("glorantv");
    QCoreApplication::setOrganizationDomain("glorantv.hu");
    QCoreApplication::setApplicationVersion(MESSENGER_STATS_VERSION);

    QApplication a(argc, argv);

    a.setStyle(QStyleFactory::create("Fusion"));

    QSettings settings;
    int baseIndex = settings.value(SETTINGS_KEY_BACKGROUND_THEME, 0).toInt();
    int colorIndex = settings.value(SETTINGS_KEY_COLOR_SCHEME, 0).toInt();
    theme::ThemeService::apply(baseIndex, colorIndex);

    // Load the fonts used for the UI

    int fontId =
        QFontDatabase::addApplicationFont(":/resources/font/Inter-Regular.ttf");
    QFontDatabase::addApplicationFont(
        ":/resources/font/CascadiaCodePL-Regular.ttf");

    // Sadly this doesn't work yet due to a bug in Qt (on Windows)
    /*QFontDatabase::addApplicationFont(
        ":/resources/font/NotoColorEmoji-Regular.ttf");*/

    QFont::insertSubstitution(
        QFontDatabase::applicationFontFamilies(fontId).at(0),
        "Noto Color Emoji");

    QString family = QFontDatabase::applicationFontFamilies(fontId).at(0);
    QFont interFont(family);
    interFont.setPointSize(9);

    a.setFont(interFont);

    qDebug() << "UI font:" << QFontDatabase::applicationFontFamilies(0).at(0);
    qDebug() << "Monospaced font:"
             << QFontDatabase::applicationFontFamilies(1).at(0);
    /*qDebug() << "Emoji font:"
             << QFontDatabase::applicationFontFamilies(2).at(0);*/

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString& locale : uiLanguages) {
        const QString baseName = "MessengerStatsNG_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }

    unsigned int pixmapCacheSize =
        settings.value(SETTINGS_KEY_IMAGE_CACHE_SIZE, 1024).toUInt() * 1024;

    QPixmapCache::setCacheLimit(pixmapCacheSize);  // In kilobytes

    qDebug() << "Pixmap cache size set to:" << pixmapCacheSize / 1024 << "MB";

    MainWindow w;
    w.show();
    return a.exec();
}
