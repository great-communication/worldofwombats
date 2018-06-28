#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "WombatRunner/wr_highscore.h"
#include "WombatRunner/wr_game.h"
#include <QDesktopWidget>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{    
    ui->setupUi(this);
    ui->centralWidget->layout()->setAlignment(Qt::AlignHCenter);
    ui->mainmenu_HW->setVisible(false);
    ui->mainmenu_HWsel->setVisible(true);
    ui->mainmenu_WRsel->setVisible(false);
    ui->mainmenu_CWsel->setVisible(false);
    ui->mainmenu_IAWsel->setVisible(false);
    ui->mainmenu_HWmousearea->installEventFilter(this);
    ui->mainmenu_WRmousearea->installEventFilter(this);
    ui->mainmenu_CWmousearea->installEventFilter(this);
    ui->mainmenu_IAWmousearea->installEventFilter(this);

    //WR menu
    ui->wrMenu_start->setVisible(false);
    ui->wrMenu_startsel->setVisible(true);
    ui->wrMenu_highscoresel->setVisible(false);
    ui->wrMenu_quitsel->setVisible(false);
    ui->wr_highscore->setVisible(false);
    ui->wrMenu_exitbutton->setVisible(false);
    ui->wrMenu_exitbuttonsel->setVisible(false);
    ui->wrMenu_startmousearea->installEventFilter(this);
    ui->wrMenu_highscoremousearea->installEventFilter(this);
    ui->wrMenu_quitmousearea->installEventFilter(this);
    ui->wrMenu_exitbuttonmousearea->installEventFilter(this);
    highScoreOpen = false;
    wr_highscore_score.append(ui->wr_highscore_score_1);
    wr_highscore_score.append(ui->wr_highscore_score_2);
    wr_highscore_score.append(ui->wr_highscore_score_3);
    wr_highscore_score.append(ui->wr_highscore_score_4);
    wr_highscore_score.append(ui->wr_highscore_score_5);
    wr_highscore_score.append(ui->wr_highscore_score_6);
    wr_highscore_score.append(ui->wr_highscore_score_7);
    wr_highscore_score.append(ui->wr_highscore_score_8);
    wr_highscore_score.append(ui->wr_highscore_score_9);
    wr_highscore_score.append(ui->wr_highscore_score_10);
    wr_highscore_name.append(ui->wr_highscore_name_1);
    wr_highscore_name.append(ui->wr_highscore_name_2);
    wr_highscore_name.append(ui->wr_highscore_name_3);
    wr_highscore_name.append(ui->wr_highscore_name_4);
    wr_highscore_name.append(ui->wr_highscore_name_5);
    wr_highscore_name.append(ui->wr_highscore_name_6);
    wr_highscore_name.append(ui->wr_highscore_name_7);
    wr_highscore_name.append(ui->wr_highscore_name_8);
    wr_highscore_name.append(ui->wr_highscore_name_9);
    wr_highscore_name.append(ui->wr_highscore_name_10);
    wr_highscore_new.append(ui->wr_highscore_new_1);
    wr_highscore_new.append(ui->wr_highscore_new_2);
    wr_highscore_new.append(ui->wr_highscore_new_3);
    wr_highscore_new.append(ui->wr_highscore_new_4);
    wr_highscore_new.append(ui->wr_highscore_new_5);
    wr_highscore_new.append(ui->wr_highscore_new_6);
    wr_highscore_new.append(ui->wr_highscore_new_7);
    wr_highscore_new.append(ui->wr_highscore_new_8);
    wr_highscore_new.append(ui->wr_highscore_new_9);
    wr_highscore_new.append(ui->wr_highscore_new_10);
    for (int i=0;i<10;i++){
        wr_highscore_new.value(i)->setStyleSheet("QLabel { color : rgba(0, 0, 0, 0); }");        //invisible
        wr_highscore_new.value(i)->setVisible(false);
        wr_highscore_score.value(i)->setStyleSheet("QLabel { color : yellow; }");
        wr_highscore_score.value(i)->setVisible(false);
        wr_highscore_name.value(i)->setStyleSheet("QLabel { color : yellow; }");
        wr_highscore_name.value(i)->setVisible(false);
    }

    //Music
    QMediaPlaylist *playlistmusic = new QMediaPlaylist(this);
    playlistmusic->addMedia(QUrl("qrc:/Sounds/Music/Sounds/Music/05 Antibalas - War Hero.mp3"));
    playlistmusic->setPlaybackMode(QMediaPlaylist::Loop);

    QMediaPlayer *music = new QMediaPlayer(this);
    music->setPlaylist(playlistmusic);
    // Volume set to 0!
    music->setVolume(0);
    music->play();

    playlistsfx = new QMediaPlaylist(this);
    playlistsfx->addMedia(QUrl("qrc:/Sounds/Sounds/ui_maprollover_flyout_01mod.wav"));
    playlistsfx->addMedia(QUrl("qrc:/Sounds/Sounds/ui_skill_increase.wav"));
    playlistsfx->setPlaybackMode(QMediaPlaylist::CurrentItemOnce);

    soundfx = new QMediaPlayer(this);
    soundfx->setPlaylist(playlistsfx);
    soundfx->setVolume(80);

    //This is retarded. Why the fuck can I not stop the mediaplayer without hearing cracks and pops?
    //Using 2 mediaplayers and flipping between them made it better. but still not good, and it's a
    //retarded solution.
    playlistsfx2 = new QMediaPlaylist(this);
    playlistsfx2->addMedia(QUrl("qrc:/Sounds/Sounds/ui_maprollover_flyout_01mod.wav"));
    playlistsfx2->addMedia(QUrl("qrc:/Sounds/Sounds/ui_skill_increase.wav"));
    playlistsfx2->setPlaybackMode(QMediaPlaylist::CurrentItemOnce);

    soundfx2 = new QMediaPlayer(this);
    soundfx2->setPlaylist(playlistsfx2);
    soundfx2->setVolume(80);

    flipSoundFXPlayer = false;

    connect(ui->wrGameWidget,SIGNAL(s_backToWRGameMenu()),this,SLOT(backToWRMenu()));
    connect(ui->hwMenu,SIGNAL(s_backToMainMenu()),this,SLOT(backToMainMenu()));
    connect(ui->hwMenu,SIGNAL(s_playSoundFX(int)),this,SLOT(playSoundFX(int)));
    connect(ui->hwMenu,SIGNAL(s_getMainWindowState()),this,SLOT(getMainWindowState()));

    QRect rec = QApplication::desktop()->availableGeometry(this);
    int width = rec.width();
    int height = rec.height();

    if(width<1600 || height <900){
        displayScreenResolutionError();
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::openHighScore(){
    ui->wr_highscore->setVisible(true);
    ui->wrMenu_exitbutton->setVisible(true);
    for (int i=0;i<10;i++){
        wr_highscore_score.value(i)->setVisible(true);
        wr_highscore_name.value(i)->setVisible(true);
        wr_highscore_new.value(i)->setVisible(true);
    }
    highScoreOpen = true;
}

void MainWindow::closeHighScore(){
    ui->wr_highscore->setVisible(false);
    ui->wrMenu_exitbutton->setVisible(false);
    ui->wrMenu_exitbuttonsel->setVisible(false);
    for (int i=0;i<10;i++){
        wr_highscore_score.value(i)->setVisible(false);
        wr_highscore_name.value(i)->setVisible(false);
        wr_highscore_new.value(i)->setVisible(false);
    }
    highScoreOpen = false;
}

void MainWindow::updateHighScore(int index, int score, QString name){
    QString s = QString::number(score);
    if (score !=0){
        wr_highscore_score.value(index)->setText(s);
        wr_highscore_name.value(index)->setText(name);
    }
}

void MainWindow::newHighScore(int index){
    wr_highscore_score.value(index)->setStyleSheet("QLabel { color : rgb(255, 255, 170); }");
    wr_highscore_name.value(index)->setStyleSheet("QLabel { color : rgb(255, 255, 170); }");
    wr_highscore_new.value(index)->setStyleSheet("QLabel { color : rgba(255, 255, 170, 255); }");
    openHighScore();
    playSoundFX(Sound_NewHighScore);
}

void MainWindow::unmarkHighScore(){
    for (int i=0;i<10;i++){
        wr_highscore_score.value(i)->setStyleSheet("QLabel { color : yellow; }");
        wr_highscore_name.value(i)->setStyleSheet("QLabel { color : yellow; }");
        wr_highscore_new.value(i)->setStyleSheet("QLabel { color : rgba(0, 0, 0, 0); }");
    }
}

void MainWindow::playSoundFX(int index){
    //This is retarded
    if (flipSoundFXPlayer==false){
        flipSoundFXPlayer=true;
        playlistsfx->setCurrentIndex(index);
        soundfx2->setMuted(true);
        soundfx->setMuted(false);
        soundfx->play();

    }else if (flipSoundFXPlayer==true){
        flipSoundFXPlayer=false;
        playlistsfx2->setCurrentIndex(index);
        soundfx->setMuted(true);
        soundfx2->setMuted(false);
        soundfx2->play();
    }
}

void MainWindow::backToWRMenu(){
    ui->stackedWidget->setCurrentIndex(Widget_WombatRunnerMenu);    
    wrGameObject->deleteLater();
}

void MainWindow::backToMainMenu(){
    ui->stackedWidget->setCurrentIndex(Widget_WorldofWombatsMenu);    
}

void MainWindow::getMainWindowState(){
    //HW_GAME_UI needs to know if the program is maximized or not, but I could not fucking figure out how
    //to access MainWindow, so now I am doing it like this, which is fucking retarded
    int state = this->windowState();
    ui->hwMenu->gui->mainWindowState = state;
}




//To get mouse enter/leave and mouse click functionality on qLabels
bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    // This function repeatedly call for those QObjects
    // which have installed eventFilter (Step 2)

   if (obj == (QObject*)ui->mainmenu_HWmousearea){
        if (event->type() == QEvent::Enter)
        {            
            playSoundFX(Sound_Menu);
            ui->mainmenu_HWsel->setVisible(true);
            ui->mainmenu_WRsel->setVisible(false);
            ui->mainmenu_CWsel->setVisible(false);
            ui->mainmenu_IAWsel->setVisible(false);
            ui->mainmenu_HW->setVisible(false);
            ui->mainmenu_WR->setVisible(true);
            ui->mainmenu_CW->setVisible(true);
            ui->mainmenu_IAW->setVisible(true);
        }else if (event->type() == QEvent::Leave) {
            ui->mainmenu_HW->setVisible(true);
            ui->mainmenu_HWsel->setVisible(false);
        }else if (event->type() == QEvent::MouseButtonRelease)
        {            
            ui->hwMenu->setup();
            ui->stackedWidget->setCurrentIndex(Widget_HabitatWombatMenu);            
        }

        return QWidget::eventFilter(obj, event);

    }else if (obj == (QObject*)ui->mainmenu_WRmousearea){
        if (event->type() == QEvent::Enter)
        {
            playSoundFX(Sound_Menu);
            ui->mainmenu_HWsel->setVisible(false);
            ui->mainmenu_WRsel->setVisible(true);
            ui->mainmenu_CWsel->setVisible(false);
            ui->mainmenu_IAWsel->setVisible(false);
            ui->mainmenu_HW->setVisible(true);
            ui->mainmenu_WR->setVisible(false);
            ui->mainmenu_CW->setVisible(true);
            ui->mainmenu_IAW->setVisible(true);
        }else if (event->type() == QEvent::Leave) {
            ui->mainmenu_WR->setVisible(true);
            ui->mainmenu_WRsel->setVisible(false);
        }else if (event->type() == QEvent::MouseButtonRelease){
            ui->stackedWidget->setCurrentIndex(Widget_WombatRunnerMenu);
            highscore = new WR_Highscore(this);
            connect(highscore,SIGNAL(s_updateHighScore(int,int,QString)),this,SLOT(updateHighScore(int,int,QString)));
            connect(highscore,SIGNAL(s_newHighScore(int)), this, SLOT(newHighScore(int)));
            connect(ui->wrGameWidget, SIGNAL(s_storeHighScoreOnFile()),highscore, SLOT(storeHighScoreOnFile()));
            highscore->setup();
        }

        return QWidget::eventFilter(obj, event);

    }else if (obj == (QObject*)ui->mainmenu_CWmousearea){
        if (event->type() == QEvent::Enter)
        {
            playSoundFX(Sound_Menu);
            ui->mainmenu_HWsel->setVisible(false);
            ui->mainmenu_WRsel->setVisible(false);
            ui->mainmenu_CWsel->setVisible(true);
            ui->mainmenu_IAWsel->setVisible(false);
            ui->mainmenu_HW->setVisible(true);
            ui->mainmenu_WR->setVisible(true);
            ui->mainmenu_CW->setVisible(false);
            ui->mainmenu_IAW->setVisible(true);
        }else if (event->type() == QEvent::Leave) {
            ui->mainmenu_CW->setVisible(true);
            ui->mainmenu_CWsel->setVisible(false);
        }else if (event->type() == QEvent::MouseButtonRelease)
        {

        }

        return QWidget::eventFilter(obj, event);

    }else if (obj == (QObject*)ui->mainmenu_IAWmousearea){
        if (event->type() == QEvent::Enter)
        {
            playSoundFX(Sound_Menu);
            ui->mainmenu_HWsel->setVisible(false);
            ui->mainmenu_WRsel->setVisible(false);
            ui->mainmenu_CWsel->setVisible(false);
            ui->mainmenu_IAWsel->setVisible(true);
            ui->mainmenu_HW->setVisible(true);
            ui->mainmenu_WR->setVisible(true);
            ui->mainmenu_CW->setVisible(true);
            ui->mainmenu_IAW->setVisible(false);
        }else if (event->type() == QEvent::Leave) {
            ui->mainmenu_IAW->setVisible(true);
            ui->mainmenu_IAWsel->setVisible(false);
        }

        return QWidget::eventFilter(obj, event);

     }else if (obj == (QObject*)ui->wrMenu_startmousearea){
        if (event->type() == QEvent::Enter)
        {
            playSoundFX(Sound_Menu);
            ui->wrMenu_startsel->setVisible(true);
            ui->wrMenu_highscoresel->setVisible(false);
            ui->wrMenu_quitsel->setVisible(false);
            ui->wrMenu_start->setVisible(false);
            ui->wrMenu_highscore->setVisible(true);
            ui->wrMenu_quit->setVisible(true);
        }else if (event->type() == QEvent::Leave) {
            ui->wrMenu_startsel->setVisible(false);
            ui->wrMenu_start->setVisible(true);
        }else if (event->type()== QEvent::MouseButtonPress){
            ui->stackedWidget->setCurrentIndex(Widget_WombatRunner);

            unmarkHighScore();

            wrGameObject = new WR_Game(this); //Is this created several times? fix!
            // Connect all signals and slots so that the game object can communicate with the main window            
            connect(wrGameObject, SIGNAL(s_updateCash(int,int)), ui->wrGameWidget, SLOT(updateCash(int,int)));
            connect(wrGameObject, SIGNAL(s_updateWombatPositions(int,int,int)), ui->wrGameWidget, SLOT(updateWombatPositions(int,int,int)));
            connect(wrGameObject, SIGNAL(s_startWombatAnimation()), ui->wrGameWidget, SLOT(startWombatAnimation()));
            connect(wrGameObject, SIGNAL(s_stopWombatAnimation()), ui->wrGameWidget, SLOT(stopWombatAnimation()));
            connect(wrGameObject, SIGNAL(s_disableInput()), ui->wrGameWidget, SLOT(disableInput()));
            connect(wrGameObject, SIGNAL(s_enableInput()), ui->wrGameWidget, SLOT(enableInput()));
            connect(wrGameObject, SIGNAL(s_clearInvalidBets(int,int)), ui->wrGameWidget, SLOT(clearInvalidBets(int,int)));
            connect(wrGameObject, SIGNAL(s_updateMessageBoard(QString)), ui->wrGameWidget, SLOT(updateMessageBoard(QString)));
            connect(wrGameObject, SIGNAL(s_resetMessageBoard()), ui->wrGameWidget, SLOT(resetMessageBoard()));
            connect(wrGameObject, SIGNAL(s_disablePlayer(int)), ui->wrGameWidget, SLOT(disablePlayer(int)));
            connect(wrGameObject, SIGNAL(s_checkHighScore(int,QString)), highscore, SLOT(checkHighScore(int,QString)));
            connect(wrGameObject, SIGNAL(s_markNewHighScore()), highscore, SLOT(markNewHighScore()));

            wrGameObject->setup();

            connect(ui->wrGameWidget, SIGNAL(s_setAllBets(QList<int>,QList<int>)),wrGameObject, SLOT(setAllBets(QList<int>,QList<int>)));
            connect(ui->wrGameWidget, SIGNAL(s_startRace()),wrGameObject, SLOT(startRace()));
            connect(ui->wrGameWidget, SIGNAL(s_checkIfNewHighScoreForAllPlayers()),wrGameObject, SLOT(checkIfNewHighScoreForAllPlayers()));
            connect(ui->wrGameWidget, SIGNAL(s_setLanguage(Language)),wrGameObject->announcer, SLOT(setLanguage(Language)));
            connect(ui->wrGameWidget, SIGNAL(s_addPlayer(QString)),wrGameObject, SLOT(addPlayer(QString)));
            connect(ui->wrGameWidget, SIGNAL(s_stopRace()),wrGameObject, SLOT(stopRace()));



        }

        return QWidget::eventFilter(obj, event);

    }else if (obj == (QObject*)ui->wrMenu_highscoremousearea){
       if (event->type() == QEvent::Enter)
       {
           playSoundFX(Sound_Menu);
           ui->wrMenu_startsel->setVisible(false);
           ui->wrMenu_highscoresel->setVisible(true);
           ui->wrMenu_quitsel->setVisible(false);
           ui->wrMenu_start->setVisible(true);
           ui->wrMenu_highscore->setVisible(false);
           ui->wrMenu_quit->setVisible(true);
       }else if (event->type() == QEvent::Leave) {
           ui->wrMenu_highscoresel->setVisible(false);
           ui->wrMenu_highscore->setVisible(true);
       }else if (event->type() == QEvent::MouseButtonPress)
       {
           openHighScore();
       }

       return QWidget::eventFilter(obj, event);

   }else if (obj == (QObject*)ui->wrMenu_quitmousearea){
       if (event->type() == QEvent::Enter)
       {
           playSoundFX(Sound_Menu);
           ui->wrMenu_startsel->setVisible(false);
           ui->wrMenu_highscoresel->setVisible(false);
           ui->wrMenu_quitsel->setVisible(true);
           ui->wrMenu_start->setVisible(true);
           ui->wrMenu_highscore->setVisible(true);
           ui->wrMenu_quit->setVisible(false);
       }else if (event->type() == QEvent::Leave) {
           ui->wrMenu_quitsel->setVisible(false);
           ui->wrMenu_quit->setVisible(true);
       }else if (event->type() == QEvent::MouseButtonPress)
       {
           ui->stackedWidget->setCurrentIndex(Widget_WorldofWombatsMenu);           
           unmarkHighScore();
           highscore->deleteLater();
       }

       return QWidget::eventFilter(obj, event);

   }else if (obj == (QObject*)ui->wrMenu_exitbuttonmousearea){
      if (highScoreOpen==true) {

           if (event->type() == QEvent::Enter)
           {
               ui->wrMenu_exitbutton->setVisible(false);
               ui->wrMenu_exitbuttonsel->setVisible(true);
           }else if (event->type() == QEvent::Leave) {
               ui->wrMenu_exitbutton->setVisible(true);
               ui->wrMenu_exitbuttonsel->setVisible(false);
           }else if (event->type() == QEvent::MouseButtonPress)
           {
                closeHighScore();
           }

      }
       return QWidget::eventFilter(obj, event);

   }else {
        // pass the event on to the parent class
        return QWidget::eventFilter(obj, event);
    }
}

void MainWindow::displayScreenResolutionError(){ // Not currently used
    QLabel *errorMessage = new QLabel(this);
    errorMessage->setText("A minimum screen resolution of 1600x900 px \n is needed to play this game.");
    errorMessage->setStyleSheet("QLabel { border: 2 px solid black; color: white; background: rgb(30, 0, 0);}");
    errorMessage->setGeometry((this->width()/2)-(350/2),(this->height()/2)-(50/2),350,50);
    errorMessage->setAlignment(Qt::AlignCenter);
    errorMessage->show();    
}


