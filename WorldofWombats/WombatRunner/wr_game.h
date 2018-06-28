#ifndef WR_GAME_H
#define WR_GAME_H

#include <QObject>
#include <QTimer>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include "wr_player.h"
#include "wr_announcer.h"

class WR_Game : public QObject
{
    Q_OBJECT

private:
    WR_Player *player1;
    QList<WR_Player*> players;
    QTimer *timer;
    int w1Pos, w2Pos, w3Pos;
    int count;
    int randSpeed1, randSpeed2, randSpeed3;
    QMediaPlaylist *wr_playlistsfx;
    QMediaPlayer *wr_soundfx;
    void wr_playSoundFX(int index);
    void checkFinishLine();
    void payOut(int winnerWombat1);
    void payOut(int winnerWombat1,int winnerWombat2);
    void payOut(int winnerWombat1,int winnerWombat2,int winnerWombat3);
    void disableBrokePlayers();
    void gameOver();
    enum WRSoundFX {

        WRSound_Gold,
        WRSound_Growl1,
        WRSound_Growl2,
        WRSound_Growl3,
        WRSound_Highscore,
        WRSound_Death,

    };


public:
    explicit WR_Game(QObject *parent = 0);
    WR_Announcer *announcer;
    void setup();        
    int getNumberOfPlayers();
    void clearAllInvalidBets();


signals:    
    void s_updateCash(int cash, int index);
    void s_updateWombatPositions(int xfc, int xfcxx, int xfcs);
    void s_startWombatAnimation();
    void s_stopWombatAnimation();
    void s_disableInput();
    void s_enableInput();
    void s_clearInvalidBets(int index, int money);
    void s_updateMessageBoard(QString output);
    void s_resetMessageBoard();    
    void s_disablePlayer(int index);
    void s_checkHighScore(int score, QString name);
    void s_markNewHighScore();    

public slots:
    void addPlayer(QString name);
    void setAllBets(QList<int> wombat, QList<int> betAmount);
    void startRace();
    void runWombatsRun();
    void relay_updateMessageBoard(QString output);
    void relay_resetMessageBoard();
    void checkIfNewHighScoreForAllPlayers();
    void stopRace();
};

#endif // WR_GAME_H
