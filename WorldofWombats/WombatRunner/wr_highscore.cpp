#include "wr_highscore.h"
#include <QFile>
#include <QTextStream>
#include <iostream>
#include <QRegExp>



WR_Highscore::WR_Highscore(QObject *parent) : QObject(parent)
{       

}

void WR_Highscore::setup(){

    //Read highscore from file and send to ui
    QString filename = "data";
    QString s;

    QFile file( filename );

    if ( file.open(QIODevice::ReadWrite | QIODevice::Text) )
    {
        QTextStream stream(&file);
        s = stream.readAll();
        file.close();
    }

    QStringList query = s.split("|");

    for (int i=0;i<20;i=i+2){
        wr_highscoreEntry highscoreEntry = {query.value(i+1).toInt(),query.value(i+2),false};
        highscoreArray.append(highscoreEntry);
    }

    for (int i=0;i<10;i++){        
        emit s_updateHighScore(i,highscoreArray.value(i).score,highscoreArray.value(i).name);
    }

}

void WR_Highscore::storeHighScoreOnFile(){
     QString filename = "data";
     QFile file( filename );
    if ( file.open(QIODevice::ReadWrite | QIODevice::Text) )
    {
        QTextStream stream(&file);
        for (int i=0;i<10;i++){
            QString s = "|"+QString::number(highscoreArray.value(i).score)+"|"+highscoreArray.value(i).name;
            stream << s;
        }
        file.close();
    }
}

void WR_Highscore::checkHighScore(int score, QString name){

    bool newHighScore = false;
    int pos;

    for (int i=0;i<10;i++){
        if (score>highscoreArray.value(i).score){
            newHighScore = true;
            pos = i;
            break;
        }
    }

    if (newHighScore) {
        wr_highscoreEntry highscoreEntry = {score,name,true};
        highscoreArray.insert(pos,highscoreEntry);

        highscoreArray.removeLast();

        for (int i=0;i<10;i++){
            emit s_updateHighScore(i,highscoreArray.value(i).score,highscoreArray.value(i).name);
        }

    }


}

void WR_Highscore::markNewHighScore(){
    for (int i=0;i<10;i++){
        if (highscoreArray.value(i).newHighScore == true){
            emit s_newHighScore(i);

            wr_highscoreEntry temp = highscoreArray.value(i);
            temp.newHighScore = false;
            highscoreArray.replace(i, temp);

        }
    }
}

