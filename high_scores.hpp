#ifndef _PUZZLE_SCORE_H_
#define _PUZZLE_SCORE_H_

#include<QDialog>
#include<QSettings>

struct Score {
    QString name;
    int lines;
    Score();
    Score(QString n, int l);
    bool operator<(const Score &s) const;
};

class HighScores : public QDialog {
Q_OBJECT
public:
    explicit HighScores(QWidget *parent = 0);
    ~HighScores();
    void paintEvent(QPaintEvent *e);
    void loadScores();
    void saveScores();
    void addScore(QString name, int lines);
    bool inputName(QString &text);
private:
    std::vector<Score> scores;
    QSettings settings;
};


#endif
