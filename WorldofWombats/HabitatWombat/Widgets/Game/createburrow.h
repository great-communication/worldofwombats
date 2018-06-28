#ifndef CREATEBURROW_H
#define CREATEBURROW_H

#include <QWidget>
#include "../../gui.h"

class GUI;

namespace Ui {
class CreateBurrow;
}

class CreateBurrow : public QWidget
{
    Q_OBJECT

public:
    explicit CreateBurrow(QWidget *parent = 0);
    ~CreateBurrow();
    bool eventFilter(QObject *obj, QEvent *event);
    GUI *gui;
    void setburrowLineEditText(QString burrowName);
    void setup();

private slots:
    void on_burrowRandom_clicked();
    void on_burrowCancel_clicked();
    void on_burrowCreate_clicked();

    void on_burrowClose_clicked();

private:
    Ui::CreateBurrow *ui;   
};

#endif // CREATEBURROW_H
