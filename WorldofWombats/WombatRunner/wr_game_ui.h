#ifndef WR_GAME_UI_H
#define WR_GAME_UI_H

#include "wr_announcer.h"
#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QIntValidator>


namespace Ui {
class WR_GAME_UI;
}

class WR_GAME_UI : public QWidget
{
    Q_OBJECT

public:
    explicit WR_GAME_UI(QWidget *parent = 0);
    ~WR_GAME_UI();
    bool eventFilter(QObject *obj, QEvent *event);

signals:
    void s_setAllBets(QList<int> wombat, QList<int> betAmount);
    void s_startRace();
    void s_checkIfNewHighScoreForAllPlayers();
    void s_storeHighScoreOnFile();
    void s_setLanguage(Language selection);
    void s_addPlayer(QString name);
    void s_stopRace();
    void s_backToWRGameMenu();

private:
    Ui::WR_GAME_UI *ui;
    int w1, w2, w3;
    QIntValidator *wombatRange;
    QIntValidator *betRangePlayer1,*betRangePlayer2,*betRangePlayer3;
    QList<QIntValidator*> betRange;
    bool startDisabled;
    bool addPlayer1Clicked;
    QList<QLabel*> wr_cash,wr_cash_dollarsign,wr_bet_dollarsign;
    QList<QLineEdit*> wr_playername,wr_bet,wr_betw;
    QMovie *wombatAnimation;

private slots:
    void updateCash (int cash, int index);
    void startWombatAnimation();
    void stopWombatAnimation();
    void updateWombatPositions(int w1Pos, int w2Pos, int w3Pos);
    void disableInput();
    void enableInput();
    void clearInvalidBets(int index, int money);
    void updateMessageBoard(QString output);
    void resetMessageBoard();
    void disablePlayer(int index);
    void on_flag_uk_clicked();
    void on_flag_sweden_clicked();
    void on_flag_china_clicked();
    void on_wr_playername1_returnPressed();
    void on_wr_playername2_returnPressed();
    void on_wr_playername3_returnPressed();
    void on_wr_bet_1_textEdited(const QString &arg1);
    void on_wr_bet_2_textEdited(const QString &arg1);
    void on_wr_bet_3_textEdited(const QString &arg1);
    void on_wr_betw_1_textEdited(const QString &arg1);
    void on_wr_betw_2_textEdited(const QString &arg1);
    void on_wr_betw_3_textEdited(const QString &arg1);
};

#endif // WR_GAME_UI_H
