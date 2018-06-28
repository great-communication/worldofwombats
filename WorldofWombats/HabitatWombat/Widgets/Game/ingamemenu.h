#ifndef INGAMEMENU_H
#define INGAMEMENU_H

#include <QWidget>
#include "../../gui.h"

class GUI;

namespace Ui {
class InGameMenu;
}

class InGameMenu : public QWidget
{
    Q_OBJECT

public:
    explicit InGameMenu(QWidget *parent = 0);
    ~InGameMenu();
    GUI *hwGameUI;

private slots:
    void on_showBurrowList_clicked();

    void on_quit_clicked();

private:
    Ui::InGameMenu *ui;    
};

#endif // INGAMEMENU_H
