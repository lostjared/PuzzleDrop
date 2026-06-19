#include "main_window.hpp"
#include<QApplication>
#include<QColor>
#include<QPalette>


int main(int argc, char **argv) {
    srand(static_cast<unsigned int>(time(0)));
    QApplication app(argc, argv);
    app.setStyle("Fusion");
    app.setApplicationDisplayName("PuzzleDrop");

    QPalette palette;
    palette.setColor(QPalette::Window, QColor("#101820"));
    palette.setColor(QPalette::WindowText, QColor("#f4efe6"));
    palette.setColor(QPalette::Base, QColor("#0b1117"));
    palette.setColor(QPalette::AlternateBase, QColor("#14212b"));
    palette.setColor(QPalette::Text, QColor("#f4efe6"));
    palette.setColor(QPalette::Button, QColor("#1d2d37"));
    palette.setColor(QPalette::ButtonText, QColor("#f4efe6"));
    palette.setColor(QPalette::Highlight, QColor("#e76f51"));
    palette.setColor(QPalette::HighlightedText, QColor("#fffaf0"));
    app.setPalette(palette);

    app.setStyleSheet(
        "QMenuBar {"
        "  background: #101820;"
        "  color: #f4efe6;"
        "  spacing: 4px;"
        "  padding: 4px 8px;"
        "  border-bottom: 1px solid #2f414b;"
        "}"
        "QMenuBar::item {"
        "  background: transparent;"
        "  padding: 7px 12px;"
        "  border-radius: 4px;"
        "}"
        "QMenuBar::item:selected {"
        "  background: #233642;"
        "}"
        "QMenu {"
        "  background: #101820;"
        "  color: #f4efe6;"
        "  border: 1px solid #3a4d57;"
        "  padding: 6px;"
        "}"
        "QMenu::item {"
        "  padding: 7px 28px 7px 18px;"
        "  border-radius: 4px;"
        "}"
        "QMenu::item:selected {"
        "  background: #e76f51;"
        "  color: #101820;"
        "}"
        "QMenu::separator {"
        "  height: 1px;"
        "  background: #31434d;"
        "  margin: 6px 4px;"
        "}"
        "QDialog, QMessageBox, QInputDialog {"
        "  background: #101820;"
        "  color: #f4efe6;"
        "}"
        "QLabel { color: #f4efe6; }"
        "QLineEdit {"
        "  background: #0b1117;"
        "  color: #f4efe6;"
        "  border: 1px solid #526872;"
        "  border-radius: 4px;"
        "  padding: 6px;"
        "}"
        "QPushButton {"
        "  background: #e76f51;"
        "  color: #101820;"
        "  border: 0;"
        "  border-radius: 4px;"
        "  padding: 7px 14px;"
        "  font-weight: 600;"
        "}"
        "QPushButton:hover { background: #f08a68; }"
    );

    GameWindow game_window;
    game_window.show();
    return app.exec();
}
