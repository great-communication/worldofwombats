#include "burrowmenu.h"
#include "ui_burrowmenu.h"

BurrowMenu::BurrowMenu(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BurrowMenu)
{
    ui->setupUi(this);
    ui->buildStorage->setStyleSheet("QPushButton {border: 1px solid black; color: white;}");
    ui->buildNest->setStyleSheet("QPushButton {border: 1px solid black; color: white;}");
    buttons.append(ui->buildStorage);
    buttons.append(ui->buildNest);
    buildingType = None;
    storageButtonActive = nestButtonActive = false; //If you add new button remember to set boolean = false in deselectAllButtons functions
}

BurrowMenu::~BurrowMenu()
{
    delete ui;
}

void BurrowMenu::deselectAllButtons(){
     for (int i=0;i<buttons.length();i++){
        buttons[i]->setStyleSheet("QPushButton {border: 1px solid black; color: white;}");
        storageButtonActive = false;
        nestButtonActive = false;
     }
}

void BurrowMenu::on_buildStorage_clicked()
{
    if (storageButtonActive){        
        gui->building = false;
        buildingType = None;
        deselectAllButtons();        
    }else{
        gui->building = true;        
        buildingType = Storage;
        deselectAllButtons();
        storageButtonActive = true;
        ui->buildStorage->setStyleSheet("QPushButton {border: 1px solid black; color: white; background: rgb(90, 20, 20);}");        
    }
    gui->enableEvents->setFocus();
}

void BurrowMenu::on_buildNest_clicked()
{
    if (nestButtonActive){        
        gui->building = false;
        buildingType = None;
        deselectAllButtons();
    }else{        
        gui->building = true;        
        buildingType = Nest;
        deselectAllButtons();
        nestButtonActive = true;
        ui->buildNest->setStyleSheet("QPushButton {border: 1px solid black; color: white; background: rgb(90, 20, 20);}");
    }
    gui->enableEvents->setFocus();
}
