#ifndef NEWWORLDDIALOG_H
#define NEWWORLDDIALOG_H

#include <QWidget>
#include "../../menu_ui.h"

class Menu_ui;

namespace Ui {
class NewWorldDialog;
}

class NewWorldDialog : public QWidget
{
    Q_OBJECT

public:
    explicit NewWorldDialog(QWidget *parent = 0);
    ~NewWorldDialog();
    Menu_ui *hwMenuUI;

signals:   

private slots:

    void on_Ok_clicked();
    void on_Cancel_clicked();

private:
    Ui::NewWorldDialog *ui;
};

#endif // NEWWORLDDIALOG_H
