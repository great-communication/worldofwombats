#include "wr_game.h"
#include "wr_player.h"
#include "mainwindow.h"
#include <QTime>
#include <QTimer>
#include <cstdlib> // for rand() and srand()
#include <ctime> // for time()

unsigned int getRandomNumber(int min, int max)
{
    static const double fraction = 1.0 / (static_cast<double>(RAND_MAX) + 1.0);
    return static_cast<int>(rand() * fraction * (max - min + 1) + min);
}

//constructor
WR_Game::WR_Game(QObject *parent) : QObject(parent)
{

}

void WR_Game::setup(){

    announcer = new WR_Announcer(this);
    connect(announcer,SIGNAL(s_updateMessageBoard(QString)),this, SLOT(relay_updateMessageBoard(QString)));
    connect(announcer,SIGNAL(s_resetMessageBoard()),this, SLOT(relay_resetMessageBoard()));
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(runWombatsRun()));

    wr_playlistsfx = new QMediaPlaylist(this);
    wr_playlistsfx->addMedia(QUrl("qrc:/Sounds/Sounds/itm_gold_up_01.wav"));
    wr_playlistsfx->addMedia(QUrl("qrc:/Sounds/Sounds/npc_werewolf_growl_01.wav"));
    wr_playlistsfx->addMedia(QUrl("qrc:/Sounds/Sounds/npc_werewolf_growl_02.wav"));
    wr_playlistsfx->addMedia(QUrl("qrc:/Sounds/Sounds/npc_werewolf_growl_03.wav"));
    wr_playlistsfx->addMedia(QUrl("qrc:/Sounds/Sounds/ui_skill_increase.wav"));
    wr_playlistsfx->addMedia(QUrl("qrc:/Sounds/Sounds/mag_vampire_sunlight_02.wav"));
    wr_playlistsfx->setPlaybackMode(QMediaPlaylist::CurrentItemOnce);

    wr_soundfx = new QMediaPlayer(this);
    wr_soundfx->setPlaylist(wr_playlistsfx);
    wr_soundfx->setVolume(80);

    announcer->sendMessage(announcer->Message_Welcome,"",0,0);

}

void WR_Game::addPlayer(QString name){
    WR_Player *player;
    player = new WR_Player(this);
    player->setName(name);
    players.append(player);
    emit s_updateCash(players.last()->getMoney(),players.length()-1);
    announcer->sendMessage(announcer->Message_WelcomeNewPlayer,players.last()->getName(),0,0);
}

int WR_Game::getNumberOfPlayers(){
    return players.length();
}

void WR_Game::wr_playSoundFX(int index){
    wr_playlistsfx->setCurrentIndex(index);
    wr_soundfx->play();
}

void WR_Game::clearAllInvalidBets(){
    for (int i=0;i<players.length();i++){
        emit s_clearInvalidBets(i,players.value(i)->getMoney());
    }
}

void WR_Game::setAllBets(QList<int> wombat, QList<int> betAmount) {

    bool nobets = true;

    for (int i=0;i<players.length();i++){
        int t_wombat = wombat.value(i);
        int t_betAmount = betAmount.value(i);

        if (t_wombat != 0 && t_betAmount != 0) {
            players.value(i)->setBetWombat(t_wombat);
            players.value(i)->setBetAmount(t_betAmount);
            nobets = false;
            emit s_updateCash(players.value(i)->getMoney(),i);
            announcer->sendMessage(announcer->Message_ABetHasBeenMade,players.value(i)->getName(),players.value(i)->getBetWombat(),players.value(i)->getBetAmount());
        }
    }

    if (nobets){
        announcer->sendMessage(announcer->Message_NoBets,"",0,0);
    }

    emit s_disableInput();
}

void WR_Game::startRace(){

    srand(static_cast<unsigned int>(time(0)));
    getRandomNumber(1, 100);

    timer->start(20);
    w1Pos = w2Pos = w3Pos = 70;
    randSpeed1 = randSpeed2 = randSpeed3 = 1;
    count = 90;

   emit s_startWombatAnimation();   
   announcer->sendMessage(announcer->Message_StartingRace,"",0,0);

   int growl = getRandomNumber(1, 3);
   wr_playSoundFX(growl);

}

void WR_Game::stopRace(){
    emit s_updateWombatPositions(70, 70, 70);
    emit s_stopWombatAnimation();
}


void WR_Game::runWombatsRun(){
    if (count > 100) {
       randSpeed1 = getRandomNumber(1, 3);
       randSpeed2 = getRandomNumber(1, 3);
       randSpeed3 = getRandomNumber(1, 3);
       count = 0;
    }
    count++;

    w1Pos += randSpeed1;
    w2Pos += randSpeed2;
    w3Pos += randSpeed3;

    emit s_updateWombatPositions(w1Pos, w2Pos, w3Pos);

    checkFinishLine();

}

void WR_Game::checkFinishLine(){
    // Check if someone crossed the finish line
    if (w1Pos >= 1280 || w2Pos >= 1280 || w3Pos >= 1280){
        timer->stop();

        emit s_stopWombatAnimation();

        //Reset
        count = 90;
        randSpeed1 = 1;
        randSpeed2 = 1;
        randSpeed3 = 1;

        int winnerWombat1;
        int winnerWombat2;
        int winnerWombat3;

        //One winner
        if (w1Pos>w2Pos && w1Pos>w3Pos){
            winnerWombat1 = 1;
            announcer->sendMessage(announcer->Message_RaceWinner,"",winnerWombat1,0);
            payOut(winnerWombat1);
        }else if (w2Pos>w1Pos && w2Pos>w3Pos){
            winnerWombat1 = 2;
            announcer->sendMessage(announcer->Message_RaceWinner,"",winnerWombat1,0);
            payOut(winnerWombat1);
        }else if (w3Pos>w1Pos && w3Pos>w2Pos){
            winnerWombat1 = 3;
            announcer->sendMessage(announcer->Message_RaceWinner,"",winnerWombat1,0);
            payOut(winnerWombat1);
        // Three winners
        }else if (w1Pos==w2Pos && w1Pos == w3Pos){
            winnerWombat1 = 1;
            winnerWombat2 = 2;
            winnerWombat3 = 3;
            announcer->sendMessage(announcer->Message_ThreeRaceWinners,"",0,0);
            payOut(winnerWombat1,winnerWombat2,winnerWombat3);
        //Two winners
        }else if (w1Pos==w2Pos){
            winnerWombat1 = 1;
            winnerWombat2 = 2;
            announcer->sendMessage(announcer->Message_TwoRaceWinners,"",winnerWombat1,winnerWombat2);
            payOut(winnerWombat1,winnerWombat2);
        }else if (w1Pos==w3Pos){
            winnerWombat1 = 1;
            winnerWombat2 = 3;
            announcer->sendMessage(announcer->Message_TwoRaceWinners,"",winnerWombat1,winnerWombat2);
            payOut(winnerWombat1,winnerWombat2);
        }else if (w2Pos==w3Pos){
            winnerWombat1 = 2;
            winnerWombat2 = 3;
            announcer->sendMessage(announcer->Message_TwoRaceWinners,"",winnerWombat1,winnerWombat2);
            payOut(winnerWombat1,winnerWombat2);
        }

        clearAllInvalidBets();
        emit s_enableInput();

        disableBrokePlayers();
    }
}

//One wombat winner
void WR_Game::payOut(int winnerWombat1) {

    bool winner = false;

    for (int i=0;i<players.length();i++){

        if (winnerWombat1 == players.value(i)->getBetWombat()){
            winner = true;
            int moneyToBePayedBack = players.value(i)->getBetAmount()*2;
            players.value(i)->setMoney(moneyToBePayedBack);
            announcer->sendMessage(announcer->Message_WonMoney,players.value(i)->getName(),0,moneyToBePayedBack);
            emit s_updateCash(players.value(i)->getMoney(),i);
        }

        //Reset
        players.value(i)->setBetWombat(0);
        players.value(i)->setBetAmount(0);

        //Play sound if someone won
        if (winner == true){
            wr_playSoundFX(WRSound_Gold);
        }
     }

}

//Two wombat winners
void WR_Game::payOut(int winnerWombat1, int winnerWombat2) {

    bool winner = false;
    for (int i=0;i<players.length();i++){

        if (winnerWombat1 == players.value(i)->getBetWombat() || winnerWombat2 == players.value(i)->getBetWombat()){
            winner = true;
            int moneyToBePayedBack = players.value(i)->getBetAmount();
            players.value(i)->setMoney(moneyToBePayedBack);
            announcer->sendMessage(announcer->Message_WonMoney,players.value(i)->getName(),0,moneyToBePayedBack);
            emit s_updateCash(players.value(i)->getMoney(),i);
        }

        //Reset
        players.value(i)->setBetWombat(0);
        players.value(i)->setBetAmount(0);

        //Play sound if someone won
        if (winner == true){
            wr_playSoundFX(WRSound_Gold);
        }
     }

}

//Three wombat winners
void WR_Game::payOut(int winnerWombat1, int winnerWombat2, int winnerWombat3) {

    bool winner = false;
    for (int i=0;i<players.length();i++){

        if (winnerWombat1 == players.value(i)->getBetWombat() || winnerWombat2 == players.value(i)->getBetWombat() || winnerWombat3 == players.value(i)->getBetWombat()){
            winner = true;
            double temp = qRound((players.value(i)->getBetAmount())/2.0);
            int moneyToBePayedBack = (int)floor(temp);
            players.value(i)->setMoney(moneyToBePayedBack);
            announcer->sendMessage(announcer->Message_WonMoney,players.value(i)->getName(),0,moneyToBePayedBack);
            emit s_updateCash(players.value(i)->getMoney(),i);
        }

        //Reset
        players.value(i)->setBetWombat(0);
        players.value(i)->setBetAmount(0);

        //Play sound if someone won
        if (winner == true){
            wr_playSoundFX(WRSound_Gold);
        }
     }

}

void WR_Game::disableBrokePlayers(){
    for (int i=0;i<players.length();i++){
        if (players.value(i)->getBroke()==false){

            int money = players.value(i)->getMoney();
            if (money <1){
                players.value(i)->setBrokeTrue();
                emit s_disablePlayer(i);
                announcer->sendMessage(announcer->Message_IsBroke,players.value(i)->getName(),0,0);
                wr_playSoundFX(WRSound_Death);
            }
        }
    }
}

void WR_Game::checkIfNewHighScoreForAllPlayers(){
    for (int i=0;i<players.length();i++){
        emit s_checkHighScore(players.value(i)->getHighscore(),players.value(i)->getName());
    }

    emit s_markNewHighScore();

}

void WR_Game::relay_updateMessageBoard(QString output){
    emit s_updateMessageBoard(output);
}

void WR_Game::relay_resetMessageBoard(){
    emit s_resetMessageBoard();
}




