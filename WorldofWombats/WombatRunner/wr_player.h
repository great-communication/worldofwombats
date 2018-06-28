#ifndef WR_PLAYER_H
#define WR_PLAYER_H

#include <QObject>
#include <QString>

class WR_Player : public QObject
{
    Q_OBJECT

private:
    int m_id;
    QString m_name;
    int m_money;
    int m_highscore;
    int m_betWombat;
    int m_betAmount;
    bool m_broke;

public:
    explicit WR_Player(QObject *parent = 0);
    QString getName();
    void setName(QString name);
    int getMoney();
    void setMoney(int money);
    int getHighscore();
    int getBetWombat();
    bool setBetWombat(int betWombat);
    int getBetAmount();
    bool setBetAmount(int betAmount);
    bool getBroke();
    void setBrokeTrue();

signals:

public slots:

};

#endif // WR_PLAYER_H
