#ifndef BURROWLIST_H
#define BURROWLIST_H

#include <QWidget>
#include "../../gui.h"

class GUI;

namespace Ui {
class BurrowList;
}

class BurrowList : public QWidget
{
    Q_OBJECT

public:
    explicit BurrowList(QWidget *parent = 0);
    ~BurrowList();
    GUI *gui;
    void displayBurrowList(QVector<QString> strings, QVector<int> burrowIds, QVector<int> selectedBurrowWombatIds);

public slots:
    void setup();
    void update();    

private slots:
    void on_burrowListExit_clicked();
    void on_burrowListNameList_cursorPositionChanged();
    void on_burrowListSizeList_cursorPositionChanged();

private:
    Ui::BurrowList *ui;
    int indexSelectedBurrow;
    bool selfBlock;
    bool updateBlock, updateBlock2;
    QVector<int> wombatIds;
    QVector<int> burrowIds;
    void prepareBurrowListForDisplay();
};

#endif // BURROWLIST_H
