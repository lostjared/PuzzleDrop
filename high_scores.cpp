#include "high_scores.hpp"
#include<QLinearGradient>
#include<QPainter>
#include<QPainterPath>
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
    setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
    setFixedSize(640, 500);
    setMinimumSize(640, 500);
    setMaximumSize(640, 500);
    setSizeGripEnabled(false);
    setWindowTitle("High Scores");
    setWindowIcon(QIcon(QPixmap(":/img/green3.png").scaled(32, 32)));
    loadScores();
}

HighScores::~HighScores() {
    saveScores();
}

void HighScores::paintEvent(QPaintEvent *e) {
    Q_UNUSED(e);
    QPainter paint(this);
    paint.setRenderHint(QPainter::Antialiasing, true);

    QLinearGradient background(rect().topLeft(), rect().bottomRight());
    background.setColorAt(0.0, QColor("#101820"));
    background.setColorAt(0.55, QColor("#172833"));
    background.setColorAt(1.0, QColor("#070b0f"));
    paint.fillRect(rect(), background);

    QRectF panel(28, 28, width() - 56, height() - 56);
    QPainterPath panelPath;
    panelPath.addRoundedRect(panel, 18, 18);
    paint.fillPath(panelPath, QColor(7, 12, 16, 190));
    paint.setPen(QPen(QColor(255, 255, 255, 42), 1));
    paint.drawPath(panelPath);

    QFont titleFont = paint.font();
    titleFont.setPixelSize(38);
    titleFont.setBold(true);
    paint.setFont(titleFont);
    paint.setPen(QColor("#fff4df"));
    paint.drawText(QRectF(52, 42, width() - 104, 48), Qt::AlignLeft | Qt::AlignVCenter, tr("High Scores"));

    QFont labelFont = paint.font();
    labelFont.setPixelSize(13);
    labelFont.setBold(true);
    labelFont.setLetterSpacing(QFont::AbsoluteSpacing, 1.4);
    paint.setFont(labelFont);
    paint.setPen(QColor("#8fb6bf"));
    paint.drawText(QRectF(56, 104, 70, 24), Qt::AlignLeft | Qt::AlignVCenter, tr("RANK"));
    paint.drawText(QRectF(148, 104, 95, 24), Qt::AlignLeft | Qt::AlignVCenter, tr("LINES"));
    paint.drawText(QRectF(268, 104, 300, 24), Qt::AlignLeft | Qt::AlignVCenter, tr("PLAYER"));

    QFont rowFont = paint.font();
    rowFont.setPixelSize(20);
    rowFont.setBold(false);
    paint.setFont(rowFont);

    int pos = 138;
    for(int i = 0; i < 10; ++i) {
        QRectF row(48, pos, width() - 96, 30);
        QPainterPath rowPath;
        rowPath.addRoundedRect(row, 8, 8);
        paint.fillPath(rowPath, i % 2 == 0 ? QColor(255, 255, 255, 18) : QColor(255, 255, 255, 9));

        paint.setPen(i == 0 ? QColor("#ffd166") : QColor("#f4efe6"));
        paint.drawText(QRectF(64, pos, 52, 30), Qt::AlignLeft | Qt::AlignVCenter, QString::number(i + 1));
        paint.drawText(QRectF(148, pos, 90, 30), Qt::AlignLeft | Qt::AlignVCenter, QString::number(scores[i].lines));
        paint.drawText(QRectF(268, pos, 300, 30), Qt::AlignLeft | Qt::AlignVCenter, scores[i].name);
        pos += 34;
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
    text = QInputDialog::getText(this, tr("New High Score"),tr("Player name:"), QLineEdit::Normal,"", &ok);
    std::string name = text.toStdString();
    for(auto i = name.begin(); i != name.end(); ++i) {
        if(*i == ':')
            *i = ' ';
    }
    text = name.c_str();
    return ok;
}
