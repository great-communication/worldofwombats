#ifndef PROGRESSWIDGET_H
#define PROGRESSWIDGET_H

#include <QWidget>
#include "../../gui.h"

class GUI;

namespace Ui {
class ProgressWidget;
}

class ProgressWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ProgressWidget(QWidget *parent = 0);
    ~ProgressWidget();
    GUI *gui;
    void setup(int time);

private slots:
    void addProgress();

private:
    Ui::ProgressWidget *ui;
    QTimer *timer;
    int progressCount;
};

#endif // PROGRESSWIDGET_H
