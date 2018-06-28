#ifndef INFOBOX_H
#define INFOBOX_H

#include <QWidget>
#include "../../gui.h"

class GUI;

namespace Ui {
class InfoBox;
}

class InfoBox : public QWidget
{
    Q_OBJECT

public:
    explicit InfoBox(QWidget *parent = 0);
    ~InfoBox();    
    GUI *gui;
    int wombatId;

private slots:
    void on_infoBoxClose_clicked();

    void on_infoBoxPossess_clicked();

private:
    Ui::InfoBox *ui;
};

#endif // INFOBOX_H







