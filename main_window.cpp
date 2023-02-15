#include "main_window.hpp"
#include "high_scores.hpp"
#include<QPainter>
#include<QMessageBox>
#include<cstdlib>
#include<ctime>
#include<QTextStream>

using puzzle::BlockType;

GameWindow::GameWindow() : grid(1280/BLOCK_WIDTH, 720/BLOCK_HEIGHT) {
    scores_window = new HighScores(this);
    setWindowTitle("PuzzleDrop");
    setWindowIcon(QPixmap(":/img/green3.png").scaled(32, 32));
    setFixedSize(1280, 745);
    blocks[0] = QImage(":/img/red1.png");
    blocks[1] = QImage(":/img/red2.png");
    blocks[2] = QImage(":/img/red3.png");
    blocks[3] = QImage(":/img/green1.png");
    blocks[4] = QImage(":/img/green2.png");
    blocks[5] = QImage(":/img/green3.png");
    blocks[6] = QImage(":/img/blue1.png");
    blocks[7] = QImage(":/img/blue2.png");
    blocks[8] = QImage(":/img/blue3.png");
    background[0] = loadAndScale(":/img/level1.png");
    background[1] = loadAndScale(":/img/level2.png");
    background[2] = loadAndScale(":/img/level3.png");
    background[3] = loadAndScale(":/img/level4.png");
    background[4] = loadAndScale(":/img/level5.png");
    background[5] = loadAndScale(":/img/level6.png");
    background[6] = loadAndScale(":/img/level7.png");
    background[7] = loadAndScale(":/img/level8.png");
    file_menu = menuBar()->addMenu(tr("&File"));
    options_menu = menuBar()->addMenu(tr("&Options"));
    help_menu = menuBar()->addMenu(tr("&Help"));
    file_menu_new = new QAction(tr("&New Game"), this);
    file_menu->addAction(file_menu_new);
    file_menu_scores = new QAction(tr("High Scores"), this);
    file_menu->addAction(file_menu_scores);
    diff_menu = options_menu->addMenu(tr("Difficulty"));
    diff_easy = new QAction(tr("Easy"), this);
    diff_med = new QAction(tr("Medium"), this);
    diff_hard = new QAction(tr("Hard"), this);
    diff_easy->setCheckable(true);
    diff_easy->setChecked(true);
    diff_med->setCheckable(true);
    diff_med->setChecked(false);
    diff_hard->setCheckable(true);
    diff_hard->setChecked(false);
    diff_menu->addAction(diff_easy);
    diff_menu->addAction(diff_med);
    diff_menu->addAction(diff_hard);
    connect(diff_easy, SIGNAL(triggered()), this, SLOT(setEasy()));
    connect(diff_med, SIGNAL(triggered()), this, SLOT(setMedium()));
    connect(diff_hard, SIGNAL(triggered()), this, SLOT(setHard()));
    difficulty_level = 0;
    game_started = false;
    help_about = new QAction(tr("&About"), this);
    connect(help_about, SIGNAL(triggered()), this, SLOT(showAbout()));
    help_howto = new QAction(tr("&How to Play"), this);
    connect(help_howto, SIGNAL(triggered()), this, SLOT(showHowTo()));
    help_menu->addAction(help_howto);
    help_menu->addAction(help_about);
    connect(file_menu_new, SIGNAL(triggered()), this, SLOT(newGame()));
    connect(file_menu_scores, SIGNAL(triggered()), this, SLOT(openScores()));
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    background_proc = new QTimer(this);
    connect(background_proc, SIGNAL(timeout()), this, SLOT(proc()));
    first_game = true;
}

QImage GameWindow::loadAndScale(QString filename) {
    QImage img = QImage(filename);
    return img.scaled(1280, 745);
}


void GameWindow::paintEvent(QPaintEvent *e) {
    Q_UNUSED(e);
    QPainter paint(this);
    int offset_y = 25;
    paint.drawImage(0, 0, background[grid.level()-1]);
    for(int x = 0; x < grid.getWidth(); ++x) {
        for(int y = 0; y < grid.getHeight(); ++y) {
            puzzle::Block *b = grid.grid(x,y);
            if(b != 0 && b->getType() == puzzle::BlockType::BLOCK_NULL) {
                paint.fillRect(QRect(x*BLOCK_WIDTH+1, offset_y+(y*BLOCK_HEIGHT+1), BLOCK_WIDTH-1, BLOCK_HEIGHT-1), QBrush("#000000"));
            }
             else {
                int image = static_cast<int>(b->getType())-2;
                if(b->getType() == BlockType::BLOCK_CLEAR || b->getType() == BlockType::MATCH)
                    image = (rand()%9);
                paint.drawImage(x*BLOCK_WIDTH, offset_y+(y*BLOCK_HEIGHT), blocks[image]);
            }
        }
    }  
    puzzle::Piece &p = grid.getPiece();
    int b1 = static_cast<int>(p.blocks[0].getType())-2;
    int b2 = static_cast<int>(p.blocks[1].getType())-2;
    int b3 = static_cast<int>(p.blocks[2].getType())-2;
    if(p.blocks[0] == BlockType::MATCH)
        b1 = rand()%9;
    if(p.blocks[1] == BlockType::MATCH)
        b2 = rand()%9;
    if(p.blocks[2] == BlockType::MATCH)
        b3 = rand()%9;
    paint.drawImage(p.blocks[0].getX()*BLOCK_WIDTH, offset_y+(p.blocks[0].getY()*BLOCK_HEIGHT), blocks[b1]);
    paint.drawImage(p.blocks[1].getX()*BLOCK_WIDTH, offset_y+(p.blocks[1].getY()*BLOCK_HEIGHT), blocks[b2]);
    paint.drawImage(p.blocks[2].getX()*BLOCK_WIDTH, offset_y+(p.blocks[2].getY()*BLOCK_HEIGHT), blocks[b3]);
    if(game_started == false) {
        QFont font = paint.font();
        QPen pen = paint.pen();
        pen.setColor(QColor(QRgb(0xFFFFFF)));
        font.setPixelSize(70);
        font.setBold(true);
        paint.setFont(font);
        paint.setPen(pen);
        paint.drawText(425, offset_y+150, "Puzzle Drop");
        paint.drawText(235, offset_y+250, "Click New Game in Menu");
    } 
    if(game_started == true || first_game == false) {
        QFont font = paint.font();
        QPen pen = paint.pen();
        pen.setColor(QColor(QRgb(0xFFFFFF)));
        font.setPixelSize(25);
        font.setBold(true);
        paint.setFont(font);
        paint.setPen(pen);
        QString text;
        QTextStream stream(&text);
        stream << "Level: " << grid.level() << " Lines: " << grid.currentLines();
        paint.drawText(25, offset_y+60, text);
    }
}

void GameWindow::keyPressEvent(QKeyEvent *ke) {
    if(ke->key() == Qt::Key_Escape)
        exit(0);
    if(game_started == false)
        return;
    if(grid.gameOver() == true) {
        game_started = false;
        gameOver();
        repaint();
        return;
    }
    switch(ke->key()) {
        case Qt::Key_Left:
            grid.keyLeft();
            repaint();
            break;
        case Qt::Key_Right:
            grid.keyRight();
            repaint();
            break;
        case Qt::Key_Down:
            grid.keyDown();
            repaint();
            break;
        case Qt::Key_A:
            grid.keyShiftUp();
            repaint();
            break;
        case Qt::Key_S:
            grid.keyShiftDown();
            repaint();
            break;
        case Qt::Key_Z:
            grid.keyRotateLeft();
            repaint();
            break;
        case Qt::Key_X:
            grid.keyRotateRight();
            repaint();
            break;
    }
}

void GameWindow::gameOver() {
    background_proc->stop();
    timer->stop();
    game_started = false;
    first_game = false;
    diff_menu->setEnabled(true);
    QString name;
    if(scores_window->inputName(name)) {
        scores_window->addScore(name, grid.currentLines());
        scores_window->show();
    }
}
  

void GameWindow::update() {
    grid.keyDown();
    repaint();
    if(grid.gameOver() == true) {
        game_started = false;
        gameOver();
        repaint();
    }
}

void GameWindow::proc() {
    grid.procBlocks();
    grid.procMoveDown();
    repaint();  
}
  

void GameWindow::newGame() {
     int interval = 1000;
    if(difficulty_level == 1)
        interval = 750;
    else if(difficulty_level == 2)
        interval = 500;
    grid.clearGrid();
    game_started = true;
    timer->setInterval(interval);
    background_proc->setInterval(10);
    timer->start();
    background_proc->start();
    diff_menu->setEnabled(false);
}

void GameWindow::openScores() {
    scores_window->show();
}

void GameWindow::setEasy() {
    diff_easy->setChecked(true);
    diff_med->setChecked(false);
    diff_hard->setChecked(false);
    difficulty_level = 0;
}

void GameWindow::setMedium() {
    diff_easy->setChecked(false);
    diff_med->setChecked(true);
    diff_hard->setChecked(false);
    difficulty_level = 1;
}

void GameWindow::setHard() {
    diff_easy->setChecked(false);
    diff_med->setChecked(false);
    diff_hard->setChecked(true);
    difficulty_level = 2;
}

void GameWindow::showAbout() {
    QMessageBox box;
    QString info;
    QTextStream stream(&info);
    stream << tr("Puzzle Drop ") << PUZZLEDROP_VERSION << "\n" << tr("(C) 2022 LostSideDead Software\n Programmed by Jared Bruni\n");
    box.setText(info);
    QPixmap pix = QPixmap(":/img/red3.png");
    box.setIconPixmap(pix.scaled(QSize(64, 64)));
    box.setWindowIcon(QPixmap(":/img/red1.png").scaled(32, 32));
    box.setWindowTitle(tr("About Puzzle Drop"));
    box.exec();
}

void GameWindow::showHowTo() {
    QMessageBox box;
    box.setText(tr("You play by moving the blocks left and right with the arrow keys. You press A and S to shift the blocks, Z and X to rotate them Line up the pattern of the same color 1 2 3, or 3 2 1 Either horizontal, vertical, or diagonal line. There is a wildcard block that flashes different colors, it will match any block. Clear as many lines as you can."));
    QPixmap pix = QPixmap(":/img/blue3.png");
    box.setIconPixmap(pix.scaled(QSize(64, 64)));
    box.setWindowIcon(QPixmap(":/img/red1.png").scaled(32, 32));
    box.setWindowTitle(tr("How to Play"));
    box.exec();
}