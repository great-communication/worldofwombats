#ifndef DEATHNOTICE_H
#define DEATHNOTICE_H

#include <QWidget>
#include "../../gui.h"

class GUI;

namespace Ui {
class DeathNotice;
}

class DeathNotice : public QWidget
{
    Q_OBJECT

public:
    explicit DeathNotice(QWidget *parent = 0);
    ~DeathNotice();
    GUI *gui;
    void setup(QString name, int age, int wombatCount, CauseOfDeath causeOfDeath);

private slots:
    void on_deathNoticeClose_clicked();
    void on_openBurrowList_clicked();
    void on_possessRandomWombat_clicked();
    void on_okButton_clicked();
    void on_quitButton_clicked();

private:
    Ui::DeathNotice *ui;

};

#endif // DEATHNOTICE_H
