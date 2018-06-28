#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QDebug>
#include "WombatRunner/wr_highscore.h"
#include "WombatRunner/wr_game.h"
#include "WombatRunner/wr_announcer.h"
#include <QLabel>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    bool eventFilter(QObject *obj, QEvent *event);    
    Ui::MainWindow *ui;    

signals:

public slots:
    void backToMainMenu();
    void playSoundFX(int index);
    void getMainWindowState(); //Retarded    

private:
    WR_Highscore *highscore;
    bool highScoreOpen;
    WR_Game *wrGameObject;    
    QList<QLabel*> wr_highscore_new,wr_highscore_score,wr_highscore_name;
    QMediaPlaylist *playlistsfx;
    QMediaPlayer *soundfx;
    QMediaPlaylist *playlistsfx2;
    QMediaPlayer *soundfx2;
    bool flipSoundFXPlayer;
    enum SoundFX {

        Sound_Menu,
        Sound_NewHighScore,
    };
    void unmarkHighScore();
    void openHighScore();
    void closeHighScore();
    void hw_createWorld();
    enum Widget {

        Widget_WorldofWombatsMenu,
        Widget_HabitatWombatMenu,            
        Widget_WombatRunnerMenu,
        Widget_WombatRunner,
        Widget_HabitatWombat,    //Not used
    };    
    void displayScreenResolutionError();

private slots:    
    void updateHighScore (int index, int score, QString name);
    void newHighScore (int index);
    void backToWRMenu();
};



#endif // MAINWINDOW_H
