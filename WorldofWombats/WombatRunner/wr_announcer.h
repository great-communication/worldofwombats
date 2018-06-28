#ifndef WR_ANNOUNCER_H
#define WR_ANNOUNCER_H

#include <QObject>
#include "wr_player.h"

enum Language {

    Language_English,
    Language_Swedish,
    Language_Chinese,

};

class WR_Announcer : public QObject
{
    Q_OBJECT
public:
    explicit WR_Announcer(QObject *parent = 0);

private:


public:
    enum Message {

        Message_Welcome,
        Message_WelcomeNewPlayer,
        Message_ABetHasBeenMade,
        Message_NoBets,
        Message_StartingRace,
        Message_RaceWinner,
        Message_TwoRaceWinners,
        Message_ThreeRaceWinners,
        Message_WonMoney,
        Message_IsBroke,

    };

    void sendMessage (Message message, QString pstring, int pwombat, int pamount);

private:
    Language language;


signals:    
    void s_updateMessageBoard (QString output);
    void s_resetMessageBoard ();

public slots:
    void setLanguage(Language selection);



};

#endif // WR_ANNOUNCER_H
