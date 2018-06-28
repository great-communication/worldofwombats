#include "wr_player.h"

int id_count { 0 };

//constructor
WR_Player::WR_Player(QObject *parent) : QObject(parent)
{
    m_id = id_count;
    id_count++;
    m_name = "player"+QString::number(m_id);
    m_money = 100;
    m_highscore = 100;
    m_broke = false;

}

QString WR_Player::getName(){
    return m_name;
}

void WR_Player::setName(QString name){
    m_name = name;
}

int WR_Player::getMoney(){
    return m_money;
}

void WR_Player::setMoney(int money) {
    m_money = m_money + money;

    //Set highscore
    if (m_money > m_highscore){
        m_highscore = m_money;
    }

}

int WR_Player::getHighscore(){
    return m_highscore;
}

int WR_Player::getBetWombat() {
    return m_betWombat;
}

bool WR_Player::setBetWombat(int betWombat)
{
    if (betWombat >= 0 && betWombat <= 3)
    {
        m_betWombat = betWombat;
        return (true);
    }
    else
    {
        return(false);
    }
}

int WR_Player::getBetAmount() {
    return m_betAmount;
}

bool WR_Player::setBetAmount(int betAmount)
{
    if (betAmount >= 0 && betAmount <= m_money)
    {
        m_betAmount = betAmount;
        setMoney(-m_betAmount);
        return (true);
    }
    else
    {
        return(false);
    }
}

bool WR_Player::getBroke(){
    return m_broke;
}

void WR_Player::setBrokeTrue(){
    m_broke = true;
}
