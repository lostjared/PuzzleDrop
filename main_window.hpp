#ifndef __MAIN_WINDOW_H__
#define __MAIN_WINDOW_H__

#include<QMainWindow>
#include<QKeyEvent>
#include<QTimer>
#include<QImage>
#include<QMenu>
#include<QAction>
#include<QMenuBar>
#include<QResizeEvent>

#include"puzzle_drop.hpp"

#define PUZZLEDROP_VERSION "v1.0.0"

class HighScores;

class GameWindow : public QMainWindow {
  Q_OBJECT
public:
    GameWindow();
    void paintEvent(QPaintEvent *e);
    void keyPressEvent(QKeyEvent *ke);
    void resizeEvent(QResizeEvent *re);
    void gameOver();
    void recalculateBlockSize();
    
public slots:
    void update();
    void proc();
    void setEasy();
    void setMedium();
    void setHard();
    void newGame();
    void showAbout();
    void showHowTo();
    void openScores();
  
private:
    puzzle::Grid grid;
    QTimer *timer, *background_proc;
    QImage blocks[9];
    QImage background[8];
    QMenu *file_menu, *options_menu, *help_menu, *diff_menu;
    QAction *file_menu_new, *file_menu_scores;
    QAction *diff_easy, *diff_med, *diff_hard;
    QAction *help_about, *help_howto;
    QImage loadAndScale(QString filename);
    int difficulty_level;
    bool game_started;
    bool first_game;
    HighScores *scores_window;
    int gridBlocksWidth;
    int gridBlocksHeight;
    int blockWidth;
    int blockHeight;
    int offsetX;
    int offsetY;
    double gridAspectRatio;
};





#endif

