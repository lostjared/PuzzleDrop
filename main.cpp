#include "main_window.hpp"
#include<QApplication>


int main(int argc, char **argv) {
    srand(static_cast<unsigned int>(time(0)));
    QApplication app(argc, argv);
    GameWindow game_window;
    game_window.show();
    return app.exec();
}
