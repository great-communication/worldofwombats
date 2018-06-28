#include "level.h"
#include "worldbuilder.h"
#include <QFile>
#include <QDir>

Level::Level(QObject *parent) : QObject(parent){

}

void Level::setup(WorldBuilder *worldBuilder, QString levelName){
    this->worldBuilder = worldBuilder;
    this->name = levelName;

     QString fileName = "hw/"+name+"/level";
     if (worldBuilder->fileExists(fileName)){
        load();
     }else{
        create();
     }
}

void Level::create(){
    int rand1 = worldBuilder->getRandomNumber(1,RAND_MAX); //Remember: max value for int is 2147483647
    int rand2 = worldBuilder->getRandomNumber(1,RAND_MAX);
    seed = rand1*rand2;

    QDir dir("hw/"+name);
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    save();
}


void Level::load(){
    QString fileName = "hw/"+name+"/level";

    if (worldBuilder->fileExists(fileName)==false){
        return; //Early return, file does not exist...
    }

    QFile file(fileName);

    if (file.open(QIODevice::ReadOnly)){
        QDataStream stream(&file);
        qint32 temp;
        stream >> temp;
        seed = temp;
        stream >> temp;
        worldBuilder->worldPosition.setX(temp);
        stream >> temp;
        worldBuilder->worldPosition.setY(temp);
        stream >> temp;
        worldBuilder->month=temp;
        bool b;
        stream >> b;
        worldBuilder->isUnderground = b;
        stream >> b;
        worldBuilder->gameOver = b;
    }
}

void Level::save(){
        QString filename = "hw/"+name+"/level";
        QFile file( filename );

        if (file.open(QIODevice::WriteOnly)){
            QDataStream out(&file);
            out << (qint32)seed;
            out << (qint32)worldBuilder->worldPosition.x();
            out << (qint32)worldBuilder->worldPosition.y();
            out << (qint32)worldBuilder->month;
            out << (bool)worldBuilder->isUnderground;
            out << (bool)worldBuilder->gameOver;
        }
}

QString Level::getName(){
    return name;
}

int Level::getSeed(){
    return seed;
}
