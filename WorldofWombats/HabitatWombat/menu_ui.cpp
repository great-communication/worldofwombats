#include "menu_ui.h"
#include "ui_menu_ui.h"
#include <QLineEdit>
#include <QKeyEvent> // why the fuck do I need this when it is not needed in hw_game_ui.cpp ??
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QListWidget>

Menu_ui::Menu_ui(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Menu_ui)
{
    ui->setupUi(this);
    ui->hwMenu_newworld->setVisible(true);
    ui->hwMenu_newworldsel->setVisible(false);
    ui->hwMenu_loadworld->setVisible(true);
    ui->hwMenu_loadworldsel->setVisible(false);
    ui->hwMenu_options->setVisible(true);
    ui->hwMenu_optionssel->setVisible(false);
    ui->hwMenu_quit->setVisible(true);
    ui->hwMenu_quitsel->setVisible(false);
    ui->hwMenu_newworldmousearea->installEventFilter(this);
    ui->hwMenu_loadworldmousearea->installEventFilter(this);
    ui->hwMenu_optionsmousearea->installEventFilter(this);
    ui->hwMenu_quitmousearea->installEventFilter(this);
    dialogOpen = false;
    movingWindow = false;    
}

Menu_ui::~Menu_ui(){
    delete ui;
}

void Menu_ui::setup(){
    readLevelsFile();
    gui = new GUI(this);
    connect(gui,SIGNAL(s_backToHWMenu()),this,SLOT(backToHWMenu()));
    connect(gui,SIGNAL(s_getMainWindowState()),this,SLOT(getMainWindowState()));
    ui->stackedWidget->addWidget(gui);
    //hwGameObject = new HW_Game(this);
    //hwGameObject->setGameUI(hwGameUI);
    //hwGameUI->setGameObject(hwGameObject);
    guiLoaded = false;
}

void Menu_ui::readLevelsFile(){

    levels.clear();

    QString fileName = "hw/levels";

    if (fileExists(fileName)==false){
        saveLevelsFile(); //Creates a levels file
    }

    QFile file(fileName);

    if (file.open(QIODevice::ReadOnly)){
        QDataStream stream(&file);
        qint32 temp;
        stream >> temp;
        QString s;
        for (int i=0;i<temp;i++){
            stream >> s;
            levels.append(s);
        }
        file.close();
    }
}

void Menu_ui::saveLevelsFile(){

        QDir dir("hw");
        if (!dir.exists()) {
            dir.mkpath(".");
        }

        QString filename = "hw/levels";
        QFile file( filename );
        if (file.open(QIODevice::WriteOnly)){
            QDataStream out(&file);
            out << (qint32)levels.length();
            for (int i=0;i<levels.length();i++){
                out << levels.value(i);
            }
            file.close();
        }
}

bool Menu_ui::fileExists(QString path) {
    QFileInfo check_file(path);
    // Check if file exists and if yes: Is it really a file and no directory?
    if (check_file.exists() && check_file.isFile()) {
        return true;
    } else {
        return false;
    }
}

void Menu_ui::showNewWorldDialog(){
    dialogOpen=true;
    newWorldDialog = new NewWorldDialog(this);
    newWorldDialog->hwMenuUI = this;
    QPoint center;
    center.setX((this->width()/2)-(newWorldDialog->width()/2));
    center.setY((this->height()/2)-(newWorldDialog->height()/2));
    newWorldDialog->move(center);
    newWorldDialog->show();
    QLineEdit* lineEdit = newWorldDialog->findChild<QLineEdit*>("lineEdit");
    lineEdit->setFocus();
    newWorldDialog->installEventFilter(this);
    newWorldDialogOffset = newWorldDialog->pos();    
}

void Menu_ui::newWorldDialogOkClicked(){
    QLineEdit* lineEdit = newWorldDialog->findChild<QLineEdit*>("lineEdit");
    QString levelName = lineEdit->text();
    levelName = levelName.trimmed();
    if (levelName!=""){
        ui->hwMenu_newworld->setVisible(true);
        ui->hwMenu_newworldsel->setVisible(false);
        newWorldDialog->hide();
        newWorldDialog->deleteLater();
        dialogOpen=false;
        ui->label->setText("Creating world...");
        ui->stackedWidget->setCurrentIndex(1);
        ui->progressBar->setValue(20);
        levels.append(levelName);
        saveLevelsFile();
        gui->setup(levelName);
        ui->progressBar->setValue(50);
        //hwGameObject->level = levelName;
        //hwGameObject->setup();
        ui->progressBar->setValue(100);
        ui->stackedWidget->setCurrentIndex(ui->stackedWidget->indexOf(gui));
        guiLoaded = true;
    }else{
        lineEdit->clear();
        lineEdit->setFocus();
    }
}

void Menu_ui::newWorldDialogCancelClicked(){
    ui->hwMenu_newworld->setVisible(true);
    ui->hwMenu_newworldsel->setVisible(false);
    newWorldDialog->deleteLater();
    dialogOpen=false;
}

void Menu_ui::showLoadWorldDialog(){
    dialogOpen=true;
    lloadWorldDialog = new loadWorldDialog(this);
    lloadWorldDialog->hwMenuUI = this;
    QPoint center;
    center.setX((this->width()/2)-(lloadWorldDialog->width()/2));
    center.setY((this->height()/2)-(lloadWorldDialog->height()/2));
    lloadWorldDialog->move(center);
    lloadWorldDialog->show();
    lloadWorldDialog->installEventFilter(this);
    loadWorldDialogOffset = lloadWorldDialog->pos();
    QListWidget* listWidget = lloadWorldDialog->findChild<QListWidget*>("listWidget");
    listWidget->setStyleSheet("QListWidget { border: 2px solid black; padding: 5px; color:white; background: rgb(30, 0, 0) }");
    for (int i=0;i<levels.length();i++){
        QString label = levels.value(i);
        listWidget->addItem(label);
    }         
}

void Menu_ui::loadWorldDialogOkClicked(){
    QListWidget* listWidget = lloadWorldDialog->findChild<QListWidget*>("listWidget");
    if(listWidget->currentItem() != NULL) {
        QListWidgetItem *item = listWidget->currentItem();
        QString levelName = item->text();
        ui->hwMenu_loadworld->setVisible(true);
        ui->hwMenu_loadworldsel->setVisible(false);
        lloadWorldDialog->hide();
        lloadWorldDialog->deleteLater();
        dialogOpen=false;
        ui->label->setText("Loading world...");
        ui->stackedWidget->setCurrentIndex(1);
        ui->progressBar->setValue(20);
        gui->setup(levelName);
        ui->progressBar->setValue(50);
        //hwGameObject->level = levelName;
        //hwGameObject->setup();
        ui->progressBar->setValue(100);
        ui->stackedWidget->setCurrentIndex(ui->stackedWidget->indexOf(gui));
        guiLoaded = true;
    }
}

void Menu_ui::loadWorldDialogCancelClicked(){
    ui->hwMenu_loadworld->setVisible(true);
    ui->hwMenu_loadworldsel->setVisible(false);
    lloadWorldDialog->deleteLater();
    dialogOpen=false;
}

void Menu_ui::moveWidget(QWidget *widget,QPoint offset)
{
    if (movingWindow){
        // Messy, clean it up!
        QPoint temp = this->mapFromGlobal(QCursor::pos())-cursorPos+offset;
        QPoint temp2;
        if (temp.x()  > 0 && temp.x()+widget->width() < 1600){
            temp2.setX(temp.x());
            temp2.setY(widget->pos().y());
            widget->move(temp2);
        }else if (temp.x()  <= 0){
            temp2.setX(0);
            temp2.setY(widget->pos().y());
            widget->move(temp2);
        }else {
            temp2.setX(1600-widget->width());
            temp2.setY(widget->pos().y());
            widget->move(temp2);
        }
        if (temp.y()  > 0 && temp.y()+widget->height() < 900){
            temp2.setX(widget->pos().x());
            temp2.setY(temp.y());
            widget->move(temp2);
        }else if (temp.y()  <=0){
            temp2.setX(widget->pos().x());
            temp2.setY(0);
            widget->move(temp2);
        }else{
            temp2.setX(widget->pos().x());
            temp2.setY(900-widget->height());
            widget->move(temp2);
        }
    }
}

//To get mouse enter/leave and mouse click functionality on qLabels
bool Menu_ui::eventFilter(QObject *obj, QEvent *event)
{
    // This function repeatedly call for those QObjects
    // which have installed eventFilter (Step 2)

   if (obj == (QObject*)ui->hwMenu_newworldmousearea && dialogOpen==false){
        if (event->type() == QEvent::Enter)
        {
            emit s_playSoundFX(0);
            ui->hwMenu_newworld->setVisible(false);
            ui->hwMenu_newworldsel->setVisible(true);
            ui->hwMenu_loadworld->setVisible(true);
            ui->hwMenu_loadworldsel->setVisible(false);
            ui->hwMenu_options->setVisible(true);
            ui->hwMenu_optionssel->setVisible(false);
            ui->hwMenu_quit->setVisible(true);
            ui->hwMenu_quitsel->setVisible(false);

        }else if (event->type() == QEvent::Leave) {
            ui->hwMenu_newworld->setVisible(true);
            ui->hwMenu_newworldsel->setVisible(false);
            ui->hwMenu_loadworld->setVisible(true);
            ui->hwMenu_loadworldsel->setVisible(false);
            ui->hwMenu_options->setVisible(true);
            ui->hwMenu_optionssel->setVisible(false);
            ui->hwMenu_quit->setVisible(true);
            ui->hwMenu_quitsel->setVisible(false);

        }else if (event->type() == QEvent::MouseButtonRelease){
            showNewWorldDialog();
        }

        return QWidget::eventFilter(obj, event);
   }else if (obj == (QObject*)ui->hwMenu_loadworldmousearea && dialogOpen==false){
        if (event->type() == QEvent::Enter)
        {
            emit s_playSoundFX(0);
            ui->hwMenu_newworld->setVisible(true);
            ui->hwMenu_newworldsel->setVisible(false);
            ui->hwMenu_loadworld->setVisible(false);
            ui->hwMenu_loadworldsel->setVisible(true);
            ui->hwMenu_options->setVisible(true);
            ui->hwMenu_optionssel->setVisible(false);
            ui->hwMenu_quit->setVisible(true);
            ui->hwMenu_quitsel->setVisible(false);

        }else if (event->type() == QEvent::Leave) {
            ui->hwMenu_newworld->setVisible(true);
            ui->hwMenu_newworldsel->setVisible(false);
            ui->hwMenu_loadworld->setVisible(true);
            ui->hwMenu_loadworldsel->setVisible(false);
            ui->hwMenu_options->setVisible(true);
            ui->hwMenu_optionssel->setVisible(false);
            ui->hwMenu_quit->setVisible(true);
            ui->hwMenu_quitsel->setVisible(false);

        }else if (event->type() == QEvent::MouseButtonRelease){
            showLoadWorldDialog();
        }

       return QWidget::eventFilter(obj, event);
  }else if (obj == (QObject*)ui->hwMenu_optionsmousearea && dialogOpen==false){
       if (event->type() == QEvent::Enter)
       {
            emit s_playSoundFX(0);
            ui->hwMenu_newworld->setVisible(true);
            ui->hwMenu_newworldsel->setVisible(false);
            ui->hwMenu_loadworld->setVisible(true);
            ui->hwMenu_loadworldsel->setVisible(false);
            ui->hwMenu_options->setVisible(false);
            ui->hwMenu_optionssel->setVisible(true);
            ui->hwMenu_quit->setVisible(true);
            ui->hwMenu_quitsel->setVisible(false);

        }else if (event->type() == QEvent::Leave) {
            ui->hwMenu_newworld->setVisible(true);
            ui->hwMenu_newworldsel->setVisible(false);
            ui->hwMenu_loadworld->setVisible(true);
            ui->hwMenu_loadworldsel->setVisible(false);
            ui->hwMenu_options->setVisible(true);
            ui->hwMenu_optionssel->setVisible(false);
            ui->hwMenu_quit->setVisible(true);
            ui->hwMenu_quitsel->setVisible(false);

       }else if (event->type() == QEvent::MouseButtonRelease){

       }

       return QWidget::eventFilter(obj, event);
  }else if (obj == (QObject*)ui->hwMenu_quitmousearea && dialogOpen==false){
       if (event->type() == QEvent::Enter)
       {
            emit s_playSoundFX(0);
            ui->hwMenu_newworld->setVisible(true);
            ui->hwMenu_newworldsel->setVisible(false);
            ui->hwMenu_loadworld->setVisible(true);
            ui->hwMenu_loadworldsel->setVisible(false);
            ui->hwMenu_options->setVisible(true);
            ui->hwMenu_optionssel->setVisible(false);
            ui->hwMenu_quit->setVisible(false);
            ui->hwMenu_quitsel->setVisible(true);

       }else if (event->type() == QEvent::Leave) {
            ui->hwMenu_newworld->setVisible(true);
            ui->hwMenu_newworldsel->setVisible(false);
            ui->hwMenu_loadworld->setVisible(true);
            ui->hwMenu_loadworldsel->setVisible(false);
            ui->hwMenu_options->setVisible(true);
            ui->hwMenu_optionssel->setVisible(false);
            ui->hwMenu_quit->setVisible(true);
            ui->hwMenu_quitsel->setVisible(false);

       }else if (event->type() == QEvent::MouseButtonRelease){
           //hwGameObject->deleteLater();
           gui->deleteLater();
           emit s_backToMainMenu();
           ui->hwMenu_quit->setVisible(true);
           ui->hwMenu_quitsel->setVisible(false);
       }

       return QWidget::eventFilter(obj, event);
  }else if (obj == (QObject*)newWorldDialog){

       if (event->type() == QEvent::MouseButtonPress){
           const QMouseEvent* const mouseEvent = static_cast<const QMouseEvent*>( event ); //why the fuck can I not check left mouse button in QEvent??
           if(mouseEvent->buttons() == Qt::LeftButton){
               movingWindow = true;
               cursorPos=this->mapFromGlobal(QCursor::pos());
           }

       }else if (event->type() == QEvent::MouseMove){
           moveWidget(newWorldDialog,newWorldDialogOffset);

       }else if (event->type() == QEvent::MouseButtonRelease){
           if (movingWindow){
               movingWindow=false;
               newWorldDialogOffset = newWorldDialog->pos();
           }

       }

          return QWidget::eventFilter(obj, event);
    }else if (obj == (QObject*)lloadWorldDialog){

       if (event->type() == QEvent::MouseButtonPress){
           const QMouseEvent* const mouseEvent = static_cast<const QMouseEvent*>( event ); //why the fuck can I not check left mouse button in QEvent??
           if(mouseEvent->buttons() == Qt::LeftButton){
               movingWindow = true;
               cursorPos=this->mapFromGlobal(QCursor::pos());
           }

       }else if (event->type() == QEvent::MouseMove){
           moveWidget(lloadWorldDialog,loadWorldDialogOffset);

       }else if (event->type() == QEvent::MouseButtonRelease){
           if (movingWindow){
               movingWindow=false;
               loadWorldDialogOffset = lloadWorldDialog->pos();
           }

       }

          return QWidget::eventFilter(obj, event);
    }else {
        // pass the event on to the parent class
        return QWidget::eventFilter(obj, event);
    }

}

void Menu_ui::backToHWMenu(){
    gui->deleteLater();
    ui->stackedWidget->setCurrentIndex(0);
    setup();
}

void Menu_ui::getMainWindowState(){
    emit s_getMainWindowState();
}
