#ifndef BURROWMENU_H
#define BURROWMENU_H

#include <QWidget>
#include "../../gui.h"
#include <QPushButton>

class GUI;

namespace Ui {
class BurrowMenu;
}

class BurrowMenu : public QWidget
{
    Q_OBJECT

public:
    explicit BurrowMenu(QWidget *parent = 0);
    ~BurrowMenu();
    GUI *gui;
    TileType buildingType;

private slots:
    void on_buildStorage_clicked();
    void on_buildNest_clicked();

private:
    Ui::BurrowMenu *ui;   
    QVector<QPushButton*> buttons;
    void deselectAllButtons ();
    bool storageButtonActive;
    bool nestButtonActive;
};

#endif // BURROWMENU_H
