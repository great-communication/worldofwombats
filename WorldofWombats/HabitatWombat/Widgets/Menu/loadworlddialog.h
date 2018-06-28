#ifndef LOADWORLDDIALOG_H
#define LOADWORLDDIALOG_H

#include <QWidget>
#include "../../menu_ui.h"

class Menu_ui;

namespace Ui {
class loadWorldDialog;
}

class loadWorldDialog : public QWidget
{
    Q_OBJECT

public:
    explicit loadWorldDialog(QWidget *parent = 0);
    ~loadWorldDialog();
    Menu_ui *hwMenuUI;

private slots:
    void on_Ok_clicked();

    void on_Cancel_clicked();

private:
    Ui::loadWorldDialog *ui;
};

#endif // LOADWORLDDIALOG_H
