#include "ingamemenu.h"
#include "ui_ingamemenu.h"

InGameMenu::InGameMenu(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::InGameMenu)
{
    ui->setupUi(this);
    this->setStyleSheet("QWidget { border: 2px solid black; padding: 5px; background: rgb(30, 0, 0) }");
    ui->showBurrowList->setStyleSheet("QPushButton {border: 1px solid black; color: white;}");
    ui->quit->setStyleSheet("QPushButton { border: 1px solid black; color: white; }");
}

InGameMenu::~InGameMenu()
{
    delete ui;
}

void InGameMenu::on_showBurrowList_clicked()
{
    hwGameUI->on_showBurrowList_clicked();
}

void InGameMenu::on_quit_clicked()
{
    hwGameUI->on_quit_clicked();
}
