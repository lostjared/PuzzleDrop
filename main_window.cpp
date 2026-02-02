#include "main_window.hpp"
#include "high_scores.hpp"
#include<QPainter>
#include<QMessageBox>
#include<cstdlib>
#include<ctime>
#include<QTextStream>

using puzzle::BlockType;
static const int DEFAULT_BLOCK_WIDTH = 64;
static const int DEFAULT_BLOCK_HEIGHT = 32;

GameWindow::GameWindow() : grid(1280/DEFAULT_BLOCK_WIDTH, 720/DEFAULT_BLOCK_HEIGHT) {
    scores_window = new HighScores(this);
    setWindowTitle("PuzzleDrop");
    setWindowIcon(QPixmap(":/img/green3.png").scaled(32, 32));
    gridBlocksWidth = 1280 / DEFAULT_BLOCK_WIDTH;  
    gridBlocksHeight = 720 / DEFAULT_BLOCK_HEIGHT; 
    gridAspectRatio = (double)(gridBlocksWidth * DEFAULT_BLOCK_WIDTH) / (double)(gridBlocksHeight * DEFAULT_BLOCK_HEIGHT);

    blockWidth = DEFAULT_BLOCK_WIDTH;
    blockHeight = DEFAULT_BLOCK_HEIGHT;
    offsetX = 0;
    offsetY = 25; 

    setMinimumSize(640, 400);
    resize(1280, 745);
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
    
    
    for(int i = 0; i < 3; ++i) {
        lastFrameMatchColor[i] = rand() % 9;
    }
    for(int x = 0; x < 20; ++x) {
        for(int y = 0; y < 30; ++y) {
            lastFrameGridMatchColor[x][y] = rand() % 9;
        }
    }
}

QImage GameWindow::loadAndScale(QString filename) {
    QImage img = QImage(filename);
    return img; 
}

void GameWindow::recalculateBlockSize() {
    int menuHeight = menuBar()->height();
    int availableWidth = width();
    int availableHeight = height() - menuHeight;
    double blockWidthFromWidth = (double)availableWidth / gridBlocksWidth;
    double blockHeightFromHeight = (double)availableHeight / gridBlocksHeight;
    double blockWidthFromHeight = blockHeightFromHeight * 2.0; 
    
    if (blockWidthFromWidth <= blockWidthFromHeight) {
        blockWidth = (int)blockWidthFromWidth;
        blockHeight = blockWidth / 2;
    } else {
        blockHeight = (int)blockHeightFromHeight;
        blockWidth = blockHeight * 2;
    }
    
    if (blockWidth < 16) blockWidth = 16;
    if (blockHeight < 8) blockHeight = 8;

    int gridPixelWidth = blockWidth * gridBlocksWidth;
    int gridPixelHeight = blockHeight * gridBlocksHeight;
    
    offsetX = (availableWidth - gridPixelWidth) / 2;
    offsetY = menuHeight + (availableHeight - gridPixelHeight) / 2;
}

void GameWindow::resizeEvent(QResizeEvent *re) {
    Q_UNUSED(re);
    recalculateBlockSize();
    int gridPixelWidth = blockWidth * gridBlocksWidth;
    int gridPixelHeight = blockHeight * gridBlocksHeight;
    for (int i = 0; i < 8; ++i) {
        QString filename = QString(":/img/level%1.png").arg(i + 1);
        background[i] = QImage(filename).scaled(gridPixelWidth, gridPixelHeight, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    }
    repaint();
}


void GameWindow::paintEvent(QPaintEvent *e) {
    Q_UNUSED(e);
    QPainter paint(this);
    paint.fillRect(rect(), Qt::black);
    paint.drawImage(offsetX, offsetY, background[grid.level()-1]);
    static bool colorsCached = false;
    
    for(int x = 0; x < grid.getWidth(); ++x) {
        for(int y = 0; y < grid.getHeight(); ++y) {
            puzzle::Block *b = grid.grid(x,y);
            if(b != 0 && b->getType() == puzzle::BlockType::BLOCK_NULL) {
                paint.fillRect(QRect(offsetX + x*blockWidth+1, offsetY+(y*blockHeight+1), blockWidth-1, blockHeight-1), QBrush("#000000"));
            }
             else {
                int image = static_cast<int>(b->getType())-2;
                
                if(b->getType() == BlockType::MATCH) {
                    image = lastFrameGridMatchColor[x][y]; 
                } else if(b->getType() == BlockType::BLOCK_CLEAR) {
                    if(!colorsCached)
                        lastFrameGridMatchColor[x][y] = (rand()%9);
                    
                    
                    int flash_cycle = (b->flashCounter() / 3) % 9;  
                    image = flash_cycle;
                    
                    
                    int flash_phase = b->flashCounter() % 12;
                    if(flash_phase >= 6 && flash_phase < 8) {
                        
                        continue;
                    }
                }
                
                QImage scaledBlock = blocks[image].scaled(blockWidth, blockHeight, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
                paint.drawImage(offsetX + x*blockWidth, offsetY+(y*blockHeight), scaledBlock);
            }
        }
    }  
    puzzle::Piece &p = grid.getPiece();
    int b1 = static_cast<int>(p.blocks[0].getType())-2;
    int b2 = static_cast<int>(p.blocks[1].getType())-2;
    int b3 = static_cast<int>(p.blocks[2].getType())-2;
    if(p.blocks[0] == BlockType::MATCH) {
        b1 = lastFrameMatchColor[0]; 
    }
    if(p.blocks[1] == BlockType::MATCH) {
        b2 = lastFrameMatchColor[1]; 
    }
    if(p.blocks[2] == BlockType::MATCH) {
        b3 = lastFrameMatchColor[2]; 
    }
    colorsCached = true;
    QImage scaledB1 = blocks[b1].scaled(blockWidth, blockHeight, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    QImage scaledB2 = blocks[b2].scaled(blockWidth, blockHeight, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    QImage scaledB3 = blocks[b3].scaled(blockWidth, blockHeight, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    paint.drawImage(offsetX + p.blocks[0].getX()*blockWidth, offsetY+(p.blocks[0].getY()*blockHeight), scaledB1);
    paint.drawImage(offsetX + p.blocks[1].getX()*blockWidth, offsetY+(p.blocks[1].getY()*blockHeight), scaledB2);
    paint.drawImage(offsetX + p.blocks[2].getX()*blockWidth, offsetY+(p.blocks[2].getY()*blockHeight), scaledB3);
    if(game_started == false) {
        QFont font = paint.font();
        QPen pen = paint.pen();
        pen.setColor(QColor(QRgb(0xFFFFFF)));
        int fontSize = blockHeight * 2;
        if (fontSize < 20) fontSize = 20;
        if (fontSize > 70) fontSize = 70;
        font.setPixelSize(fontSize);
        font.setBold(true);
        paint.setFont(font);
        paint.setPen(pen);
        int textX = offsetX + (gridBlocksWidth * blockWidth) / 2 - fontSize * 4;
        int textY = offsetY + (gridBlocksHeight * blockHeight) / 3;
        paint.drawText(textX, textY, "Puzzle Drop");
        paint.drawText(textX - fontSize * 3, textY + fontSize + 20, "Click New Game in Menu");
    } 
    if(game_started == true || first_game == false) {
        QFont font = paint.font();
        QPen pen = paint.pen();
        pen.setColor(QColor(QRgb(0xFFFFFF)));
        int fontSize = blockHeight * 0.8;
        if (fontSize < 12) fontSize = 12;
        if (fontSize > 25) fontSize = 25;
        font.setPixelSize(fontSize);
        font.setBold(true);
        paint.setFont(font);
        paint.setPen(pen);
        QString text;
        QTextStream stream(&text);
        stream << "Level: " << grid.level() << " Lines: " << grid.currentLines();
        paint.drawText(offsetX + 25, offsetY + blockHeight * 2, text);
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
    
    for(int i = 0; i < 3; ++i) {
        lastFrameMatchColor[i] = rand() % 9;
    }
    for(int x = 0; x < 20; ++x) {
        for(int y = 0; y < 30; ++y) {
            lastFrameGridMatchColor[x][y] = rand() % 9;
        }
    }
    grid.keyDown();
    repaint();
    if(grid.gameOver() == true) {
        game_started = false;
        gameOver();
        repaint();
    }
}

void GameWindow::proc() {
    
    for(int i = 0; i < 3; ++i) {
        lastFrameMatchColor[i] = rand() % 9;
    }
    for(int x = 0; x < 20; ++x) {
        for(int y = 0; y < 30; ++y) {
            lastFrameGridMatchColor[x][y] = rand() % 9;
        }
    }
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
    
    for(int i = 0; i < 3; ++i) {
        lastFrameMatchColor[i] = rand() % 9;
    }
    for(int x = 0; x < 20; ++x) {
        for(int y = 0; y < 30; ++y) {
            lastFrameGridMatchColor[x][y] = rand() % 9;
        }
    }
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
    stream << tr("Puzzle Drop ") << PUZZLEDROP_VERSION << "\n" << tr("(C) 2026 LostSideDead Software\n Programmed by Jared Bruni\n");
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