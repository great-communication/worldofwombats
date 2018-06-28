#include "deathnotice.h"
#include "ui_deathnotice.h"
#include "../../controller.h"

DeathNotice::DeathNotice(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DeathNotice)
{
    ui->setupUi(this);    

}

DeathNotice::~DeathNotice()
{
    delete ui;
}

void DeathNotice::setup(QString name, int age, int wombatCount, CauseOfDeath causeOfDeath){

    ui->info1->setText(name);

    QString obituary = gui->guiGlobal->getObituary(causeOfDeath);

    QString s;

    if(causeOfDeath = CauseOfDeath_OldAge){
        if (age<15){
            s = obituary+" at the meek age of "+QString::number(age);
        }else if (age>=15 && age<20){
            s = obituary+" at the age of "+QString::number(age);
        }else{
            s = obituary+" at the formidable age of "+QString::number(age);
        }
    }else{
        s = obituary+" at the meek age of "+QString::number(age);
    }

    ui->info2->setText(s);

    if (wombatCount>0){
        ui->okButton->setVisible(false);
        ui->quitButton->setVisible(false);
    }else{
        ui->possessRandomWombat->setVisible(false);
        ui->openBurrowList->setVisible(false);
        ui->info3->setText("All wombats have perished! Game over!");
    }

}

void DeathNotice::on_deathNoticeClose_clicked(){
    gui->closeDeathNotice();
}

void DeathNotice::on_openBurrowList_clicked(){
    gui->closeDeathNotice();
    gui->on_showBurrowList_clicked();
    gui->showMenu();
}

void DeathNotice::on_possessRandomWombat_clicked(){
    gui->controller->getRandomWombatIdForPossess(); //Controller will call possess in gui on return
    on_deathNoticeClose_clicked();
}

void DeathNotice::on_okButton_clicked()
{
    on_deathNoticeClose_clicked();
}

void DeathNotice::on_quitButton_clicked()
{
    //Quit game
    gui->on_quit_clicked();
}
