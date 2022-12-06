#include "view/mainwindow.h"

#include <QApplication>
#include <QFontDatabase>
#include <QLocale>
#include <QStyleFactory>
#include <QTranslator>

#include "generated/version.h"

int main(int argc, char* argv[]) {
    qSetMessagePattern(
        "%{time hh:mm:ss} [%{type}] %{file}:%{line} > %{message}");

    QCoreApplication::setApplicationName("MessengerStats NG");
    QCoreApplication::setOrganizationName("glorantv");
    QCoreApplication::setOrganizationDomain("glorantv.hu");
    QCoreApplication::setApplicationVersion(MESSENGER_STATS_VERSION);

    QApplication a(argc, argv);

    // Initialise the colour scheme of the application to closely mimic that of
    // Messenger, just for fun

    // TODO: Maybe more themes?

    a.setStyle(QStyleFactory::create("Fusion"));

    QPalette darkPalette;
    darkPalette.setColor(QPalette::Window, QColor(18, 18, 18));
    darkPalette.setColor(QPalette::WindowText, QColor(239, 239, 239));
    darkPalette.setColor(QPalette::Disabled, QPalette::WindowText,
                         QColor(127, 127, 127));
    darkPalette.setColor(QPalette::Base, QColor(28, 28, 28));
    darkPalette.setColor(QPalette::AlternateBase, QColor(66, 66, 66));
    darkPalette.setColor(QPalette::ToolTipBase, QColor(239, 239, 239));
    darkPalette.setColor(QPalette::ToolTipText, QColor(239, 239, 239));
    darkPalette.setColor(QPalette::Text, QColor(239, 239, 239));
    darkPalette.setColor(QPalette::Disabled, QPalette::Text,
                         QColor(127, 127, 127));
    darkPalette.setColor(QPalette::Dark, QColor(18, 18, 18));
    darkPalette.setColor(QPalette::Shadow, QColor(20, 20, 20));
    darkPalette.setColor(QPalette::Button, QColor(18, 18, 18));
    darkPalette.setColor(QPalette::ButtonText, QColor(239, 239, 239));
    darkPalette.setColor(QPalette::Disabled, QPalette::ButtonText,
                         QColor(127, 127, 127));
    darkPalette.setColor(QPalette::BrightText, Qt::red);
    darkPalette.setColor(QPalette::Link, QColor(0, 132, 255));
    darkPalette.setColor(QPalette::Highlight, QColor(0, 132, 255));
    darkPalette.setColor(QPalette::Disabled, QPalette::Highlight,
                         QColor(80, 80, 80));
    darkPalette.setColor(QPalette::HighlightedText, QColor(239, 239, 239));
    darkPalette.setColor(QPalette::Disabled, QPalette::HighlightedText,
                         QColor(127, 127, 127));
    darkPalette.setColor(QPalette::PlaceholderText, QColor(239, 239, 239));

    a.setPalette(darkPalette);

    // Load the fonts used for the UI

    int fontId =
        QFontDatabase::addApplicationFont(":/resources/font/Inter-Regular.ttf");
    QFontDatabase::addApplicationFont(
        ":/resources/font/CascadiaCodePL-Regular.ttf");
    QFontDatabase::addApplicationFont(
        ":/resources/font/NotoColorEmoji-Regular.ttf");

    // Sadly this doesn't work yet due to a bug in Qt

    /*QFont::insertSubstitution(
        QFontDatabase::applicationFontFamilies(fontId).at(0),
        QFontDatabase::applicationFontFamilies(2).at(0));*/

    QString family = QFontDatabase::applicationFontFamilies(fontId).at(0);
    QFont interFont(family);
    interFont.setPointSize(9);

    a.setFont(interFont);

    qDebug() << "UI font:" << QFontDatabase::applicationFontFamilies(0).at(0);
    qDebug() << "Monospaced font:"
             << QFontDatabase::applicationFontFamilies(1).at(0);
    qDebug() << "Emoji font:"
             << QFontDatabase::applicationFontFamilies(2).at(0);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString& locale : uiLanguages) {
        const QString baseName = "MessengerStatsNG_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }

    MainWindow w;
    w.show();
    return a.exec();
}
