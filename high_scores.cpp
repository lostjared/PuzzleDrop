#include "high_scores.hpp"
#include<QPainter>
#include<QTextStream>
#include<string>
#include<QInputDialog>
#include<QPixmap>
#include<QIcon>


Score::Score() : lines{0} {}
Score::Score(QString n, int l) : name{n}, lines{l} {}

bool Score::operator<(const Score &s) const {
    if(s.lines < lines) return true;
    return false;
}

HighScores::HighScores(QWidget *parent) : QDialog(parent), settings("LostSideDead", "PuzzleDrop") {
    setFixedSize(640, 500);
    setWindowTitle("High Scores Window");
    setWindowIcon(QIcon(QPixmap(":/img/green3.png").scaled(32, 32)));
    loadScores();
}

HighScores::~HighScores() {
    saveScores();
}

void HighScores::paintEvent(QPaintEvent *e) {
    Q_UNUSED(e);
    QPainter paint(this);
    paint.fillRect(0, 0, 640, 500, QBrush("#000000"));
    QFont font = paint.font();
    QPen pen = paint.pen();
    pen.setColor(QColor(QRgb(0xFF0000)));
    font.setPixelSize(40);
    font.setBold(true);
    paint.setFont(font);
    paint.setPen(pen);
    paint.drawText(50, 50, "High Scores");
    pen.setColor(QColor(QRgb(0xFFFFFF)));
    font.setPixelSize(30);
    font.setBold(true);
    paint.setPen(pen);
    font.setUnderline(true);
    paint.setFont(font);
    paint.drawText(50, 100, "Rank\t\tLines\t\tName");
    font.setUnderline(false);
    paint.setFont(font);
    int pos = 150;
    for(int i = 0; i < 10; ++i) {
        QString value;
        QTextStream stream(&value);
        stream << (i+1) << "\t\t" << scores[i].lines << "\t\t" << scores[i].name;
        paint.drawText(50,pos, value);
        pos += 35;
    }
}

void HighScores::loadScores() {
    for(int i = 0; i < 10; ++i) {
        QString key1, value1;
        QTextStream stream1(&key1), stream2(&value1);
        stream1 << i;
        stream2 << "Anonymous:" << i*5;
        QString score_val = settings.value(key1, value1).toString();
        std::string value = score_val.toStdString();
        auto pos = value.find(":");
        std::string left = value.substr(0, pos);
        std::string right = value.substr(pos+1, value.length());
        Score score;
        score.name = left.c_str();
        score.lines = atoi(right.c_str());
        scores.push_back(score);
    }
    std::sort(scores.begin(), scores.end());
}

void HighScores::saveScores() {
    for(int i = 0; i < 10; ++i) {
        QString key1, value1;
        QTextStream stream1(&key1), stream2(&value1);
        stream1 << i;
        stream2 << scores[i].name << ":" << scores[i].lines;
        settings.setValue(key1, value1);
    }
}

void HighScores::addScore(QString name, int lines) {
    scores.push_back({name, lines});
    std::sort(scores.begin(), scores.end());
    saveScores();
}

bool HighScores::inputName(QString &text) {
    bool ok;
    text = QInputDialog::getText(this, tr("Input Your Name"),tr("Your name:"), QLineEdit::Normal,"", &ok);
    std::string name = text.toStdString();
    for(auto i = name.begin(); i != name.end(); ++i) {
        if(*i == ':')
            *i = ' ';
    }
    text = name.c_str();
    return ok;
}

