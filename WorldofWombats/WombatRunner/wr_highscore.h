#ifndef WR_HIGHSCORE_H
#define WR_HIGHSCORE_H

#include <QObject>


class WR_Highscore : public QObject
{
    Q_OBJECT


public:
    explicit WR_Highscore(QObject *parent = 0);
    void setup();    

private:
    struct wr_highscoreEntry {
      int score;
      QString name;
      bool newHighScore;
    };
    QList<wr_highscoreEntry> highscoreArray;    


signals:
    void s_updateHighScore(int index, int score, QString name);
    void s_newHighScore(int index);

public slots:
    void checkHighScore(int score, QString name);
    void markNewHighScore();
    void storeHighScoreOnFile();
};


#endif // WR_HIGHSCORE_H
