#include "createburrow.h"
#include "ui_createburrow.h"
#include "../../controller.h"
#include "../../sound.h"
#include <QKeyEvent>

CreateBurrow::CreateBurrow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CreateBurrow)
{
    ui->setupUi(this);
    installEventFilter(this);
    this->setStyleSheet("QWidget { border: 2px solid black; padding: 5px; background: rgb(30, 0, 0) }");
    ui->burrowNamelabel->setStyleSheet("QLabel { border: 0; color: white;}");
    ui->burrowLineEdit->setStyleSheet("QLineEdit { border: 1px solid black; color: white; background: rgb(60, 10, 10)}");
    ui->burrowCreate->setStyleSheet("QPushButton { border: 1px solid black; color: white;}");
    ui->burrowCancel->setStyleSheet("QPushButton { border: 1px solid black; color: white;}");
    ui->burrowRandom->setStyleSheet("QPushButton { border: 1px solid black; color: white;}");
}

CreateBurrow::~CreateBurrow()
{
    delete ui;
}


void CreateBurrow::setup(){
    gui->controller->getRandomBurrowName(); //setburrowLineEditText() will be called on return
}

void CreateBurrow::setburrowLineEditText(QString burrowName){
    ui->burrowLineEdit->setText(burrowName);
    ui->burrowLineEdit->setFocus();
    ui->burrowLineEdit->selectAll();
}

void CreateBurrow::on_burrowRandom_clicked(){
    gui->controller->getRandomBurrowName();
}

void CreateBurrow::on_burrowCreate_clicked(){
    QString burrowName = ui->burrowLineEdit->text();
    gui->controller->createBurrow(gui->viewPosition,burrowName);
    gui->soundFXCoolDown = true;
    gui->timerSoundFXCoolDown->start(7000);
    gui->sound->playSoundFXBurrowCreated();
    gui->setLocationText(burrowName);
    on_burrowCancel_clicked();
    gui->showBurrowMenu();
    gui->startUpdate();
}

void CreateBurrow::on_burrowCancel_clicked(){
    gui->closeCreateBurrowDialog();
}

void CreateBurrow::on_burrowClose_clicked(){
    gui->closeCreateBurrowDialog();
}


bool CreateBurrow::eventFilter(QObject *obj, QEvent *event){
    // This function repeatedly call for those QObjects
    // which have installed eventFilter (Step 2)
    if (obj == (QObject*)this){
        if (event->type() == QEvent::KeyPress){
            QKeyEvent *KeyEvent = (QKeyEvent*)event;

            switch(KeyEvent->key()){
                case Qt::Key_Enter:
                    on_burrowCreate_clicked();
                    return true;
                case Qt::Key_Return:
                    on_burrowCreate_clicked();
                    return true;
                case Qt::Key_Escape:
                    on_burrowCancel_clicked();
                    return true;
                default:
                    break;
            }
        }
        return QWidget::eventFilter(obj, event);
    }else {
        // pass the event on to the parent class
        return QWidget::eventFilter(obj, event);
    }
}
