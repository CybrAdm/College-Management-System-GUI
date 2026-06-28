#include <QApplication>
#include <QFont>
#include <QFontDatabase>
#include "MainWindow.h"
#include <QIcon>
#include <QDir>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Try modern fonts with fallbacks
    QFont font;
    QStringList fontFamilies = {"Segoe UI Variable", "Segoe UI", "Inter", "Noto Sans", "Arial"};
    for (const QString& f : fontFamilies) {
        font = QFont(f, 10);
        if (font.exactMatch()) break;
    }
    font.setStyleHint(QFont::SansSerif);
    font.setHintingPreference(QFont::PreferFullHinting);
    app.setFont(font);
    app.setApplicationName("College System");
    app.setApplicationVersion("1.0.0");
    app.setStyle("Fusion");

    // Apply palette matching current theme
    auto applyPalette = [&]() {
        QPalette pal = app.palette();
        if (g_lightMode) {
            pal.setColor(QPalette::Window,          QColor("#F2EDE4"));
            pal.setColor(QPalette::WindowText,      QColor("#1A1A2E"));
            pal.setColor(QPalette::Base,            QColor("#FFFFFF"));
            pal.setColor(QPalette::AlternateBase,   QColor("#FAF8F4"));
            pal.setColor(QPalette::Text,            QColor("#1A1A2E"));
            pal.setColor(QPalette::ButtonText,      QColor("#1A1A2E"));
            pal.setColor(QPalette::Button,          QColor("#FFFFFF"));
            pal.setColor(QPalette::Highlight,       QColor("#4361EE"));
            pal.setColor(QPalette::HighlightedText, QColor("#FFFFFF"));
            pal.setColor(QPalette::ToolTipBase,     QColor("#FFFFFF"));
            pal.setColor(QPalette::ToolTipText,     QColor("#1A1A2E"));
            pal.setColor(QPalette::PlaceholderText, QColor("#8888AA"));
        } else {
            pal.setColor(QPalette::Window,          QColor("#0A0C14"));
            pal.setColor(QPalette::WindowText,      QColor("#C8D8F0"));
            pal.setColor(QPalette::Base,            QColor("#0C0F1C"));
            pal.setColor(QPalette::AlternateBase,   QColor("#0F1325"));
            pal.setColor(QPalette::Text,            QColor("#C8D8F0"));
            pal.setColor(QPalette::ButtonText,      QColor("#C8D8F0"));
            pal.setColor(QPalette::Button,          QColor("#101420"));
            pal.setColor(QPalette::Highlight,       QColor("#00CFFF"));
            pal.setColor(QPalette::HighlightedText, QColor("#000A14"));
            pal.setColor(QPalette::ToolTipBase,     QColor("#0C0F1C"));
            pal.setColor(QPalette::ToolTipText,     QColor("#C8D8F0"));
            pal.setColor(QPalette::PlaceholderText, QColor("#3C4E6A"));
        }
        app.setPalette(pal);
    };
    applyPalette();

    MainWindow w;
    w.setWindowTitle("College Management System");
    w.setWindowIcon(QIcon(":/new/prefix1/download.png"));
    w.resize(1366, 820);
    w.setMinimumSize(1100, 700);
    w.show();

    return app.exec();
}
