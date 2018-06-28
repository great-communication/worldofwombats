#include "registry.h"
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QTextStream>
#include <iostream>
#include <QDebug>
#include <windows.h>
#include "worldbuilder.h"
#include "worker.h"
#include "herder.h"
#include "herdmember.h"
#include "level.h"
#include "world.h"
#include "imagepainter.h"

Registry::Registry(QObject *parent) : QObject(parent){

}

void Registry::setup(){
    wombatIdCount = 0;
    selectedWombat = None;
    guiWInventoryWidgetOpen = false;
    guiWInventoryWidgetWombatId = None;
    guiInfoBoxOpen = false;
    guiStorageWidgetOpen = false;
    guiStorageWidgetStorageId = None;
    wombatIdGuiInfoBox = None;

    readTextFile("hw/names_burrow.txt",burrowNames);
    readTextFile("hw/names_female.txt",femaleNames);
    readTextFile("hw/names_male.txt",maleNames);
    readTextFile("hw/nicknames_female.txt",femaleNicknames);
    readTextFile("hw/nicknames_male.txt",maleNicknames);
    loadBurrowsFromFile();
    loadWombatsFromFile();          

    if (worldBuilder->gameOver==false){
        if (wombats.length()<1){
            createMiriamAndWarren();
        } else if (selectedWombat==None){
            worker->setLockMovement(true);
            worker->setSelectedWombatVisibility(false);
        }else{
            int wombatIndex = getWombatIndex(selectedWombat);
            bool underground = wombats.value(wombatIndex).underground;
            worker->addWombatToDisplay(selectedWombat,worldBuilder->worldPosition,QPoint(0,0),0,underground,2,WombatAITask_Idling); // Add selected wombat to display
        }
    }
}

void Registry::createMiriamAndWarren(){
    createWombat(QPoint(0,0),false,None);
    createWombat(QPoint(1,0),false,None);

    if (wombats[0].sex == Female){
        wombats[0].name = "Miriam";
        wombats[1].name = "Warren";
        wombats[1].sex = Male;
    }else{
        wombats[0].name = "Warren";
        wombats[1].name = "Miriam";
        wombats[1].sex = Female;
    }
    wombats[0].age = 2;
    wombats[1].age = 2;

    selectedWombat=wombats.value(0).id;
    worldBuilder->addWombatIdToTile(QPoint(1,0),wombatIds.last());
    worker->addWombatToDisplay(wombats.value(0).id,worldBuilder->worldPosition,QPoint(0,0),0,false,2,WombatAITask_Idling); // Add selected wombat to display
    herder->addWombatToHerd(wombatIds.last(),QPoint(1,0),QPoint(0,0),0);
}

void Registry::readTextFile(QString fileName, QVector<QString> &array){

    if (fileExists(fileName)==false){
        QString name = "file not found";
        array.append(name);
    }else{

        QFile file(fileName);
        QString s;

        if ( file.open(QIODevice::ReadWrite | QIODevice::Text) )
        {
            QTextStream stream(&file);
            s = stream.readAll();
            file.close();
        }

        QStringList query = s.split("\n");

        for (int i=0;i<query.length();i++){
            array.append(query.value(i));
        }
    }
}

void Registry::loadBurrowsFromFile(){
    QString levelName = worldBuilder->level->getName();
    QString filename = "hw/"+levelName+"/burrows";

    if (fileExists(filename)==true){
        QFile file(filename);

        if (file.open(QIODevice::ReadOnly)){
            QDataStream stream(&file);
            int length;
            stream >> length;

            for (int i=0;i<length;i++){
                BurrowStruct burrow;
                stream >> burrow.id;
                burrowIds.append(burrow.id);
                stream >> burrow.name;
                stream >> burrow.food;
                int riceGrassesLength = 0;
                stream >> riceGrassesLength;
                QVector<BurrowResourceStruct> riceGrasses;
                for (int j=0;j<riceGrassesLength;j++){
                    BurrowResourceStruct riceGrass;
                    int x;
                    stream >> x;
                    int y;
                    stream >> y;
                    riceGrass.pos = QPoint(x,y);
                    stream >> riceGrass.quantity;
                    stream >> riceGrass.quantityAvailable;
                    stream >> riceGrass.growth;
                    riceGrasses.append(riceGrass);
                }
                burrow.riceGrasses = riceGrasses;
                stream >> burrow.size;
                stream >> burrow.population;
                stream >> burrow.entrances;

                QVector<int> wombatIds;
                int temp;
                for (int j=0;j<burrow.population;j++){
                    stream >> temp;
                    wombatIds.append(temp);
                }
                burrow.populationWombatIds = wombatIds;

                QVector<QPoint> entrancePos;
                for (int j=0;j<burrow.entrances;j++){
                    int x;
                    int y;
                    stream >> x;
                    stream >> y;
                    entrancePos.append(QPoint(x,y));
                }
                burrow.entrancePos = entrancePos;

                QVector<QPoint> remainingPos;
                for (int j=0;j<burrow.size;j++){
                    int x;
                    int y;
                    stream >> x;
                    stream >> y;
                    remainingPos.append(QPoint(x,y));
                }
                burrow.allNonEntrancePos = remainingPos;

                QVector<StorageStruct> storages;

                int storagesLength;
                stream >> storagesLength;
                for (int j=0;j<storagesLength;j++){
                    StorageStruct storage;
                    stream >> storage.id;
                    QVector<StorageTileStruct> allTiles;
                    StorageTileStruct storageTile;
                    int allTilesLength;
                    stream >> allTilesLength;
                    for (int k=0;k<allTilesLength;k++){
                        int x;
                        int y;
                        stream >> x;
                        stream >> y;
                        storageTile.pos = QPoint(x,y);
                        int visible;
                        stream >> visible;
                        storageTile.visible = (bool) visible;
                        int inventoryLength;
                        stream >> inventoryLength;
                        QVector<InventoryItem> inventory;
                        for (int l=0;l<inventoryLength;l++){
                            InventoryItem item;
                            int resource;
                            stream >> resource;
                            item.resource = (Resource) resource;
                            stream >> item.amount;
                            inventory.append(item);
                        }
                        storageTile.inventory = inventory;
                        allTiles.append(storageTile);
                    }
                    storage.allTiles = allTiles;

                    int acceptedResourcesLength;
                    stream >> acceptedResourcesLength;
                    QVector<bool> acceptedResources;
                    for (int k=0;k<acceptedResourcesLength;k++){
                        bool temp;
                        stream >> temp;
                        acceptedResources.append(temp);
                    }
                    storage.acceptedResources = acceptedResources;
                    storages.append(storage);
                }
                burrow.storages = storages;
                stream >> burrow.workBuffer;
                stream >> burrow.eatBuffer;
                int grassesLength = 0;
                stream >> grassesLength;
                QVector<BurrowResourceStruct> grasses;
                for (int j=0;j<grassesLength;j++){
                    BurrowResourceStruct grass;
                    int x;
                    stream >> x;
                    int y;
                    stream >> y;
                    grass.pos = QPoint(x,y);
                    stream >> grass.quantity;
                    stream >> grass.quantityAvailable;
                    stream >> grass.growth;
                    grasses.append(grass);
                }
                burrow.grasses = grasses;

                int bushesLength = 0;
                stream >> bushesLength;
                QVector<BurrowResourceStruct> bushes;
                for (int j=0;j<bushesLength;j++){
                    BurrowResourceStruct bush;
                    int x;
                    stream >> x;
                    int y;
                    stream >> y;
                    bush.pos = QPoint(x,y);
                    stream >> bush.quantity;
                    stream >> bush.quantityAvailable;
                    stream >> bush.growth;
                    bushes.append(bush);
                }
                burrow.bushes = bushes;

                int nestsLength = 0;
                stream >> nestsLength;
                QVector<QPoint> nests;
                for (int j=0;j<nestsLength;j++){
                    int x;
                    stream >> x;
                    int y;
                    stream >> y;
                    nests.append(QPoint(x,y));
                }
                burrow.nests = nests;

                burrows.append(burrow);
            }
        file.close();
        }
    }
}

void Registry::loadWombatsFromFile(){
    QString levelName = worldBuilder->level->getName();
    QString filename = "hw/"+levelName+"/wombats";

    if (fileExists(filename)==true){
        QFile file(filename);
        QString s;

        if ( file.open(QIODevice::ReadWrite | QIODevice::Text) )
        {
            QTextStream stream(&file);
            s = stream.readAll();
            file.close();
        }

        QStringList query = s.split("\n");
        wombatIdCount = query.value(0).toInt();
        selectedWombat = query.value(1).toInt();

        for (int i=2;i<query.length()-1;i++){   //-1 because extra newline...
            QStringList query2 = query.value(i).split("|");
            WombatStruct wombat;
            wombat.id = query2.value(0).toInt();
            wombatIds.append(wombat.id);
            wombat.name = query2.value(1);
            wombat.sex = query2.value(2).toInt();
            wombat.age = query2.value(3).toInt();
            wombat.birthMonth = query2.value(4).toInt();
            wombat.burrowId = query2.value(5).toInt();
            int x = query2.value(6).toInt();
            int y = query2.value(7).toInt();
            wombat.pos = QPoint (x,y);
            wombat.underground = query2.value(8).toInt();
            //Load inventory
            int inventoryLength = query2.value(9).toInt();
            //console->print(QString::number(inventoryLength));
            QVector<InventoryItem> inventory;
            for (int j=0;j<inventoryLength*2;j+=2){
                InventoryItem item;
                item.resource = (Resource)query2.value(10+j).toInt();
                item.amount = query2.value(11+j).toInt();
                inventory.append(item);
            }
            wombat.inventory = inventory;
            wombat.health = query2.value(9+(inventoryLength*2)+1).toInt();
            wombat.rested = query2.value(9+(inventoryLength*2)+2).toInt();
            wombat.plump = query2.value(9+(inventoryLength*2)+3).toInt();
            //
            wombats.append(wombat);
        }
    }
}

bool Registry::fileExists(QString path) {
    QFileInfo check_file(path);
    // Check if file exists and if yes: Is it really a file and no directory?
    if (check_file.exists() && check_file.isFile()) {
        return true;
    } else {
        return false;
    }
}

void Registry::saveBurrowsToFile(){
    QString levelName = worldBuilder->level->getName();
    QDir dir("hw/"+levelName);
    if (!dir.exists()) {
        dir.mkpath(".");
    }

    QString filename = "hw/"+levelName+"/burrows";
    if (fileExists(filename)==true){
        QFile oldFile (filename);
        oldFile.remove();
    }

    QFile file(filename);
    if (file.open(QIODevice::WriteOnly))
    {
        QDataStream stream(&file);
        stream << (qint32) burrows.length();

        for (int i=0;i<burrows.length();i++){
            stream << (qint32)burrows.value(i).id;
            stream << burrows.value(i).name;
            stream << (qint32)burrows.value(i).food;
            stream << (qint32)burrows.value(i).riceGrasses.length();
            for (int j=0;j<burrows.value(i).riceGrasses.length();j++){
                stream << (qint32)burrows.value(i).riceGrasses.value(j).pos.x();
                stream << (qint32)burrows.value(i).riceGrasses.value(j).pos.y();
                stream << (qint32)burrows.value(i).riceGrasses.value(j).quantity;
                stream << (double)burrows.value(i).riceGrasses.value(j).quantityAvailable;
                stream << (double)burrows.value(i).riceGrasses.value(j).growth;
            }

            stream << (qint32)burrows.value(i).size;
            stream << (qint32)burrows.value(i).population;
            stream << (qint32)burrows.value(i).entrances;
            for (int j=0;j<burrows.value(i).populationWombatIds.length();j++){
                stream << (qint32)burrows.value(i).populationWombatIds.value(j);
            }
            for (int j=0;j<burrows.value(i).entrancePos.length();j++){
                int x = burrows.value(i).entrancePos.value(j).x();
                int y = burrows.value(i).entrancePos.value(j).y();
                stream << (qint32)x;
                stream << (qint32)y;
            }
            for (int j=0;j<burrows.value(i).allNonEntrancePos.length();j++){
                int x = burrows.value(i).allNonEntrancePos.value(j).x();
                int y = burrows.value(i).allNonEntrancePos.value(j).y();
                stream << (qint32)x;
                stream << (qint32)y;
            }
            stream << (qint32)burrows.value(i).storages.length();
            for (int j=0;j<burrows.value(i).storages.length();j++){
                stream << (qint32)burrows.value(i).storages.value(j).id;
                stream << (qint32)burrows.value(i).storages.value(j).allTiles.length();
                for (int k=0;k<burrows.value(i).storages.value(j).allTiles.length();k++){
                    stream << (qint32)burrows.value(i).storages.value(j).allTiles.value(k).pos.x();
                    stream << (qint32)burrows.value(i).storages.value(j).allTiles.value(k).pos.y();
                    stream << (qint32)burrows.value(i).storages.value(j).allTiles.value(k).visible;
                    stream << (qint32)burrows.value(i).storages.value(j).allTiles.value(k).inventory.length();
                    for (int l=0;l<burrows.value(i).storages.value(j).allTiles.value(k).inventory.length();l++){
                        stream << (qint32)burrows.value(i).storages.value(j).allTiles.value(k).inventory.value(l).resource;
                        stream << (qint32)burrows.value(i).storages.value(j).allTiles.value(k).inventory.value(l).amount;
                    }
                }
                stream << (qint32) burrows.value(i).storages.value(j).acceptedResources.length();
                for (int k=0;k<burrows.value(i).storages.value(j).acceptedResources.length();k++){
                    stream << (bool)burrows.value(i).storages.value(j).acceptedResources.value(k);
                }

            }

            stream << (qint32)burrows.value(i).workBuffer;
            stream << (qint32)burrows.value(i).eatBuffer;
            stream << (qint32)burrows.value(i).grasses.length();
            for (int j=0;j<burrows.value(i).grasses.length();j++){
                stream << (qint32)burrows.value(i).grasses.value(j).pos.x();
                stream << (qint32)burrows.value(i).grasses.value(j).pos.y();
                stream << (qint32)burrows.value(i).grasses.value(j).quantity;
                stream << (double)burrows.value(i).grasses.value(j).quantityAvailable;
                stream << (double)burrows.value(i).grasses.value(j).growth;
            }
            stream << (qint32)burrows.value(i).bushes.length();
            for (int j=0;j<burrows.value(i).bushes.length();j++){
                stream << (qint32)burrows.value(i).bushes.value(j).pos.x();
                stream << (qint32)burrows.value(i).bushes.value(j).pos.y();
                stream << (qint32)burrows.value(i).bushes.value(j).quantity;
                stream << (double)burrows.value(i).bushes.value(j).quantityAvailable;
                stream << (double)burrows.value(i).bushes.value(j).growth;
            }
            stream << (qint32)burrows.value(i).nests.length();
            for (int j=0;j<burrows.value(i).nests.length();j++){
                stream << (qint32)burrows.value(i).nests.value(j).x();
                stream << (qint32)burrows.value(i).nests.value(j).y();
            }

        }
        file.close();
    }
}

void Registry::saveWombatsToFile(){
    QString levelName = worldBuilder->level->getName();
    QDir dir("hw/"+levelName);
    if (!dir.exists()) {
        dir.mkpath(".");
    }

    QString filename = "hw/"+levelName+"/wombats";
    if (fileExists(filename)==true){
        QFile oldFile (filename);
        oldFile.remove();
    }

    QFile file(filename);
    if ( file.open(QIODevice::ReadWrite | QIODevice::Text) )
    {
        QTextStream stream(&file);
        QString s;
        s = QString::number(wombatIdCount);
        stream << s;
        stream << endl;
        s = QString::number(selectedWombat);
        stream << s;
        stream << endl;

        for (int i=0;i<wombats.length();i++){            
            s = QString::number(wombats.value(i).id)+"|";
            s += wombats.value(i).name+"|";
            s += QString::number(wombats.value(i).sex)+"|";
            s += QString::number(wombats.value(i).age)+"|";
            s += QString::number(wombats.value(i).birthMonth)+"|";
            s += QString::number(wombats.value(i).burrowId)+"|";
            s += QString::number(wombats.value(i).pos.x())+"|";
            s += QString::number(wombats.value(i).pos.y())+"|";
            s += QString::number(wombats.value(i).underground)+"|";
            s += QString::number(wombats.value(i).inventory.length())+"|";
            for (int j=0;j<wombats.value(i).inventory.length();j++){
                s += QString::number(wombats.value(i).inventory.value(j).resource)+"|";
                s += QString::number(wombats.value(i).inventory.value(j).amount)+"|";
            }
            s += QString::number(wombats.value(i).health)+"|";
            s += QString::number(wombats.value(i).rested)+"|";
            s += QString::number(wombats.value(i).plump)+"|";
            stream << s;
            stream << endl;
        }
        file.close();
    }
}

void Registry::saveBurrowsToTextFile(){
// Old! QVector<RiceGrassStruct> riceGrasses  and int workbuffer missing! + other things
/*
    QString filename = "hw/"+worldBuilder->levelName+"/burrowdebug";
    if (fileExists(filename)==true){
        QFile oldFile (filename);
        oldFile.remove();
    }
    QFile file(filename);
    if ( file.open(QIODevice::ReadWrite | QIODevice::Text) )
    {
        QTextStream stream(&file);
        QString s;
        s = "Total number of burrows:" +QString::number(burrows.length());
        stream << s;
        stream << endl;

        for (int i=0;i<burrows.length();i++){
            s = "id: "+QString::number(burrows.value(i).id);
            stream << s;
            stream << endl;

            s = "name: "+burrows.value(i).name;
            stream << s;
            stream << endl;

            s = "food: "+QString::number(burrows.value(i).food);
            stream << s;
            stream << endl;

            s = "size: "+QString::number(burrows.value(i).size);
            stream << s;
            stream << endl;

            s = "pop: "+QString::number(burrows.value(i).population);
            stream << s;
            stream << endl;

            s = "entrances: "+QString::number(burrows.value(i).entrances);
            stream << s;
            stream << endl;

            s = "wombatIds: ";
            stream << s;
            for (int j=0;j<burrows.value(i).populationWombatIds.length();j++){
                s = QString::number(burrows.value(i).populationWombatIds.value(j));
                stream << s;
                s = "|";
                stream << s;
            }
            stream << endl;

            s = "entrancesPos: ";
            stream << s;
            for (int j=0;j<burrows.value(i).entrancePos.length();j++){
                s = QString::number(burrows.value(i).entrancePos.value(j).x());
                stream << s;
                s = QString::number(burrows.value(i).entrancePos.value(j).y());
                stream << s;
                s = "|";
                stream << s;

            }
            stream << endl;

            s = "allPos: ";
            stream << s;
            for (int j=0;j<burrows.value(i).size;j++){
                s = QString::number(burrows.value(i).allNonEntrancePos.value(j).x());
                stream << s;
                s = QString::number(burrows.value(i).allNonEntrancePos.value(j).y());
                stream << s;
                s = "|";
                stream << s;
            }stream << endl;
            s = "storagesLength: "+QString::number(burrows.value(i).storages.length())+" ";
            stream << s;
            for (int j=0;j<burrows.value(i).storages.length();j++){
                s = "storage id: "+QString::number(burrows.value(i).storages.value(j).id)+" ";
                stream << s;
                s = "allTilesLength: "+ QString::number(burrows.value(i).storages.value(j).allTiles.length())+" ";
                stream << s;
                for (int k=0;k<burrows.value(i).storages.value(j).allTiles.length();k++){

                    s = "posx: "+ QString::number(burrows.value(i).storages.value(j).allTiles.value(k).pos.x())+" ";
                    stream << s;
                    s = "posy: "+ QString::number(burrows.value(i).storages.value(j).allTiles.value(k).pos.y())+" ";
                    stream << s;
                    s = "visible: "+ QString::number(burrows.value(i).storages.value(j).allTiles.value(k).visible)+" ";
                    stream << s;
                    s = "inventoryLength: "+ QString::number(burrows.value(i).storages.value(j).allTiles.value(k).inventory.length())+" ";
                    stream << s;
                    for (int l=0;l<burrows.value(i).storages.value(j).allTiles.value(k).inventory.length();l++){
                        s = "resource: "+ QString::number(burrows.value(i).storages.value(j).allTiles.value(k).inventory.value(l).resource)+" ";
                        stream << s;
                        s = "resource: "+ QString::number(burrows.value(i).storages.value(j).allTiles.value(k).inventory.value(l).amount)+" ";
                        stream << s;
                    }
                }
            }


            stream << endl;
        }
        file.close();
    }*/
}

void Registry::createBurrow(QPoint pos, QString burrowName){
    if (selectedWombat!=None){
        BurrowStruct burrow;
        burrow.id = burrows.length()+1;
        burrowIds.append(burrow.id);
        burrow.name = burrowName;
        burrow.food = 0;
        QVector<QVector<QPoint>> tiles;
        tiles = exploreTunnel(pos);
        burrow.size = tiles.value(0).length()+tiles.value(1).length(); // Holes + tunnels
        QVector<int> wombatIds;
        wombatIds.append(selectedWombat);
        burrow.population = wombatIds.length();
        burrow.entrances = tiles.value(0).length();
        burrow.populationWombatIds = wombatIds;        
        burrow.entrancePos = tiles.value(0);
        burrow.allNonEntrancePos = tiles.value(0)+tiles.value(1);
        burrow.workBuffer = 0;
        burrows.append(burrow);
        //Add burrow to tiles
        for (int i=0;i<tiles.value(0).length();i++){
            worldBuilder->changeTerrain(tiles.value(0).value(i),Terrain_Burrow,burrow.id,true);

        }
        for (int i=0;i<tiles.value(1).length();i++){
            worldBuilder->changeTerrain(tiles.value(1).value(i),Terrain_Burrow,burrow.id,true);
        }

        //Set this burrow as the selected wombat's new home
        int index = getWombatIndex(selectedWombat);
        if (index!=-1){
            int oldBurrowIndex = getBurrowIndex(wombats.value(index).burrowId);
            if (oldBurrowIndex!=None){
                burrows[oldBurrowIndex].populationWombatIds.removeOne(selectedWombat); // If wombat already belongs to a burrow, remove it from the previous burrow
                burrows[oldBurrowIndex].population -= 1;
            }
            wombats[index].burrowId = burrow.id; // Set new home            
        }else{
            worker->consolePrint("Error1 createBurrow");
        }
    }else{
        worker->consolePrint("Error2 createBurrow");
    }
}

QVector<QVector<QPoint>> Registry::exploreTunnel(QPoint pos){
//Explores the tunnels to find the extent of the burrow
    int x = pos.x();
    int y = pos.y();
    QPoint currentPos = pos;
    QVector<QPoint> tunnelCoords;
    QVector<QPoint> holeCoords;
    if (worldBuilder->hasTerrain(Terrain_Hole, false, currentPos)){
        holeCoords.append(pos);
    }else{
        tunnelCoords.append(pos);
    }
    QList<QPoint> searchPath;
    searchPath.append(pos);
    bool startOver;
    bool done = false;

    while (!done){
        startOver = false;
        x = y = 0;

        for (int i=0;i<4 && startOver==false;i++){
            if (i==0){
                x=-1;
                y=0;
            }else if (i==1){
                x=0;
                y=-1;
            }else if (i==2){
                x=0;
                y=1;
            } else if (i==3){
                x=1;
                y=0;
            }


            bool hasHole = worldBuilder->hasTerrain(Terrain_Hole,false,currentPos+QPoint(x,y));
            bool hasTunnel = worldBuilder->hasTerrain(Terrain_Tunnel,true,currentPos+QPoint(x,y));

            if(hasHole){
                pos = currentPos+QPoint(x,y);
                if (holeCoords.indexOf(pos)==-1){
                    holeCoords.append(pos);
                    searchPath.append(pos);
                    currentPos = pos;
                    startOver = true;
                }
            }else if(hasTunnel){
                pos = currentPos+QPoint(x,y);
                if (tunnelCoords.indexOf(pos)==-1){
                    tunnelCoords.append(pos);
                    searchPath.append(pos);
                    currentPos = pos;
                    startOver = true;
                }
            }
        }

        if (startOver==false){
            searchPath.pop_back();

            if (searchPath.length()==0){
                done=true;
            }else{
                pos = searchPath.last();
                currentPos=pos;
            }
        }
    }   

    QVector<QVector<QPoint>> bothLists;
    bothLists.append(holeCoords);
    bothLists.append(tunnelCoords);
    return bothLists;
}

void Registry::createWombat(QPoint pos, bool underground, int burrowId){
    WombatStruct wombat;
    wombatIdCount++;
    wombat.id = wombatIdCount;
    wombatIds.append(wombat.id);
    wombat.sex = worldBuilder->getRandomNumber(0,1);
    wombat.name = getRandomWombatName(wombat.sex);
    wombat.age = 0;
    wombat.birthMonth = worldBuilder->month;
    wombat.burrowId = burrowId;
    wombat.pos = pos;
    wombat.underground = underground;
    QVector<InventoryItem> inventory;
    wombat.inventory = inventory;
    wombat.health = 100;    //Decreases with damage and starvation
    wombat.rested = 100;    //Decreases when awake, decreases more when working, increases when sleeping
    wombat.plump = 100;     //Decreases continuously, increases when eating
    wombats.append(wombat);

    if (burrowId != -1){
        burrows[burrowId-1].population += 1;
        burrows[burrowId-1].populationWombatIds.append(wombat.id);
    }
}

QString Registry::getRandomBurrowName(){
    int random = worldBuilder->getRandomNumber(0,burrowNames.length()-1);
    return burrowNames.value(random);
}

QString Registry::getRandomWombatName(bool sex){
    QString name;
    QString nickname;

    if (sex==Female){
        int random = worldBuilder->getRandomNumber(0,femaleNames.length()-1);
        name = femaleNames.value(random);
        random = worldBuilder->getRandomNumber(0,10);
        if (random<10){
            random = worldBuilder->getRandomNumber(0,femaleNicknames.length()-1);
            nickname = femaleNicknames.value(random);
        }else{
            random = worldBuilder->getRandomNumber(0,maleNicknames.length()-1);
            nickname = maleNicknames.value(random);
        }
    }else{
        int random = worldBuilder->getRandomNumber(0,maleNames.length()-1);
        name = maleNames.value(random);
        random = worldBuilder->getRandomNumber(0,10);
        if (random<10){
            random = worldBuilder->getRandomNumber(0,maleNicknames.length()-1);
            nickname = maleNicknames.value(random);
        }else{
            random = worldBuilder->getRandomNumber(0,femaleNicknames.length()-1);
            nickname = femaleNicknames.value(random);
        }
    }

    return name+" "+nickname;
}

int Registry::getRandomWombatIdForPossess(){
    int wombatId = None;

    int length = wombatIds.length();
    if (length>0){
        for (int i=0;i<6;i++){
            int randomIndex = worldBuilder->getRandomNumber(0,length-1);
            wombatId = wombatIds.value(randomIndex);
            int wombatIndex = getWombatIndex(wombatId);
                if (wombats.value(wombatIndex).age>1){
                    return wombatId;
                }

            if (i==5){
                for (int j=0;j<length;j++){
                    wombatId = wombatIds.value(j);
                    wombatIndex = getWombatIndex(wombatId);
                    if (wombats.value(wombatIndex).age>1){
                        return wombatId;
                    }
                }
            }
        }
    }
    return wombatId;
}

void Registry::changeHomeBurrow(QPoint pos){
    int newBurrowId = worldBuilder->getBurrowId(pos);

    int index = getWombatIndex(selectedWombat);
    int oldBurrowId = -1;
    if (index!=-1){
        oldBurrowId = wombats.value(index).burrowId;
    }

    if (newBurrowId!=oldBurrowId){
        if (oldBurrowId!=-1){
            burrows[oldBurrowId-1].populationWombatIds.removeOne(selectedWombat); // If wombat already belongs to a burrow, remove it from the previous burrow
            burrows[oldBurrowId-1].population -= 1;
        }

        if (index!=-1){
            wombats[index].burrowId = newBurrowId; // Set new home
        }else{
            //console->print("Invalid index error2");
        }
        //Add to new burrow
        burrows[newBurrowId-1].populationWombatIds.append(selectedWombat);
        burrows[newBurrowId-1].population += 1;

        QVector<QString> strings;
        strings.append(wombats.value(index).name);
        strings.append("New home burrow");
        strings.append(burrows[newBurrowId-1].name);
        strings.append("");
        strings.append("");
        worker->displayTileInfo(strings);
    }
}

void Registry::setSelectedWombat(int wombatId){
   selectedWombat = wombatId;
}

int Registry::getWombatIndex(int wombatId){
    return wombatIds.indexOf(wombatId);
}

int Registry::getBurrowIndex(int burrowId){
    int index = -1;
    for (int i=0;i<burrows.length();i++){
        if (burrows[i].id == burrowId){
            index = i;
            break;
        }
    }
    return index;
}

void Registry::newMonth(){     
    makeWombatsOlder();
    growResources();
    calculateWorkDone();
    makeHerdedWombatsTired();
    makeWombatsHungry();
    emit s_newMonthUpdateDone();    
}

void Registry::makeHerdedWombatsTired(){
    for (int i=0;i<herder->wombatHerd.length();i++){
        if(herder->wombatHerd.value(i)->currentTask != WombatAITask_Sleeping){
            int wombatIndex = getWombatIndex(herder->wombatHerd.value(i)->wombatId);
            if(wombatIndex!=None){
                if (wombats.value(wombatIndex).rested>=5){
                    wombats[wombatIndex].rested -= 5;
                }
            }
        }
    }
}

void Registry::makeWombatsHungry(){
    QVector<int> killList;
    QVector<CauseOfDeath> causeOfDeath;
    for (int i=0;i<burrows.length();i++){
        for (int j=0;j<burrows.value(i).populationWombatIds.length();j++){
            if(burrows.value(i).populationWombatIds.value(j) != selectedWombat){
                int wombatId = burrows.value(i).populationWombatIds.value(j);
                int wombatIndex = getWombatIndex(wombatId);
                if(wombatIndex>=0 && wombatIndex<wombats.length()){
                    wombats[wombatIndex].plump -= 8;
                    if(wombats.value(wombatIndex).plump < 0){
                        wombats[wombatIndex].plump = 0;
                        wombats[wombatIndex].health -= 16;
                        if(wombats.value(wombatIndex).health <= 0){
                            killList.append(wombatIndex);
                            causeOfDeath.append(CauseOfDeath_Starvation);
                        }
                    }

                    bool wombatIsHerded = false;
                    for (int k=0;k<herder->wombatHerd.length();k++){
                        if(herder->wombatHerd.value(k)->wombatId == wombatId){
                            wombatIsHerded = true;
                            break;
                        }
                    }

                    if (!wombatIsHerded && wombatId != selectedWombat){
                        if(wombats.value(wombatIndex).plump<50){
                            //Remove food
                            feedWombat(wombatIndex,i);
                        }
                    }
                }else{
                    worker->consolePrint("Error makeWombatsHungry");
                }
            }
        }
    }

    if (killList.length()>0){
        //worker->consolePrint("killing "+QString::number(killList.length())+" wombats because of starvation");
        for (int k=0;k<killList.length();k++){
            //worker->consolePrint("killing "+wombats.value(killList.value(k)).name+" in burrow "+QString::number(wombats.value(killList.value(k)).burrowId));
        }
        killWombats(killList,causeOfDeath);
    }
}

void Registry::feedWombat(int wombatIndex,int burrowIndex){
    //worker->consolePrint("force feeding wombat: "+wombats.value(wombatIndex).name);

    //Frist check own inventory??


    //First check random rice grass
    if(burrows.value(burrowIndex).riceGrasses.length()>0){
        int randomRiceGrass = worldBuilder->getRandomNumber(0,burrows.value(burrowIndex).riceGrasses.length()-1);
        if (burrows.value(burrowIndex).riceGrasses.value(randomRiceGrass).quantityAvailable > 6){
            burrows[burrowIndex].riceGrasses[randomRiceGrass].quantityAvailable -= 6;
            wombats[wombatIndex].plump += 6*8;
        }else{
            int available = burrows.value(burrowIndex).riceGrasses.value(randomRiceGrass).quantityAvailable;
            burrows[burrowIndex].riceGrasses[randomRiceGrass].quantityAvailable = 0;
            wombats[wombatIndex].plump += available*8;
        }

        //If not enough, check all ricegrasses
        if(wombats.value(wombatIndex).plump<50){
            for (int i=0;i<burrows.value(burrowIndex).riceGrasses.length();i++){
                if (burrows.value(burrowIndex).riceGrasses.value(i).quantityAvailable > 6){
                    burrows[burrowIndex].riceGrasses[i].quantityAvailable -= 6;
                    wombats[wombatIndex].plump += 6*8;
                }else{
                    int available = burrows.value(burrowIndex).riceGrasses.value(i).quantityAvailable;
                    burrows[burrowIndex].riceGrasses[i].quantityAvailable = 0;
                    wombats[wombatIndex].plump += available*8;
                }

                if(wombats.value(wombatIndex).plump>50){
                    break;
                }
            }
        }
    }

    //If still not enough, check storage
    if(wombats.value(wombatIndex).plump<50){
        for (int i=0;i<burrows.value(burrowIndex).storages.length();i++){
            int storageIndex = getStorageIndex(burrowIndex,burrows.value(burrowIndex).storages.value(i).id);
            QVector<InventoryItem> items = getStorageInventory(burrowIndex,storageIndex);
            for (int j=0;j<items.length();j++){
                if(items.value(j).resource == Resource_RiceGrass){
                    int amount = items.value(j).amount;
                    wombats[wombatIndex].plump += amount*8;
                    removeItemFromStorage(burrowIndex,storageIndex,items.value(j));
                    break;
                }

                if(wombats.value(wombatIndex).plump>50){
                    break;
                }
            }
        }
    }
}

void Registry::calculateFoodUsed(){
    for (int i=0;i<burrows.length();i++){
        double foodEatenThisMonth = (double)burrows.value(i).populationWombatIds.length()*(25.0/10.0);
        burrows[i].eatBuffer += foodEatenThisMonth;
    }
}

void Registry::makeWombatsOlder(){    
    QVector<int> killList;
    QVector<CauseOfDeath> causeOfDeath;
    int luck = 1;

    // Make wombats older
    for (int i=0;i<wombats.length();i++){
        if (wombats.value(i).birthMonth==worldBuilder->month){
            wombats[i].age+=1;

            if (wombats.value(i).age>11){
                luck = worldBuilder->getRandomNumber(0,3);

            }else if (wombats.value(i).age>15){
                luck = worldBuilder->getRandomNumber(0,2);
            }

            if (luck==0){
                killList.append(i);
                causeOfDeath.append(CauseOfDeath_OldAge);
                luck = 1;
            }
            //console->print(wombats.value(i).name+" became "+QString::number(wombats.value(i).age)+" years old");
        }
    }    

    processBirthList();
    killWombats(killList,causeOfDeath);
}

void Registry::selectedWombatDied(QString wombatName, int wombatAge, CauseOfDeath causeOfDeath){
    worker->setLockMovement(true);
    worker->setSelectedWombatVisibility(false);
    int wombatCount = wombatIds.length();
    QTimer::singleShot(750, this, [=] {
        worker->showDeathNotice(wombatName, wombatAge, wombatCount, worldBuilder->gameOver, causeOfDeath);
    });
}

void Registry::killWombats(QVector<int> killList, QVector<CauseOfDeath> causeOfDeath){
    //worker->consolePrint("killlist start, length: " +QString::number(killList.length()));

    // Kill wombats
    bool corpseAdded = false;
    int wombatId;
    int herdIndex = None;
    QPoint pos;
    QString wombatNameTemp = "";
    int wombatAgeTemp = None;
    CauseOfDeath causeOfDeathTemp = CauseOfDeath_Unknown;

    if (wombats.length()>0){
        wombatNameTemp = wombats.value(0).name;
        wombatAgeTemp = wombats.value(0).age;
    }
    bool selectedWombatIsDead = false;
    for (int i=killList.length()-1;i>-1;i--){ //Need to remove from back of array or indexes are not valid anymore
        int wombatIndex = killList.value(i);
        //index = wombatIds.indexOf(wombatId);
        wombatId = wombats.value(wombatIndex).id;
        pos = wombats.value(wombatIndex).pos;
        //herdIndex = herder->wombatIds.indexOf(wombatId);

        for (int k=0;k<herder->wombatHerd.length();k++){
            if(herder->wombatHerd.value(k)->wombatId == wombatId){
                herdIndex = k;
                break;
            }
        }

        //worker->consolePrint("wombatindex: " +QString::number(wombatIndex));
        //worker->consolePrint("wombatid: " +QString::number(wombatId));
        //worker->consolePrint("herdindex: " +QString::number(herdIndex));

        if (herdIndex!=-1){
            showCorpse(pos,herder->wombatHerd.value(herdIndex)->rotation,herder->wombatHerd.value(herdIndex)->underground);
            corpseAdded = true;            
            herder->removeWombatFromHerd(herdIndex);
        }

        if (wombatId==selectedWombat){
            selectedWombatIsDead = true;
            selectedWombat = None;
            wombatNameTemp = wombats.value(wombatIndex).name;
            wombatAgeTemp = wombats.value(wombatIndex).age;
            causeOfDeathTemp = causeOfDeath.value(i);
        }else{
            worldBuilder->removeWombatIdFromTile(pos,wombatId);
        }
        removeWombatFromRegistry(wombatId);
    }


    if (selectedWombatIsDead){
        if (wombatIds.length()<1){
            worldBuilder->setGameOver();
        }
        selectedWombatDied(wombatNameTemp, wombatAgeTemp, causeOfDeathTemp);

        showCorpse(worldBuilder->worldPosition, 0, worldBuilder->isUnderground); //0 = gui->rotation
        corpseAdded = true;
        worker->playSoundFX(Sound_Death);

    }else{
        if (wombatIds.length()<1){ // BirthList needs to be proccessed before this check
            worldBuilder->setGameOver();
            selectedWombatDied(wombatNameTemp, wombatAgeTemp, causeOfDeathTemp);           
        }
    }

    if(corpseAdded){
        worldBuilder->prepareToUpdateTiles(worldBuilder->worldPosition,worldBuilder->isUnderground); //Set updating tiles to true in gui?
    }

    //worker->consolePrint("killlist end, length: " +QString::number(killList.length()));
}

void Registry::processBirthList(){
    //worker->consolePrint("process birthlist start");
    int index;

    if (worldBuilder->month==3){
        //console->print("It's spring!");
        bool hasMale;        
        for (int i=0;i<burrows.length();i++){
            QVector<int> birthList;
            QVector<int> burrowWombats = burrows.value(i).populationWombatIds;
            hasMale=false;
            for (int j=0;j<burrowWombats.length();j++){
                index = wombatIds.indexOf(burrowWombats[j]);                
                if (wombats.value(index).sex == Male){
                    hasMale=true;                    
                    break;
                }
            }

            if (hasMale==true){
                for (int j=0;j<burrowWombats.length();j++){
                    if (burrows.value(i).food-burrowWombats.length()-birthList.length()>0 ){
                        //console->print("food: "+QString::number(burrows[i].food));
                        //console->print("numberofwombats: "+QString::number(burrowWombats.length()));
                        //console->print("birthlist length: "+QString::number(birthList.length()));
                        //console->print("final number: "+QString::number(burrows[i].food-burrowWombats.length()-birthList.length()));
                        index = wombatIds.indexOf(burrowWombats.value(j));

                        bool nestsLeft = false;
                        if(burrows.value(i).nests.length() > burrows.value(i).populationWombatIds.length()+birthList.length()){
                            nestsLeft = true;
                        }

                        if (nestsLeft && wombats.value(index).sex == Female && wombats.value(index).age>1 && wombats.value(index).plump>50){
                            //int lucky = worldBuilder->getRandomNumber(0,1);
                            //if (lucky==1){
                                birthList.append(index);
                                //worker->consolePrint(wombats.value(index).name +" was lucky!");
                            //}else{
                                //worker->consolePrint(wombats.value(index).name +" was not lucky!");
                            //}
                        }
                    }
                }



                for (int j=0;j<birthList.length();j++){
                    index = birthList.value(j);
                    //int terrainType = 0;

                    QPoint birthPos;
                    //QPoint motherPos = wombats.value(index).pos;
                    //birthPos = motherPos;



                    int burrowId = wombats.value(index).burrowId;

                    //Put the baby on a random nest tile in the burrow
                    int burrowIndex = getBurrowIndex(burrowId);
                    int randomPos = worldBuilder->getRandomNumber(0,burrows.value(burrowIndex).nests.length()-1);
                    birthPos = burrows.value(burrowIndex).nests.value(randomPos);

                    createWombat(birthPos,true,wombats.value(index).burrowId);
                    worldBuilder->addWombatIdToTile(birthPos,wombatIds.last()); //This is a problem. If this pos is in another sector it will fail...
                    //console->print(" born on tile: " +QString::number(birthPos.x())+","+QString::number(birthPos.y())+" id"+QString::number(wombatIds.last()));

                    QPoint p = worldBuilder->worldPosition-birthPos;
                    //QPoint diff2 = hwGameUI->viewPosition-worldPosition;
                    QPoint chunkSize = worldBuilder->world->getChunkSize();

                    if (p.x() <= (chunkSize.x()/2)-1 || p.x() >= -(chunkSize.x()/2) || p.y() <= (chunkSize.x()/2)-1 || p.y() >= -(chunkSize.y()/2)){
                        herder->addWombatToHerd(wombatIds.last(),birthPos,QPoint(0,0),0);
                    }
                    worker->consolePrint(wombats.value(index).name+" gave birth to "+ wombats.last().name +" "+QString::number(birthPos.x())+","+QString::number(birthPos.y()));

                }
            }
        }
    }
    //worker->consolePrint("process birthlist end");
}

void Registry::removeWombatFromRegistry(int wombatId){
    int wombatIndex = wombatIds.indexOf(wombatId);
    int burrowId = wombats.value(wombatIndex).burrowId;
    int burrowIndex = getBurrowIndex(burrowId);
    if (burrowIndex!=None){
        burrows[burrowIndex].populationWombatIds.removeOne(wombatId);
        burrows[burrowIndex].population -= 1;
    }

    wombats.remove(wombatIndex);
    wombatIds.remove(wombatIndex);
}

void Registry::growResources(){
    //All resources grow 100% per year
    for (int i=0;i<burrows.length();i++){
        //Grow ricegrasses
        for (int j=0;j<burrows.value(i).riceGrasses.length();j++){
            double growth = (double)burrows[i].riceGrasses[j].quantity/(double)12.0; // 12 months per year
            burrows[i].riceGrasses[j].quantityAvailable += growth;
            if (burrows[i].riceGrasses[j].quantityAvailable > burrows[i].riceGrasses[j].quantity){ //Check to make sure they do not grow more than 100%
                burrows[i].riceGrasses[j].quantityAvailable = burrows[i].riceGrasses[j].quantity;
            }
        }
        //Grow grasses
        for (int j=0;j<burrows.value(i).grasses.length();j++){
            double growth = (double)burrows[i].grasses[j].quantity/(double)12.0; // 12 months per year
            burrows[i].grasses[j].quantityAvailable += growth;
            if (burrows[i].grasses[j].quantityAvailable > burrows[i].grasses[j].quantity){ //Check to make sure they do not grow more than 100%
                burrows[i].grasses[j].quantityAvailable = burrows[i].grasses[j].quantity;
            }
        }
        //Grow bushes
        for (int j=0;j<burrows.value(i).bushes.length();j++){
            double growth = (double)burrows[i].bushes[j].quantity/(double)12.0; // 12 months per year
            burrows[i].bushes[j].quantityAvailable += growth;
            if (burrows[i].bushes[j].quantityAvailable > burrows[i].bushes[j].quantity){ //Check to make sure they do not grow more than 100%
                burrows[i].bushes[j].quantityAvailable = burrows[i].bushes[j].quantity;
            }
        }
    }
}

void Registry::calculateWorkDone(){
    //For all burrows
    for (int i=0;i<burrows.length();i++){
        //Check how much work is available
        int workSizeAvailable = 0;
        for (int j=0;j<burrows.value(i).populationWombatIds.length();j++){
            if(burrows.value(i).populationWombatIds.value(j) != selectedWombat){
                int wombatIndex = getWombatIndex(burrows.value(i).populationWombatIds.value(j));
                if(wombats.value(wombatIndex).age>1){
                    if(wombats.value(wombatIndex).plump>50){
                        workSizeAvailable += gWombatInventorySize/20;
                    }
                }
            }
        }

        //Check how many resources are available
        int resourceSizeAvailable = 0;
        for (int j=0;j<burrows.value(i).riceGrasses.length();j++){
            int quantityAvailable = burrows.value(i).riceGrasses.value(j).quantityAvailable;
            InventoryItem temp;
            temp.resource = Resource_RiceGrass;
            temp.amount = quantityAvailable;
            resourceSizeAvailable += global->getItemSize(temp);
        }
        for (int j=0;j<burrows.value(i).grasses.length();j++){
            int quantityAvailable = burrows.value(i).grasses.value(j).quantityAvailable;
            InventoryItem temp;
            temp.resource = Resource_Grass;
            temp.amount = quantityAvailable;
            resourceSizeAvailable += global->getItemSize(temp);
        }
        for (int j=0;j<burrows.value(i).bushes.length();j++){
            int quantityAvailable = burrows.value(i).bushes.value(j).quantityAvailable;
            InventoryItem temp;
            temp.resource = Resource_Sticks;
            temp.amount = quantityAvailable;
            resourceSizeAvailable += global->getItemSize(temp);
        }

        //Check how much storage space is available
        int storageSizeAvailable = 0;
        for (int j=0;j<burrows.value(i).storages.length();j++){
            QVector<InventoryItem> items = getStorageInventory(i,j);
            int totalStorageSize = gStorageSize*burrows.value(i).storages.value(j).allTiles.length();
            int sizeUsed = 0;
            for (int k=0;k<items.length();k++){
                sizeUsed += global->getItemSize(items.value(k));
            }
            storageSizeAvailable += totalStorageSize-sizeUsed;
        }

        //Now calculate how much work can be done
        int totalWorkThatCanBeDoneThisMonth = 0;

        if(storageSizeAvailable!=0 && resourceSizeAvailable!=0 && workSizeAvailable!=0){
            if(storageSizeAvailable<=resourceSizeAvailable && storageSizeAvailable<=workSizeAvailable){
                totalWorkThatCanBeDoneThisMonth = storageSizeAvailable;     //Storage size is the limiting factor
            }else if(resourceSizeAvailable<=workSizeAvailable){
                totalWorkThatCanBeDoneThisMonth = resourceSizeAvailable;    //Resources are the limiting factor
            }else{
                totalWorkThatCanBeDoneThisMonth = workSizeAvailable;        //The number of wombats are the limiting factor
            }
        }

        if(totalWorkThatCanBeDoneThisMonth>0){
            //Add work to buffer
            burrows[i].workBuffer += totalWorkThatCanBeDoneThisMonth;
            //worker->consolePrint("Added "+QString::number(totalWorkThatCanBeDoneThisMonth)+" to the workbuffer");
            //worker->consolePrint("Buffer total is: "+QString::number(burrows[i].workBuffer));
            if(burrows[i].workBuffer > totalWorkThatCanBeDoneThisMonth*20){
                workBufferOverflow(i,burrows[i].workBuffer);
                burrows[i].workBuffer = 0;
            }
        }
    }
}

void Registry::workBufferOverflow(int burrowIndex, int overflow){
    worker->consolePrint("WorkBuffer overflow: "+QString::number(overflow));
    int overflowLeft = overflow;

    //While we have overflow left, go through all the storages and check if there is storage space left
    for (int j=0;j<burrows.value(burrowIndex).storages.length() && overflowLeft>0;j++){
        QVector<InventoryItem> items = getStorageInventory(burrowIndex,j);
        int totalStorageSize = gStorageSize*burrows.value(burrowIndex).storages.value(j).allTiles.length();
        int sizeUsed = 0;
        for (int k=0;k<items.length();k++){
            sizeUsed += global->getItemSize(items.value(k));
        }
        int storageSizeLeftToUse = totalStorageSize-sizeUsed;

        if(storageSizeLeftToUse>0){
            //We found empty storage space

            if(storageSizeLeftToUse>overflowLeft){
                storageSizeLeftToUse = overflowLeft;
            }

            //Check which resources are accepted
            bool riceGrassAccepted = burrows.value(burrowIndex).storages.value(j).acceptedResources.value(2);
            bool grassAccepted = burrows.value(burrowIndex).storages.value(j).acceptedResources.value(1);
            bool sticksAccepted = burrows.value(burrowIndex).storages.value(j).acceptedResources.value(0);


            //Try to fill storage with ricegrass
            int riceGrassSize = global->getResourceSize(Resource_RiceGrass);
            int riceGrassAmount = storageSizeLeftToUse/riceGrassSize;
            if(riceGrassAmount>0 && riceGrassAccepted){
                InventoryItem riceGrass = collectRiceGrassForWorkBufferOverflow(burrowIndex, riceGrassAmount);
                addItemToStorage(burrowIndex,j,riceGrass);
                int sizeAdded = global->getItemSize(riceGrass);
                storageSizeLeftToUse -= sizeAdded;
                overflowLeft -= sizeAdded;
            }

            //Try to fill storage with grass
            int grassSize = global->getResourceSize(Resource_Grass);
            int grassAmount = storageSizeLeftToUse/grassSize;
            if(grassAmount>0 && grassAccepted){
                InventoryItem grass = collectGrassForWorkBufferOverflow(burrowIndex, grassAmount);
                addItemToStorage(burrowIndex,j,grass);
                int sizeAdded = global->getItemSize(grass);
                storageSizeLeftToUse -= sizeAdded;
                overflowLeft -= sizeAdded;
            }

            //Try to fill storage with sticks
            int sticksSize = global->getResourceSize(Resource_Sticks);
            int sticksAmount = storageSizeLeftToUse/sticksSize;
            if(sticksAmount>0 && sticksAccepted){
                InventoryItem sticks = collectSticksForWorkBufferOverflow(burrowIndex, sticksAmount);
                addItemToStorage(burrowIndex,j,sticks);
                int sizeAdded = global->getItemSize(sticks);
                storageSizeLeftToUse -= sizeAdded;
                overflowLeft -= sizeAdded;
            }


            if(riceGrassAmount==0 && grassAmount==0 && sticksAmount==0){
                break; //If we have no more resources stop trying to fill storages
            }
        }
    }
}

InventoryItem Registry::collectRiceGrassForWorkBufferOverflow(int burrowIndex, int amount){
    InventoryItem collectedRiceGrass;
    collectedRiceGrass.resource = Resource_RiceGrass;
    collectedRiceGrass.amount = 0;

    int amountLeft = amount;
    int riceGrassCount = burrows.value(burrowIndex).riceGrasses.length();
    int randomRiceGrassesTried = 0;
    bool done = false;

    //Collect riceGrass
    while (!done){
        if(randomRiceGrassesTried<riceGrassCount){
            randomRiceGrassesTried++;
            //Choose random ricegrass
            int randomRiceGrassIndex = worldBuilder->getRandomNumber(0,riceGrassCount-1);
            if(burrows.value(burrowIndex).riceGrasses.value(randomRiceGrassIndex).quantityAvailable > amountLeft){
                burrows[burrowIndex].riceGrasses[randomRiceGrassIndex].quantityAvailable -= amountLeft;
                collectedRiceGrass.amount += amountLeft;
                amountLeft = 0;
                done = true;
            }else{
                int quantityAvailable = burrows.value(burrowIndex).riceGrasses.value(randomRiceGrassIndex).quantityAvailable;
                burrows[burrowIndex].riceGrasses[randomRiceGrassIndex].quantityAvailable -= quantityAvailable; // double quantityAvailable -= int quantityAvailable
                collectedRiceGrass.amount += quantityAvailable;
                amountLeft -= quantityAvailable;
            }
        }else{
            //If we have tried enough random ricegrasses and didn't find enough, then loop through all ricegrasses
            for (int j=0;j<burrows.value(burrowIndex).riceGrasses.length();j++){
                if(burrows.value(burrowIndex).riceGrasses.value(j).quantityAvailable > amountLeft){
                    burrows[burrowIndex].riceGrasses[j].quantityAvailable -= amountLeft;
                    collectedRiceGrass.amount += amountLeft;
                    amountLeft = 0;
                    done = true;
                    break;
                }else{
                    int quantityAvailable = burrows.value(burrowIndex).riceGrasses.value(j).quantityAvailable;
                    burrows[burrowIndex].riceGrasses[j].quantityAvailable -= quantityAvailable;
                    collectedRiceGrass.amount += quantityAvailable;
                    amountLeft -= quantityAvailable;
                }
            }
            done = true;
        }
    }

    return collectedRiceGrass;
}

InventoryItem Registry::collectGrassForWorkBufferOverflow(int burrowIndex, int amount){
    InventoryItem collectedGrass;
    collectedGrass.resource = Resource_Grass;
    collectedGrass.amount = 0;

    int amountLeft = amount;
    int grassCount = burrows.value(burrowIndex).grasses.length();
    int randomGrassesTried = 0;
    bool done = false;

    //Collect grass
    while (!done){
        if(randomGrassesTried<grassCount){
            randomGrassesTried++;
            int randomGrassIndex = worldBuilder->getRandomNumber(0,grassCount-1);
            if(burrows.value(burrowIndex).grasses.value(randomGrassIndex).quantityAvailable > amountLeft){
                burrows[burrowIndex].grasses[randomGrassIndex].quantityAvailable -= amountLeft;
                collectedGrass.amount += amountLeft;
                amountLeft = 0;
                done = true;
            }else{
                int quantityAvailable = burrows.value(burrowIndex).grasses.value(randomGrassIndex).quantityAvailable;
                burrows[burrowIndex].grasses[randomGrassIndex].quantityAvailable -= quantityAvailable; // double quantityAvailable -= int quantityAvailable
                collectedGrass.amount += quantityAvailable;
                amountLeft -= quantityAvailable;
            }
        }else{
            for (int j=0;j<burrows.value(burrowIndex).grasses.length();j++){
                if(burrows.value(burrowIndex).grasses.value(j).quantityAvailable > amountLeft){
                    burrows[burrowIndex].grasses[j].quantityAvailable -= amountLeft;
                    collectedGrass.amount += amountLeft;
                    amountLeft = 0;
                    done = true;
                    break;
                }else{
                    int quantityAvailable = burrows.value(burrowIndex).grasses.value(j).quantityAvailable;
                    burrows[burrowIndex].grasses[j].quantityAvailable -= quantityAvailable;
                    collectedGrass.amount += quantityAvailable;
                    amountLeft -= quantityAvailable;
                }
            }
            done = true;
        }
    }

    return collectedGrass;
}

InventoryItem Registry::collectSticksForWorkBufferOverflow(int burrowIndex, int amount){
    InventoryItem collectedSticks;
    collectedSticks.resource = Resource_Sticks;
    collectedSticks.amount = 0;

    int amountLeft = amount;
    int sticksCount = burrows.value(burrowIndex).bushes.length();
    int randomBushesTried = 0;
    bool done = false;

    //Collect sticks
    while (!done){

        if(randomBushesTried<sticksCount){
            randomBushesTried++;
            int randomSticksIndex = worldBuilder->getRandomNumber(0,sticksCount-1);
            if(burrows.value(burrowIndex).bushes.value(randomSticksIndex).quantityAvailable > amountLeft){
                burrows[burrowIndex].bushes[randomSticksIndex].quantityAvailable -= amountLeft;
                collectedSticks.amount += amountLeft;
                amountLeft = 0;
                done = true;
            }else{
                int quantityAvailable = burrows.value(burrowIndex).bushes.value(randomSticksIndex).quantityAvailable;
                burrows[burrowIndex].bushes[randomSticksIndex].quantityAvailable -= quantityAvailable; // double quantityAvailable -= int quantityAvailable
                collectedSticks.amount += quantityAvailable;
                amountLeft -= quantityAvailable;
            }
        }else{
            for (int j=0;j<burrows.value(burrowIndex).bushes.length();j++){
                if(burrows.value(burrowIndex).bushes.value(j).quantityAvailable > amountLeft){
                    burrows[burrowIndex].bushes[j].quantityAvailable -= amountLeft;
                    collectedSticks.amount += amountLeft;
                    amountLeft = 0;
                    done = true;
                    break;
                }else{
                    int quantityAvailable = burrows.value(burrowIndex).bushes.value(j).quantityAvailable;
                    burrows[burrowIndex].bushes[j].quantityAvailable -= quantityAvailable;
                    collectedSticks.amount += quantityAvailable;
                    amountLeft -= quantityAvailable;
                }
            }
            done = true;
        }
    }

    return collectedSticks;
}

InventoryItem Registry::addItemToStorage(int burrowIndex, int storageIndex, InventoryItem item){
    bool storageDisplayUpdated = false;

    if(item.amount!=0){
        //Check if this resources is accepted
        bool itemIsAccepted = false;
        if (burrows.value(burrowIndex).storages.value(storageIndex).acceptedResources.length()==global->gResources.length()){
            for (int i=0;i<global->gResources.length();i++){
                if(global->gResources.value(i).resource == item.resource){
                    if(burrows.value(burrowIndex).storages.value(storageIndex).acceptedResources.value(i) == true){
                        itemIsAccepted = true;
                    }
                    break;
                }
            }
        }


        if(itemIsAccepted){
            int resourceSize = global->getResourceSize(item.resource);

            for (int j=0;j<burrows[burrowIndex].storages[storageIndex].allTiles.length();j++){
               int sizeLeft = gStorageSize;
               for (int k=0;k<burrows[burrowIndex].storages[storageIndex].allTiles[j].inventory.length();k++){
                   int size = global->getItemSize(burrows[burrowIndex].storages[storageIndex].allTiles[j].inventory.value(k));
                   sizeLeft -= size;
               }

               if(sizeLeft>0){
                   int amountThatCanBeAdded = sizeLeft / resourceSize;
                   int amountToAdd;
                   if (amountThatCanBeAdded>item.amount){
                       amountToAdd = item.amount;
                   }else{
                       amountToAdd = amountThatCanBeAdded;
                   }

                   bool itemFound = false;
                   for (int k=0;k<burrows[burrowIndex].storages[storageIndex].allTiles[j].inventory.length();k++){
                       if(item.resource == burrows[burrowIndex].storages[storageIndex].allTiles[j].inventory[k].resource){ //Check if tile already has resource
                           burrows[burrowIndex].storages[storageIndex].allTiles[j].inventory[k].amount += amountToAdd; //Then just add the amount
                           item.amount -= amountToAdd;
                           itemFound=true;

                           if (burrows[burrowIndex].storages[storageIndex].allTiles[j].visible == true){ // If visible, update graphics
                               herder->updateStorageDisplay(burrowIndex, storageIndex, j);
                               storageDisplayUpdated = true;
                           }

                           break;
                       }
                   }

                   if(!itemFound){ //If tile does not have resource, create it and add it
                       InventoryItem item2;
                       item2.resource = item.resource;
                       item2.amount = amountToAdd;
                       burrows[burrowIndex].storages[storageIndex].allTiles[j].inventory.append(item2);
                       item.amount -= amountToAdd;

                       if (burrows[burrowIndex].storages[storageIndex].allTiles[j].visible == true){  // If visible, update graphics
                           herder->updateStorageDisplay(burrowIndex, storageIndex, j);
                           storageDisplayUpdated = true;
                       }
                   }

                   if (item.amount<=0){
                       break; // Nothing more to add
                   }
               }
            }


            if(guiStorageWidgetOpen){
               if (guiStorageWidgetStorageId == burrows[burrowIndex].storages[storageIndex].id){
                   worker->getStorageInventory(burrows.value(burrowIndex).id,guiStorageWidgetStorageId);
               }
            }
        }
    }


    worldBuilder->prepareToUpdateTiles(worldBuilder->worldPosition,worldBuilder->isUnderground); //Is this smart!?!? every single fucking time you modify a storage tile you update the whole fucking thing? this can't be smart...

    if(storageDisplayUpdated){
        worldBuilder->prepareToUpdateTiles(worldBuilder->worldPosition,worldBuilder->isUnderground);
    }

    return item;
}

void Registry::removeItemFromStorageOverload(int burrowId, int storageId, InventoryItem item){
    int burrowIndex = getBurrowIndex(burrowId);
    int storageIndex = getStorageIndex(burrowIndex, storageId);
    removeItemFromStorage(burrowIndex, storageIndex, item);
}

void Registry::removeItemFromStorage(int burrowIndex, int storageIndex, InventoryItem item){
    bool storageDisplayUpdated = false;
    QVector<int> tileIndexes; // Tiles with inventoryItem to be removed
    QVector<int> inventoryIndexes; //InventoryItem to be removed

    bool done = false;

    for (int j=0;j<burrows[burrowIndex].storages[storageIndex].allTiles.length() && !done;j++){
       for (int k=0;k<burrows[burrowIndex].storages[storageIndex].allTiles[j].inventory.length() && !done;k++){
            if(item.resource == burrows[burrowIndex].storages[storageIndex].allTiles[j].inventory[k].resource){

                if(burrows[burrowIndex].storages[storageIndex].allTiles[j].inventory[k].amount>item.amount){
                    burrows[burrowIndex].storages[storageIndex].allTiles[j].inventory[k].amount -= item.amount;
                    item.amount = 0;
                    done = true;
                }else{
                    item.amount = item.amount - burrows[burrowIndex].storages[storageIndex].allTiles[j].inventory[k].amount;
                    burrows[burrowIndex].storages[storageIndex].allTiles[j].inventory[k].amount = 0;
                    tileIndexes.append(j);
                    inventoryIndexes.append(k);
                }

                if (burrows[burrowIndex].storages[storageIndex].allTiles[j].visible == true){ // If visible, update graphics
                   herder->updateStorageDisplay(burrowIndex, storageIndex, j);
                   storageDisplayUpdated = true;
                }
            }
        }
    }

    for (int i=inventoryIndexes.length()-1;i>=0;i--){
        burrows[burrowIndex].storages[storageIndex].allTiles[tileIndexes.value(i)].inventory.remove(inventoryIndexes.value(i));
    }

    if(guiStorageWidgetOpen){
       if (guiStorageWidgetStorageId == burrows[burrowIndex].storages[storageIndex].id){
           worker->getStorageInventory(burrows.value(burrowIndex).id,guiStorageWidgetStorageId);
       }
    }

    if(storageDisplayUpdated){
        worldBuilder->prepareToUpdateTiles(worldBuilder->worldPosition,worldBuilder->isUnderground);
    }
}

void Registry::addTunnelsToBurrow(int burrowId, QVector<QPoint> startPos){
    //worker->consolePrint("add tunnels to burrow, start pos length:" +QString::number(startPos.length()));
    int burrowIndex = getBurrowIndex(burrowId);
    int length = startPos.length();

    while(length>0){
        QPoint start = startPos.takeFirst();
        QVector<QVector<QPoint>> tiles = exploreTunnel(start);

        QVector<int> toBeRemoved;

        //Check if any of the startPos are included in the tunnel we just explored
        for (int i=0;i<startPos.length();i++){
            for (int j=0;j<tiles.value(1).length();j++){
                if(tiles.value(1).value(j) == startPos.value(i)){
                    toBeRemoved.append(i);
                    break;
                }
            }

        }

        for (int i=0;i<toBeRemoved.length();i++){
            startPos.remove(toBeRemoved.value(i));
        }

        length = startPos.length();

        if(burrowIndex>=0 && burrowIndex<burrows.length()){
            burrows[burrowIndex].entrancePos += tiles.value(0); // Add the new tunnel's entrances to the burrow
            burrows[burrowIndex].entrances = burrows[burrowIndex].entrancePos.length();
            burrows[burrowIndex].allNonEntrancePos += tiles.value(0)+tiles.value(1); // Add all tiles in the new tunnel to the burrow
            burrows[burrowIndex].size = burrows[burrowIndex].allNonEntrancePos.length();

            //Add the burrow id to all new tiles
            for (int i=0;i<tiles.value(0).length();i++){
                worldBuilder->changeTerrain(tiles.value(0).value(i),Terrain_Burrow,burrowId,true);

            }
            for (int i=0;i<tiles.value(1).length();i++){
                worldBuilder->changeTerrain(tiles.value(1).value(i),Terrain_Burrow,burrowId,true);
            }
        }
    }
}

void Registry::addPosToBurrow(QPoint pos, QPoint newPos){
    int burrowId = worldBuilder->getBurrowId(pos);
    int burrowIndex = getBurrowIndex(burrowId);
    if (burrowIndex!=None && burrowIndex<burrows.length()){
        burrows[burrowIndex].allNonEntrancePos.append(newPos);
        burrows[burrowIndex].size = burrows[burrowIndex].allNonEntrancePos.length();
    }else{
        worker->consolePrint("Error addPosToBurrow");
    }
}

void Registry::addEntrancePosToBurrow(int burrowId, QPoint pos){
    int burrowIndex = getBurrowIndex(burrowId);

    if (burrowIndex >=0 && burrowIndex<burrows.length()){
        burrows[burrowIndex].entrancePos.append(pos);
        burrows[burrowIndex].entrances = burrows[burrowIndex].entrancePos.length();
        int index = burrows[burrowIndex].allNonEntrancePos.indexOf(pos);
        if(index == None){
            burrows[burrowIndex].allNonEntrancePos.append(pos);
            burrows[burrowIndex].size = burrows[burrowIndex].allNonEntrancePos.length();
        }
    }else{
        worker->consolePrint("Error addEntrancePosToBurrow");
    }
}

void Registry::changeWombatPos(int wombatId, QPoint newTilePos){
    int wombatIndex = getWombatIndex(wombatId);
    if(wombatIndex>=0 && wombatIndex<wombats.length()){
        QPoint oldTilePos = wombats.value(wombatIndex).pos;
        wombats[wombatIndex].pos = newTilePos;
        worldBuilder->removeWombatIdFromTile(oldTilePos,wombatId);
        worldBuilder->addWombatIdToTile(newTilePos,wombatId);
    }
}

void Registry::setPosOfDepossessedWombat(int wombatId, QPoint newTilePos){
    int wombatIndex = getWombatIndex(wombatId);
    if(wombatIndex>=0 && wombatIndex<wombats.length()){
        wombats[wombatIndex].pos = newTilePos;
        worldBuilder->addWombatIdToTile(newTilePos,wombatId);
    }
}

InventoryItem Registry::addItemToWombatInventory(int wombatId, Resource resource, int amount){
    InventoryItem returnItem;
    returnItem.resource = resource;
    returnItem.amount = 0;

    if(amount!=0){
        if(false){
        //if(hwGameUI->wInventoryWidgetOpen && wombatId == hwGameUI->wInventoryWidget->wombatId && hwGameUI->wInventoryWidget->movingLabel){
            //If we are dragging resources block adding more resources to inventory or indexing will be wrong
            //returnItem.resource = resource;
            //returnItem.amount = amount;
            //return returnItem;

        }else{
            int wombatIndex = getWombatIndex(wombatId);
            int amountToAdd = amount;

            if(amount>0){ //If we are adding stuff, then make sure there is enough room
                int sizeLeft = gWombatInventorySize;

                for (int i=0;i<wombats.value(wombatIndex).inventory.length();i++){
                   const InventoryItem &item = wombats.value(wombatIndex).inventory.value(i);
                   int itemSize = global->getItemSize(item);
                   sizeLeft -= itemSize;
                }

                int resourceSize = global->getResourceSize(resource);

                if(sizeLeft>0){
                    int amountThatCanBeAdded = sizeLeft / resourceSize;
                    //console->print("amountThatCanBeAdded "+QString::number(amountThatCanBeAdded));
                    if (amountThatCanBeAdded>amount){
                        amountToAdd = amount;
                    }else{
                        amountToAdd = amountThatCanBeAdded;
                    }
                    //console->print("amountToAdd "+QString::number(amountToAdd));
                }else{
                    amountToAdd = 0;
                }
            }


            bool alreadyHasResource = false;
            int toBeRemoved = -1;

            for (int i=0;i<wombats.value(wombatIndex).inventory.length();i++){
                if (wombats.value(wombatIndex).inventory.value(i).resource == resource){
                    alreadyHasResource = true;
                    wombats[wombatIndex].inventory[i].amount += amountToAdd;

                    if(wombats[wombatIndex].inventory[i].amount<=0){
                        toBeRemoved = i;
                    }
                }
            }

            if (!alreadyHasResource){                
                InventoryItem item;
                item.resource = resource;
                item.amount = amountToAdd;
                wombats[wombatIndex].inventory.append(item);
            }else if (toBeRemoved!=-1){
                wombats[wombatIndex].inventory.remove(toBeRemoved);
            }

            if(guiWInventoryWidgetOpen){  //Emit signal everytime? ... but only if selected wombat? I don't know...
                if(wombatId == guiWInventoryWidgetWombatId){
                    worker->getWombatInventory(wombatId); //Move this function from worker here to registry
                }
            }

            returnItem.resource = resource;
            returnItem.amount = amount-amountToAdd;
        }

    }

    return returnItem;

}

InventoryItem Registry::getItemFromWombatInventory(int wombatId, InventoryItem item){
    int wombatIndex = getWombatIndex(wombatId);

    for (int i=0;i<wombats.value(wombatIndex).inventory.length();i++){
        if (wombats.value(wombatIndex).inventory.value(i).resource == item.resource){
            item.amount = wombats.value(wombatIndex).inventory.value(i).amount;
            break;
        }
    }
    addItemToWombatInventory(wombatId,item.resource,-item.amount);
    return item;
}

void Registry::addBuilding(QPoint pos, Overlay building, int variation){

    if (building == Storage || building == Nest){
        QVector<InventoryItem> &wombatInventory = getWombatInventory(selectedWombat);
        QVector<InventoryItem> itemsNeeded = global->getBuildCost(building);


        Resource resourceTemp;
        bool allItemsFound = true;
        if(itemsNeeded.value(0).resource != Resource_None){
            // Check if all itemsNeeded is covered
            for (int i=0;i<itemsNeeded.length();i++){
                resourceTemp = itemsNeeded.value(i).resource;
                bool itemFound = false;
                for (int j=0;j<wombatInventory.length();j++){
                    if (itemsNeeded.value(i).resource == wombatInventory.value(j).resource){
                        if (itemsNeeded.value(i).amount <= wombatInventory.value(j).amount){
                            itemFound = true;
                            break;
                        }
                    }
                }
                if(!itemFound){
                    worker->consolePrint("Not enough "+global->getResourceName(resourceTemp));
                    allItemsFound=false;
                    break;
                }
            }
        }
        if(allItemsFound){
            //Remove the items used for building
            for (int i=0;i<itemsNeeded.length();i++){
                for (int j=0;j<wombatInventory.length();j++){
                    if (itemsNeeded.value(i).resource == wombatInventory.value(j).resource){
                        addItemToWombatInventory(selectedWombat,itemsNeeded.value(i).resource,-itemsNeeded.value(i).amount);
                        //wombatInventory[j].amount -= itemsNeeded.value(i).amount;
                        break;
                    }
                }
            }

            int burrowId = worldBuilder->getBurrowId(pos);
            worldBuilder->addOverlayToTileUnderground(pos,building,variation);

            if(building == Storage){
                addStorage(pos, burrowId);
                worker->consolePrint("storage added");
            }else if (building == Nest){
                addNest(pos, burrowId);
                worker->consolePrint("nest added");
            }


        }
    }
}

void Registry::addNest(QPoint pos, int burrowId){
    int burrowIndex = getBurrowIndex(burrowId);
    if (burrowIndex>=0 && burrowIndex <burrows.length()){
        burrows[burrowIndex].nests.append(pos);
    }
}


void Registry::addStorage(QPoint pos, int burrowId){
    int burrowIndex = getBurrowIndex(burrowId);
    if (burrowIndex!=-1){ //Make sure the index is valid

        QVector<int> storageIndex;

        // Check if the new storage pos is bordering any existing storages
        for (int i=0;i<burrows.value(burrowIndex).storages.length();i++){
            for (int j=0;j<burrows.value(burrowIndex).storages.value(i).allTiles.length();j++){
                QPoint p = pos-burrows.value(burrowIndex).storages.value(i).allTiles.value(j).pos;
                if (p.manhattanLength()==1){
                    if (storageIndex.indexOf(i)==-1){
                        storageIndex.append(i);
                    }
                }
            }
        }

        //If there are bordering storages
        if(storageIndex.length()>0){
            //Add new tile to bordering storage
            StorageTileStruct tile;
            tile.pos = pos;
            tile.visible = true;
            burrows[burrowIndex].storages[storageIndex[0]].allTiles.append(tile);

            //If more than 2 bordering storages, merge them
            if (storageIndex.length()>1){
                int numberOfStorages = storageIndex.length();
                //For all granaries being merged
                for (int i=1;i<numberOfStorages;i++){
                    //Merge all tiles into storages[storageIndex[0]]
                    for (int j=0;j<burrows[burrowIndex].storages[storageIndex[i]].allTiles.length();j++){
                         burrows[burrowIndex].storages[storageIndex[0]].allTiles.append(burrows[burrowIndex].storages[storageIndex[i]].allTiles.value(j));
                    }
                    /*
                    //Merge inventory into granary[granaryIndex[0]]
                    for (int j=0;j<burrows[burrowIndex].granaries[granaryIndex[i]].inventory.length();j++){
                        Resource resource = burrows[burrowIndex].granaries[granaryIndex[i]].inventory[j].resource;
                        bool itemFound = false;

                            for (int k=0;k<burrows[burrowIndex].granaries[granaryIndex[0]].inventory.length();k++){ // Check if item already exists
                                if (resource == burrows[burrowIndex].granaries[granaryIndex[0]].inventory[k].resource){
                                    // Item already exists, just add the amount.
                                    burrows[burrowIndex].granaries[granaryIndex[0]].inventory[k].amount +=  burrows[burrowIndex].granaries[granaryIndex[i]].inventory[j].amount;
                                    itemFound = true;
                                    break;
                                }
                            }

                            //If item does not exist. Add it
                            if (!itemFound){
                                InventoryItem item;
                                item.resource = resource;
                                item.amount = burrows[burrowIndex].granaries[granaryIndex[i]].inventory[j].amount;
                                burrows[burrowIndex].granaries[granaryIndex[0]].inventory.append(item);
                            }
                    }*/
                }

                //Remove merged granaries
                for (int i=numberOfStorages-1;i>0;i--){ //Remove from back or indexes won't be valid anymore
                    burrows[burrowIndex].storages.remove(storageIndex[i]);
                }
            }

            //console->print("number of bordering granaries: "+QString::number(granaryIndex.length()));

        }else{
            //If no bordering storages
            //Create new storage
            StorageStruct storage;
            storage.id = burrows.value(burrowIndex).storages.length()+1;            
            StorageTileStruct tile;
            tile.pos = pos;
            tile.visible = true;
            QVector<StorageTileStruct> allTiles;
            allTiles.append(tile);
            storage.allTiles = allTiles;

            QVector<bool> acceptedItems;
            for (int i=0;i<global->gResources.length();i++){
                acceptedItems.append(true);
            }
            storage.acceptedResources = acceptedItems;

            burrows[burrowIndex].storages.append(storage);
        }


    }else{
        //console->print("Error addStorage");
    }
}

int Registry::getStorageId(QPoint pos, int burrowId){
    int burrowIndex = getBurrowIndex(burrowId);

    for (int i=0;i<burrows.value(burrowIndex).storages.length();i++){
        for (int j=0;j<burrows.value(burrowIndex).storages.value(i).allTiles.length();j++){
            if (burrows.value(burrowIndex).storages.value(i).allTiles.value(j).pos == pos){
                return burrows.value(burrowIndex).storages.value(i).id;
            }
        }
    }

    return None;
}


int Registry::getStorageIndex(int burrowIndex, int storageId){
    for (int i=0;i<burrows.value(burrowIndex).storages.length();i++){
        if (burrows.value(burrowIndex).storages.value(i).id==storageId){
            return i;
        }
    }

    return -1;
}

QVector<InventoryItem> Registry::getStorageInventory(int burrowIndex, int storageIndex){
    //int burrowIndex = getBurrowIndex(burrowId);
    //int storageIndex = getStorageIndex(burrowIndex,storageId);
    QVector<InventoryItem> inventory;

    for (int i=0;i<burrows.value(burrowIndex).storages.value(storageIndex).allTiles.length();i++){ // Loop through all tiles that belong to this storage
        for (int j=0;j<burrows.value(burrowIndex).storages.value(storageIndex).allTiles.value(i).inventory.length();j++){ // Loop through the inventory for each tile
            bool alreadyAdded = false;
            Resource storageResource = burrows.value(burrowIndex).storages.value(storageIndex).allTiles.value(i).inventory.value(j).resource;

            for (int k=0;k<inventory.length();k++){
                 if (inventory.value(k).resource == storageResource){
                    alreadyAdded = true;
                    inventory[k].amount += burrows.value(burrowIndex).storages.value(storageIndex).allTiles.value(i).inventory.value(j).amount;
                    break;
                }
            }

            if(!alreadyAdded){
                InventoryItem item;
                item.resource = storageResource;
                item.amount = burrows.value(burrowIndex).storages.value(storageIndex).allTiles.value(i).inventory.value(j).amount;
                inventory.append(item);
            }
        }
    }

    return inventory;
}

QVector<InventoryItem> Registry::getWombatInventory(int wombatId){
    int wombatIndex = getWombatIndex(wombatId);
    QVector<InventoryItem> inventory = wombats.value(wombatIndex).inventory;
    return inventory;
}

void Registry::prepareWombatInfoForDisplay(int wombatId){
    wombatIdGuiInfoBox = wombatId;

    QString info1 = "";
    QString info2 = "";
    QString info3 = "";
    QString info4 = "";
    QString info5 = "";

    bool setPossessButtonVisible = false;

    //Display wombat info
    int index = wombatIds.indexOf(wombatId);

    //console->print("index " +QString::number(index));

    if (wombats.value(index).sex == 0){
        info1 = "Wombat ♀";
    }else{
        info1 = "Wombat ♂";
    }
    info2 = wombats.value(index).name;

    int burrowId = wombats.value(index).burrowId;
    if (burrowId == None){
        info3 = "Wanderer";
    }else{
        info3 = "Burrow: "+burrows.value(burrowId-1).name;
    }

    int age = wombats.value(index).age;

    QString s = "";
    if (age==1){
        s = " year old";
    }else{
        s = " years old";
    }
    //Add +s again!
    info4 = "Age: " + QString::number(age)+"   h: "+QString::number(wombats.value(index).health);
    info4 += "   p: "+QString::number(wombats.value(index).plump);
    info4 += "   r: "+QString::number(wombats.value(index).rested);

    if (age>1 && wombatId!=selectedWombat){
        setPossessButtonVisible = true;
    }
/*
    QVector<InventoryItem> &inventory = wombats.value(index).inventory;

    if (inventory.length()==0){
        s="";

    }else{
        s="Carrying";
        for (int j=0;j<inventory.length();j++){
            s+= " "+QString::number(inventory.value(j).amount)+" "+ global->gGetResourceName(inventory.value(j).resource)+",";
        }
    }
*/
    int herdIndex = herder->wombatIds.indexOf(wombatId);
    if(herdIndex!=None){
        if(!herder->wombatHerd.value(herdIndex)->movingInOpenGl){
            s = herder->wombatHerd.value(herdIndex)->taskString;
        }else{
            s = herder->wombatHerd.value(herdIndex)->oldTaskString;
        }
        info5 = s;
    }

    QVector<QString> strings;
    strings.append(info1);
    strings.append(info2);
    strings.append(info3);
    strings.append(info4);
    strings.append(info5);

    worker->displayWombatInfo(strings,wombatId,setPossessButtonVisible);
}

void Registry::prepareTileInfoForDisplay(QPoint pos){
    wombatIdGuiInfoBox = None;

    QString info1 = "";
    QString info2 = "";
    QString info3 = "";
    QString info4 = "";
    QString info5 = "";
    //Display terraintype

    if(worldBuilder->hasOverlay(pos,Overlay_Storage,worldBuilder->isUnderground)){
        if(worldBuilder->isUnderground && worldBuilder->hasOverlay(pos,Overlay_HoleLight,true)){
            info1 = "Exit";
        }else{
            info1 = "Burrow";
        }
        int burrowId = worldBuilder->getBurrowId(pos);
        int burrowIndex = getBurrowIndex(burrowId);
        QString name = burrows.value(burrowIndex).name;
        info2 = name;
        info3 = "Storage";
    }else if(worldBuilder->hasOverlay(pos,Overlay_Nest,worldBuilder->isUnderground)){
        if(worldBuilder->isUnderground && worldBuilder->hasOverlay(pos,Overlay_HoleLight,true)){
            info1 = "Exit";
        }else{
            info1 = "Burrow";
        }
        int burrowId = worldBuilder->getBurrowId(pos);
        int burrowIndex = getBurrowIndex(burrowId);
        QString name = burrows.value(burrowIndex).name;
        info2 = name;
        info3 = "Nest";
    }else{

        bool hasHole = worldBuilder->hasTerrain(Terrain_Hole,false,pos);
        bool hasBurrow = worldBuilder->hasTerrain(Terrain_Burrow,true,pos);

        bool isUnderground = worldBuilder->isUnderground;
        Terrain terrain = worldBuilder->getTerrain(pos,isUnderground);
        info1 = global->getName(terrain);
        if(isUnderground && hasHole){
            info1 = "Exit";
        }

        if ((!isUnderground && hasHole && hasBurrow) || (isUnderground && hasBurrow)){
            if(!isUnderground && hasHole){
                info1 = "Burrow";
            }

            //Display burrow name
            int burrowId = worldBuilder->getBurrowId(pos);
            int burrowIndex = getBurrowIndex(burrowId);
            QString name = burrows.value(burrowIndex).name;
            info2 = name;
        }else if(worldBuilder->hasOverlay(pos,Overlay_Bush,worldBuilder->isUnderground)){
            //Display owner
            info1 = global->getName(Overlay_Bush);
            int burrowId = worldBuilder->getOwner(pos.x(),pos.y());            
            if (burrowId!=None){
                int burrowIndex = getBurrowIndex(burrowId);
                QString name = burrows.value(burrowIndex).name;
                info2 = "Claimed by "+name;
                for (int i=0;i<burrows.value(burrowIndex).bushes.length();i++){
                    if(pos == burrows.value(burrowIndex).bushes.value(i).pos){
                        int total = burrows.value(burrowIndex).bushes.value(i).quantity;
                        int available = (int)burrows.value(burrowIndex).bushes.value(i).quantityAvailable;
                        info3 = "Quantity: "+QString::number(available);
                        double growth =  ((double)available/(double)total)*100.0;
                        info4 = "Growth: "+QString::number(growth,'f',0)+"%";
                        break;
                    }
                }
            }else{
                int variation = worldBuilder->getOverlayVariation(pos,Overlay_Bush,worldBuilder->isUnderground);
                info3 = "Quantity: "+QString::number(variation);
            }
        }else if(worldBuilder->hasOverlay(pos,Overlay_RiceGrass,worldBuilder->isUnderground)){
            //Display owner
            info1 = global->getName(Overlay_RiceGrass);
            int burrowId = worldBuilder->getOwner(pos.x(),pos.y());
            if (burrowId!=None){
                int burrowIndex = getBurrowIndex(burrowId);
                QString name = burrows.value(burrowIndex).name;
                info2 = "Claimed by "+name;
                for (int i=0;i<burrows.value(burrowIndex).riceGrasses.length();i++){
                    if(pos == burrows.value(burrowIndex).riceGrasses.value(i).pos){
                        int total = burrows.value(burrowIndex).riceGrasses.value(i).quantity;
                        int available = (int)burrows.value(burrowIndex).riceGrasses.value(i).quantityAvailable;
                        info3 = "Quantity: "+QString::number(available);
                        double growth =  ((double)available/(double)total)*100.0;
                        info4 = "Growth: "+QString::number(growth,'f',0)+"%";
                        break;
                    }
                }
            }else{
                int variation = worldBuilder->getOverlayVariation(pos,Overlay_RiceGrass,worldBuilder->isUnderground);
                info3 = "Quantity: "+QString::number(variation);
            }

        }else if(worldBuilder->hasOverlay(pos,Overlay_Grass,worldBuilder->isUnderground)){
            //Display owner
            info1 = global->getName(Overlay_Grass);
            int burrowId = worldBuilder->getOwner(pos.x(),pos.y());
            if (burrowId!=None){
                int burrowIndex = getBurrowIndex(burrowId);
                QString name = burrows.value(burrowIndex).name;
                info2 = "Claimed by "+name;
                for (int i=0;i<burrows.value(burrowIndex).grasses.length();i++){
                    if(pos == burrows.value(burrowIndex).grasses.value(i).pos){
                        int total = burrows.value(burrowIndex).grasses.value(i).quantity;
                        int available = (int)burrows.value(burrowIndex).grasses.value(i).quantityAvailable;
                        info3 = "Quantity: "+QString::number(available);
                        double growth =  ((double)available/(double)total)*100.0;
                        info4 = "Growth: "+QString::number(growth,'f',0)+"%";
                        break;
                    }
                }
            }else{
                int variation = worldBuilder->getOverlayVariation(pos,Overlay_Grass,worldBuilder->isUnderground);
                info3 = "Quantity: "+QString::number(variation);
            }

        }
    }    

    QVector<QString> strings;
    strings.append(info1);
    strings.append(info2);
    strings.append(info3);
    strings.append(info4);
    strings.append(info5);

    worker->displayTileInfo(strings);
}


void Registry::possess(int wombatId, QPoint viewPosition, QPoint moved, bool underground, qreal rotation){
    //worker->consolePrint("possesing wombat id: "+QString::number(wombatId));
    int oldWombatIndex = getWombatIndex(selectedWombat);
    int newWombatIndex = getWombatIndex(wombatId);

    if (newWombatIndex!=None){
        QPoint wombatPos = wombats.value(newWombatIndex).pos;

        QPoint diff = wombatPos-viewPosition;        
        setPosOfDepossessedWombat(selectedWombat,viewPosition); //Add the depossessed wombat to the world.
        QSize backgroundSize = worldBuilder->imagePainter->getBackgroundSize();
        int halfWidth = backgroundSize.width()/gTileSize/2;
        int halfHeight = backgroundSize.height()/gTileSize/2;

        if (diff.x() >= halfWidth || diff.x() <= -halfWidth || diff.y() >= halfHeight || diff.y() <= -halfHeight){
            //Wombat is far away, so teleport there
            setSelectedWombat(wombatId);
            prepareWombatInfoForDisplay(selectedWombat);
            worker->setUnderground(wombats.value(newWombatIndex).underground);
            worker->setViewPosition(wombatPos);
            worker->setXYMoved(QPoint(0,0));
            worldBuilder->removeWombatIdFromTile(wombatPos,wombatId); //Have to remove the new wombatID from the current tile
            worldBuilder->prepareToUpdateTilesForTeleport(wombatPos,wombats.value(newWombatIndex).underground);
            worker->setSelectedWombatId(wombatId);
            worker->setSelectedWombatVisibility(true);
        }else{
            //Wombat is close by, so scroll there and then update
            if (oldWombatIndex!=None){
                herder->addWombatToHerd(selectedWombat,viewPosition,QPoint(moved.x(),moved.y()),rotation); //Add old wombat to herd
            }
            worker->setSelectedWombatVisibility(false);
            setSelectedWombat(wombatId);
            prepareWombatInfoForDisplay(selectedWombat);
            if (wombats.value(newWombatIndex).underground!=underground){
                worker->setUnderground(wombats.value(newWombatIndex).underground);
                worker->setUpdatingTiles(true);
                worldBuilder->prepareToUpdateTiles(viewPosition,wombats.value(newWombatIndex).underground); //Update to switch underground/above ground view
            }
            int herdIndex = herder->wombatIds.indexOf(wombatId);
            if (herdIndex!=None){
                //worker->consolePrint("stopping timers wombat id: "+QString::number(wombatId));
                herder->wombatHerd[herdIndex]->timerMoveToTarget->stop();
                herder->wombatHerd[herdIndex]->timerNewRandomTarget->stop();
                herder->wombatHerd[herdIndex]->timerTEMPFIXFUCK->stop();
                worker->setScrollTarget(wombatId); //Scroll to new wombat position, scrolling is handled in the front end
            }else{
                worker->consolePrint("Error possess");
            }
        }
    }else{
        worker->setLockInput(false);
        worker->setLockMovement(false);
        //worker->setUpdatingTiles(false);
    }
}

void Registry::scrollToTargetPosDone(){
    //worker->consolePrint("scrollToTargetPosDone, selected wombat is: "+QString::number(selectedWombat));
    int index = herder->wombatIds.indexOf(selectedWombat);
    if (index!=None){  //Check that our new wombat has not died while we were scrolling to it
        int rotationTargetWombat = (int)herder->wombatHerd.value(index)->rotation;
        worldBuilder->removeWombatIdFromTile(worldBuilder->worldPosition,selectedWombat);
        herder->removeWombatFromHerd(index);
        worker->setSelectedWombatId(selectedWombat);
        worker->setSelectedWombatRotation(rotationTargetWombat);
        worker->setSelectedWombatVisibility(true);
        worker->setLockMovement(false);
        worker->setLockInput(false);
    }else{
        worker->setLockInput(false);
    }
}

void Registry::setSelectedWombatInRegistryUnderground(bool b){
    int wombatIndex = getWombatIndex(selectedWombat);
    if(wombatIndex>=0 && wombatIndex<wombats.length()){
        wombats[wombatIndex].underground = b;
    }else{
        worker->consolePrint("Error setSelectedWombatInRegistryUnderground");
    }
}

void Registry::setWombatUnderground(int wombatId, bool underground){
    int wombatIndex = getWombatIndex(wombatId);
    if(wombatIndex>=0 && wombatIndex<wombats.length()){
        wombats[wombatIndex].underground = underground;
    }else{
        worker->consolePrint("Error setWombatUnderground");
    }
}

void Registry::prepareBurrowListForDisplay(int selectedBurrowId){    
    int population = 0;
    int totalPopulation = 0;
    int indexSelectedBurrow = getBurrowIndex(selectedBurrowId);
    QVector<int> selectedBurrowWombatIds;
    QVector<QString> strings;

    QString list1 = "";
    QString list2 = "";
    QString list3 = "";
    QString list4 = "";
    QString temp = "";

    if (burrows.length()==0){
        list1 = "No burrows";
    }else{
        for (int i=0;i<burrows.length();i++){
            population = burrows[i].populationWombatIds.length();
            totalPopulation += burrows[i].populationWombatIds.length();
            int nests = burrows[i].nests.length();

            if (population==0){
                list1 += "<span style='color: gray'>"+burrows[i].name+" (abandoned)";
                list2 += "<span style='color: gray'>";
                list3 += "<span style='color: gray'>";
                list4 += "<span style='color: gray'>";
            }else{
                list1 += "<span>"+burrows[i].name;
                list2 += "<span>";
                list3 += "<span>";
                list4 += "<span>";
            }

            list1 += "</span><br>";
            list2 += QString::number(population)+"/"+QString::number(nests)+"</span><br>";
            list3 += QString::number(burrows[i].food)+"</span><br>";
            list4 += QString::number(burrows[i].allNonEntrancePos.length())+"</span><br>";

            if (i==indexSelectedBurrow){
                if (population!=0){
                    list1 += "<span></span><br>";
                    list2 += "<span></span><br>";
                    list3 += "<span></span><br>";
                    list4 += "<span></span><br>";

                    //Add all the fucking wombats
                    list1 += "<span style='font-size: 10pt; font-weight: bold;'>&nbsp;&nbsp;&nbsp;Wombats</span><br>";
                    list2 += "<span style='font-size: 10pt; font-weight: bold;'>Age</span><br>";
                    list3 += "<span style='font-size: 10pt; font-weight: bold;'>Sex</span><br>";
                    list4 += "<span style='font-size: 10pt; font-weight: bold;'>Possess</span><br>";

                    int age = 0;
                    int wombatId = 0;
                    for (int j=0;j<population;j++){
                        wombatId = burrows[indexSelectedBurrow].populationWombatIds[j];
                        //hwGameUI->sendToConsole("pop"+QString::number(population));
                        //hwGameUI->sendToConsole("wombatId"+QString::number(wombatId));
                        selectedBurrowWombatIds.append(wombatId);
                        int index = getWombatIndex(wombatId);
                        list1 += "<span style='font-size: 10pt;'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;"+wombats.value(index).name+"</span><br>";
                        age = wombats.value(index).age;
                        list2 += "<span style='font-size: 10pt;'>&nbsp;&nbsp;"+QString::number(age)+"</span><br>";
                        if (wombats.value(index).sex == 0){
                            temp = "♀";
                        }else{
                            temp = "♂";
                        }
                        list3 += "<span style='font-size: 10pt;'>&nbsp;&nbsp;"+temp+"</span><br>";

                        if (wombatId != selectedWombat){
                            if (age>1){
                                list4 += "<span style='font-size: 10pt;'>&nbsp;&nbsp;&nbsp;&nbsp;ok</span><br>";
                            }else{
                                list4 += "<span style='font-size: 8pt;'>Too young</span><br>";
                            }
                        }else{
                            list4 += "<span style='font-size: 8pt;'>Current</span><br>";
                        }
                    }
                    list1 += "<span></span><br>";
                    list2 += "<span></span><br>";
                    list3 += "<span></span><br>";
                    list4 += "<span></span><br>";
                }
            }
        }
    }

    strings.append(list1);
    strings.append(list2);
    strings.append(list3);
    strings.append(list4);
    strings.append(QString::number(totalPopulation));

    worker->displayBurrowList(strings,burrowIds,selectedBurrowWombatIds);
}

QString Registry::getBurrowName(QPoint pos){
    QString burrowName = "";
    int burrowId = worldBuilder->getBurrowId(pos);
    if (burrowId!=None){
        int burrowIndex = getBurrowIndex(burrowId);
        burrowName = burrows.value(burrowIndex).name;
    }
    return burrowName;
}


void Registry::showCorpse(QPoint pos, qreal rotation, bool isUnderground){
    //Corpse is added to visibleTile array, so it stays after updating, but it
    //is not added to the chunks array, so it won't be saved to file, and if the
    //tile is reloaded the corpse will be gone
    VisibleTileStruct &visibleTile = worldBuilder->getVisibleTile(pos);
    visibleTile.rotation = rotation;

    if (isUnderground){
        bool alreadyAdded = false;
        for (int i=0;i<visibleTile.overlayTerrainTypesUnderground.length();i++){
            if (visibleTile.overlayTerrainTypesUnderground.value(i) == Corpse){
                alreadyAdded = true;
                break;
            }
        }

        if (alreadyAdded==false){
            visibleTile.overlayTerrainTypesUnderground.append(Corpse);
            visibleTile.overlayVariationsUnderground.append(2);
            QVector<int> rrre;
            rrre.append(1);
            rrre.append(3);
            rrre.append(7);
            rrre.append(9);
            visibleTile.overlayMiniTileNumbersUnderground.append(rrre);

            visibleTile.rotation = rotation;
            //worker->consolePrint("Corpse added to underground overlay");
        }

    }else{
        bool hasHole = worldBuilder->hasTerrain(Terrain_Hole,false,pos);
        if(!hasHole){
            bool alreadyAdded = false;
            for (int i=0;i<visibleTile.overlayTerrainTypes.length();i++){
                if (visibleTile.overlayTerrainTypes.value(i) == Corpse){
                    alreadyAdded = true;
                    break;
                }
            }

            if (alreadyAdded==false){
                visibleTile.overlayTerrainTypes.append(Corpse);
                visibleTile.overlayVariations.append(1);
                QVector<int> rrre;
                rrre.append(1);
                rrre.append(3);
                rrre.append(7);
                rrre.append(9);
                visibleTile.overlayMiniTileNumbers.append(rrre);

                visibleTile.rotation = rotation;
                //worker->consolePrint("Corpse added to above ground overlay");
            }
        }
    }
}

void Registry::claimResource(QPoint pos, TileType tileType){    
    Resource resource = Resource_None;
    Overlay overlay = Overlay_None;

    if(tileType == RiceGrass){
        resource = Resource_RiceGrass;
        overlay = Overlay_RiceGrass;
    }else if(tileType == Grass){
        resource = Resource_Grass;
        overlay = Overlay_Grass;
    }else if (tileType == Bush){
        resource = Resource_Sticks;
        overlay = Overlay_Bush;
    }

    int index = wombatIds.indexOf(selectedWombat);
    int burrowId = wombats.value(index).burrowId;
    if (burrowId != None){

        int owner = worldBuilder->getOwner(pos.x(),pos.y());

        if (burrowId!=owner){
            //Check distance from burrow entrance
            int shortestDistance = 6;
            QPoint p = QPoint(0,0);
            int distance = 0;
            int burrowIndex = getBurrowIndex(burrowId);

            for (int i=0;i<burrows.value(burrowIndex).entrancePos.length();i++){
                p = pos - burrows.value(burrowIndex).entrancePos[i];
                distance = p.manhattanLength();
                if (distance<shortestDistance){
                    shortestDistance=distance;
                }
            }

            QString s1, s2, s3, s4, s5;
            QString resourceName = global->getResourceName(resource);

            if(resourceName == "Rice Grass"){
                s1 = "Delicious Rice Grass!";
            }else{
                s1 = resourceName;
            }

            s3 = burrows.value(burrowIndex).name;
            s4 = s5 = "";

            if (shortestDistance>5){
                s2 = "Too far away from burrow";
            }else{
                s2 = "Claimed for burrow";

                int quantity = worldBuilder->getOverlayVariation(pos,overlay,false);

                if (owner!=None){ //Remove resource from old owner
                    int ownerIndex = getBurrowIndex(owner);
                    if(ownerIndex!=None){
                        if(tileType == RiceGrass){
                            burrows[ownerIndex].food -= quantity;
                            int indexToRemove = None;
                            for (int i=0;i<burrows.value(burrowIndex).riceGrasses.length();i++){
                                if(pos == burrows.value(burrowIndex).riceGrasses.value(i).pos){
                                    indexToRemove = i;
                                    break;
                                }
                            }
                            if(indexToRemove>=0 && indexToRemove<burrows.value(burrowIndex).riceGrasses.length()){
                                burrows[ownerIndex].riceGrasses.remove(indexToRemove);
                            }
                        }else if(tileType == Grass){
                            int indexToRemove = None;
                            for (int i=0;i<burrows.value(burrowIndex).grasses.length();i++){
                                if(pos == burrows.value(burrowIndex).grasses.value(i).pos){
                                    indexToRemove = i;
                                    break;
                                }
                            }

                            if(indexToRemove>=0 && indexToRemove<burrows.value(burrowIndex).grasses.length()){
                                burrows[ownerIndex].grasses.remove(indexToRemove);
                            }
                        }else if (tileType == Bush){
                            int indexToRemove = None;
                            for (int i=0;i<burrows.value(burrowIndex).bushes.length();i++){
                                if(pos == burrows.value(burrowIndex).bushes.value(i).pos){
                                    indexToRemove = i;
                                    break;
                                }
                            }

                            if(indexToRemove>=0 && indexToRemove<burrows.value(burrowIndex).bushes.length()){
                                burrows[ownerIndex].bushes.remove(indexToRemove);
                            }
                        }
                    }
                }

                //Set the new owner
                worldBuilder->setOwner(pos.x(),pos.y(),burrowId);
                if (burrowIndex >= 0 && burrowIndex < burrows.length()){
                    BurrowResourceStruct newResource;
                    newResource.pos = pos;
                    newResource.growth = 100.0;
                    newResource.quantity = quantity;
                    newResource.quantityAvailable = quantity;

                    if(tileType == RiceGrass){
                        burrows[burrowIndex].food += quantity;
                        burrows[burrowIndex].riceGrasses.append(newResource);
                    }else if(tileType == Grass){
                        burrows[burrowIndex].grasses.append(newResource);
                    }else if (tileType == Bush){                        
                        burrows[burrowIndex].bushes.append(newResource);
                    }
                    worker->playSoundFX(Sound_NewHome);
                }else{
                    worker->consolePrint("Error claimResource");
                }
            }
            QVector<QString> strings;
            strings.append(s1);
            strings.append(s2);
            strings.append(s3);
            strings.append(s4);
            strings.append(s5);
            worker->displayTileInfo(strings);
        }
    }
}

void Registry::collectResource(QPoint pos, TileType tileType){
    if(tileType == RiceGrass){
        collectRiceGrass(pos);
    }else if(tileType == Grass){
        collectGrass(pos);
    }else if (tileType == Bush){
        collectBush(pos);
    }
}

void Registry::collectRiceGrass(QPoint pos){
    int sizeLeft = sizeLeftInWombatInventory(selectedWombat);
    InventoryItem item;
    item.resource = Resource_RiceGrass;
    item.amount = 1;
    int itemSize = global->getItemSize(item);

    if(itemSize<=sizeLeft){
        int owner = worldBuilder->getOwner(pos.x(),pos.y());
        if (owner==None){
            addItemToWombatInventory(selectedWombat,item.resource,item.amount);
            worker->playSoundFX(Sound_ClaimRiceGrass);
        }else{
            int burrowIndex = getBurrowIndex(owner);
            //Check how much RiceGrass is available at this location
            for (int i=0;i<burrows.value(burrowIndex).riceGrasses.length();i++){
                if(pos == burrows.value(burrowIndex).riceGrasses.value(i).pos){
                    if(burrows.value(burrowIndex).riceGrasses.value(i).quantityAvailable>0){
                        burrows[burrowIndex].riceGrasses[i].quantityAvailable -= item.amount;
                        addItemToWombatInventory(selectedWombat,item.resource,item.amount);
                        worker->playSoundFX(Sound_ClaimRiceGrass);
                    }
                    if(burrows.value(burrowIndex).riceGrasses.value(i).quantityAvailable==0){
                        worker->stopCollectResource();
                    }
                    break;
                }
            }
        }
    }
}

void Registry::collectGrass(QPoint pos){
    int sizeLeft = sizeLeftInWombatInventory(selectedWombat);
    InventoryItem item;
    item.resource = Resource_Grass;
    item.amount = 1;
    int itemSize = global->getItemSize(item);

    if(itemSize<=sizeLeft){
        int owner = worldBuilder->getOwner(pos.x(),pos.y());
        if (owner==None){
            addItemToWombatInventory(selectedWombat,item.resource,item.amount);
            worker->playSoundFX(Sound_ClaimRiceGrass);
        }else{
            int burrowIndex = getBurrowIndex(owner);
            //Check how much Grass is available at this location
            for (int i=0;i<burrows.value(burrowIndex).grasses.length();i++){
                if(pos == burrows.value(burrowIndex).grasses.value(i).pos){
                    if(burrows.value(burrowIndex).grasses.value(i).quantityAvailable>0){
                        burrows[burrowIndex].grasses[i].quantityAvailable -= item.amount;
                        addItemToWombatInventory(selectedWombat,item.resource,item.amount);
                        worker->playSoundFX(Sound_ClaimRiceGrass);
                    }
                    if(burrows.value(burrowIndex).grasses.value(i).quantityAvailable==0){
                        worker->stopCollectResource();
                    }
                    break;
                }
            }
        }
    }
}

void Registry::collectBush(QPoint pos){
    int sizeLeft = sizeLeftInWombatInventory(selectedWombat);
    InventoryItem item;
    item.resource = Resource_Sticks;
    item.amount = 1;
    int itemSize = global->getItemSize(item);

    if(itemSize<=sizeLeft){
        int owner = worldBuilder->getOwner(pos.x(),pos.y());
        if (owner==None){
            addItemToWombatInventory(selectedWombat,item.resource,item.amount);
            worker->playSoundFX(Sound_ClaimRiceGrass);
        }else{
            int burrowIndex = getBurrowIndex(owner);
            //Check how many Sticks are available at this location
            for (int i=0;i<burrows.value(burrowIndex).bushes.length();i++){
                if(pos == burrows.value(burrowIndex).bushes.value(i).pos){
                    if(burrows.value(burrowIndex).bushes.value(i).quantityAvailable>0){
                        burrows[burrowIndex].bushes[i].quantityAvailable -= item.amount;
                        addItemToWombatInventory(selectedWombat,item.resource,item.amount);
                        worker->playSoundFX(Sound_ClaimRiceGrass);                        
                    }
                    if(burrows.value(burrowIndex).bushes.value(i).quantityAvailable==0){
                        worker->stopCollectResource();
                    }
                    break;
                }
            }
        }
    }
}

int Registry::sizeLeftInWombatInventory(int wombatId){
    int wombatIndex = getWombatIndex(wombatId);
    int sizeLeft = gWombatInventorySize;

    for (int i=0;i<wombats.value(wombatIndex).inventory.length();i++){
       const InventoryItem &item = wombats.value(wombatIndex).inventory.value(i);
       int itemSize = global->getItemSize(item);
       sizeLeft -= itemSize;
    }

    return sizeLeft;
}

void Registry::moveItemFromWombatToStorage(InventoryItem item, int wombatId, int burrowId, int storageId, QPoint draggedLabelPos){

        QVector<InventoryItem> wombatInventory = getWombatInventory(wombatId);

        bool itemFound = false;
        for (int i=0;i<wombatInventory.length();i++){
            if(wombatInventory.value(i).resource == item.resource){
                if(wombatInventory.value(i).amount < item.amount){
                    item.amount = wombatInventory.value(i).amount;
                }
                itemFound = true;
                break;
            }
        }

        if (itemFound){
            int burrowIndex = getBurrowIndex(burrowId);            
            int storageIndex = getStorageIndex(burrowIndex,storageId);
            bool inRange = checkIfInRange(burrowIndex,storageIndex);
            //hwGameUI->console->print(QString::number(inRange));
            if(inRange){
                InventoryItem returnedItem = addItemToStorage(burrowIndex,storageIndex,item);
                InventoryItem toBeRemoved = item;
                toBeRemoved.amount = item.amount-returnedItem.amount;
                addItemToWombatInventory(wombatId,toBeRemoved.resource,-toBeRemoved.amount);
            }else{
                worker->displayOutOfRangeMessage(draggedLabelPos);
            }


        }

}

void Registry::moveItemFromStorageToWombat(InventoryItem item, int wombatId, int burrowId, int storageId, QPoint draggedLabelPos){
        int burrowIndex = getBurrowIndex(burrowId);
        int storageIndex = getStorageIndex(burrowIndex,storageId);

        QVector<InventoryItem> storageInventory = getStorageInventory(burrowIndex,storageIndex);

        bool itemFound = false;
        for (int i=0;i<storageInventory.length();i++){
            if(storageInventory.value(i).resource == item.resource){
                if(storageInventory.value(i).amount < item.amount){
                    item.amount = storageInventory.value(i).amount;
                }
                itemFound = true;
                break;
            }
        }

        if (itemFound){
            bool inRange = checkIfInRange(burrowIndex,storageIndex);

            if(inRange){
                InventoryItem returnedItem = addItemToWombatInventory(wombatId,item.resource,item.amount);
                InventoryItem toBeRemoved = item;
                toBeRemoved.amount = item.amount-returnedItem.amount;
                removeItemFromStorage(burrowIndex,storageIndex,toBeRemoved);
            }else{
                worker->displayOutOfRangeMessage(draggedLabelPos);
            }


        }

}

bool Registry::checkIfInRange(int burrowIndex, int storageIndex){
    if(worldBuilder->isUnderground){
        QPoint wPos = worldBuilder->worldPosition;

        for (int i=0;i<burrows.value(burrowIndex).storages.value(storageIndex).allTiles.length();i++){
            QPoint temp = wPos-burrows.value(burrowIndex).storages.value(storageIndex).allTiles.value(i).pos;
            if(temp.manhattanLength()<=1){
                return true;
            }
        }
    }

    return false;
}

void Registry::setWInventoryWidgetData(bool wInventoryWidgetOpen, int wombatId){
    this->guiWInventoryWidgetOpen = wInventoryWidgetOpen;
    this->guiWInventoryWidgetWombatId = wombatId;
}

void Registry::setStorageWidgetData(bool storageWidgetOpen, int storageId){
    this->guiStorageWidgetOpen = storageWidgetOpen;
    this->guiStorageWidgetStorageId = storageId;
}

void Registry::prepareStorageInfoForShowStorageWidget(QPoint pos){
    int burrowId = worldBuilder->getBurrowId(pos);
    int storageId = getStorageId(pos,burrowId);
    int burrowIndex = getBurrowIndex(burrowId);
    int storageIndex = getStorageIndex(burrowIndex,storageId);
    QVector<bool> acceptedResources;
    if (burrowIndex>=0 && burrowIndex<burrows.length()){
        if(storageIndex>=0 && storageIndex<burrows.value(burrowIndex).storages.length()){
            acceptedResources = burrows[burrowIndex].storages[storageIndex].acceptedResources;
        }
    }

    if(storageId!=None){
        worker->returnStorageInfoForShowStorageWidget(burrowId, storageId, acceptedResources);
    }
}

QVector<PathPoint> Registry::findPath(PathPoint start,PathPoint end){
    bool debug = false;
   // worker->consolePrint("findPath() started");
   // worker->consolePrint("pathpoint start "+QString::number(start.pos.x())+","+QString::number(start.pos.y())+" underground: "+QString::number(start.underground));
   // worker->consolePrint("pathpoint end "+QString::number(end.pos.x())+","+QString::number(end.pos.y())+" underground: "+QString::number(end.underground));

    bool isUnderground = start.underground;
    QVector<FrontierStruct> frontier;
    FrontierStruct frontierPoint;
    frontierPoint.pos = start.pos;
    QPoint diff = start.pos-end.pos;
    int distance = diff.manhattanLength();
    frontierPoint.distanceFromStart = 0;
    frontierPoint.distanceToTarget = distance;
    frontierPoint.distance = frontierPoint.distanceFromStart+frontierPoint.distanceToTarget;
    frontierPoint.underground = isUnderground;
    frontier.append(frontierPoint);
    if(debug){
        if(frontierPoint.underground){
            worldBuilder->addOverlayToTileUnderground(start.pos,Overlay_Cross,1);
        }else{
            worldBuilder->addOverlayToTile(start.pos,Overlay_Cross,1);
        }
    }
    visited.clear();
    PathFindStruct addPos;
    addPos.pos = start.pos;;
    addPos.cameFrom = start.pos;
    addPos.underground = isUnderground;
    visited.append(addPos);

 //******************** FIX! REMOVE  //Special fix if startPos is in/under a hole
    {
        bool hasHole = worldBuilder->hasTerrain(Terrain_Hole,false,start.pos);
        if(hasHole){
            if(!(visited.value(0).pos == start.pos && visited.value(0).underground == !isUnderground)){
                FrontierStruct frontierPoint;
                frontierPoint.pos = start.pos;
                QPoint diff = start.pos-end.pos;
                int distance = diff.manhattanLength();
                frontierPoint.distanceFromStart = 0; //When traveling through hole, don't increase distance from start
                frontierPoint.distanceToTarget = distance;
                frontierPoint.distance = frontierPoint.distanceFromStart+frontierPoint.distanceToTarget;
                frontierPoint.underground = !isUnderground;
                frontier.append(frontierPoint);
                PathFindStruct addPos;
                addPos.pos = start.pos;
                addPos.cameFrom = start.pos; //Came from same pos, because we are just traveling through hole
                addPos.underground = !isUnderground;
                visited.append(addPos);
            }
        }
    }
 //********************

    bool endReached = false;
    int count = 0;
    while(!endReached && frontier.length()>0 && count<1000){
        //worker->consolePrint("path count: "+QString::number(count));
        //Chose the tile closest to the target
        int shortestDistance = 2147483647; //
        int index = 0;
        for (int i=0;i<frontier.length();i++){
            if (frontier.value(i).distance<=shortestDistance){
                shortestDistance = frontier.value(i).distance;
                index = i;
            }
        }        

        FrontierStruct currentTile = frontier.takeAt(index);
        isUnderground = currentTile.underground;
        QPoint currentPos = currentTile.pos;
        //worker->consolePrint("currentTilePos: "+QString::number(currentPos.x())+","+QString::number(currentPos.y())+" to path");
        //QPoint currentPos = frontier.takeFirst().pos;

        //Check all neighbours of that tile (and same tile if it has a hole)
        for (int i=-1;i<2 && !endReached;i++){
            for (int j=-1;j<2 && !endReached;j++){
                bool isCurrentTile = false;
                bool hasHole = false;
                if((i==0 && j==0)){
                    isCurrentTile = true;
                    hasHole = worldBuilder->hasTerrain(Terrain_Hole, false, currentPos+QPoint(i,j));
                    if(hasHole){
                        isUnderground = !isUnderground;
                    }
                }

                if(!isCurrentTile || (isCurrentTile && hasHole)){  //Ignore self (pos 0,0) if there is no hole
                    bool diagonalBlock = false;
                    if(isUnderground==true){
                        if((i==-1&&j==-1)||(i==-1&&j==1)||(i==1&&j==-1)||(i==1&&j==1)){
                            //Diagonal movement blocked underground
                            //worker->consolePrint("diagonal block ij:"+QString::number(i)+","+QString::number(j)+" pos "+QString::number(currentPos.x())+","+QString::number(currentPos.y()));
                            diagonalBlock = true;
                        }
                    }

                    if(!diagonalBlock){
                        QPoint pos = currentPos+QPoint(i,j);
                        double newDistance = 1;
                        if((i==-1&&j==-1)||(i==-1&&j==1)||(i==1&&j==-1)||(i==1&&j==1)){
                            newDistance = 3; //Punish moving diagonally to get straighter paths
                        }                        
                        if((i==0 && j==0)){
                            newDistance = 0;
                        }


                        if(pos == end.pos && isUnderground==end.underground){
                            PathFindStruct addPos;
                            addPos.pos = pos;
                            addPos.cameFrom = currentPos;
                            addPos.underground = isUnderground;
                            visited.append(addPos);
                            endReached = true;
                            if(debug){
                                if(addPos.underground){
                                    worldBuilder->addOverlayToTileUnderground(pos,Overlay_Cross,1);
                                }else{
                                    worldBuilder->addOverlayToTile(pos,Overlay_Cross,1);
                                }
                            }
                        }else{
                            //Check that the tile has not already been visited
                            bool hasBeenVisited = false;
                            for (int k=0;k<visited.length();k++){
                                if(visited.value(k).pos == pos && visited.value(k).underground == isUnderground){
                                    hasBeenVisited = true;
                                    break;
                                }
                            }

                            //if(notVisited || shorterPathFound){
                            if(!hasBeenVisited){
                                bool isUnDug = worldBuilder->hasTerrain(Terrain_UnDug,isUnderground,pos);
                                bool hasBush = worldBuilder->hasOverlay(pos,Overlay_Bush,isUnderground);

                                if(!hasBush && !isUnDug){
                                    FrontierStruct frontierPoint;
                                    frontierPoint.pos = pos;
                                    QPoint diff = pos-end.pos;
                                    int distance = diff.manhattanLength();
                                    frontierPoint.distanceFromStart = currentTile.distanceFromStart+newDistance;
                                    frontierPoint.distanceToTarget = distance;
                                    frontierPoint.distance = frontierPoint.distanceFromStart+frontierPoint.distanceToTarget;
                                    frontierPoint.underground = isUnderground;
                                    frontier.append(frontierPoint);
                                    if(debug){
                                        if(frontierPoint.underground){
                                            worldBuilder->addOverlayToTileUnderground(pos,Overlay_Cross,1);
                                        }else{
                                            worldBuilder->addOverlayToTile(pos,Overlay_Cross,1);
                                        }
                                    }

                                    //worldBuilder->prepareToUpdateTiles(QPoint(0,0),false);
                                    //Sleep(540);
                                    //worker->consolePrint("added tile: "+QString::number(pos.x())+","+QString::number(pos.y())+" to path");
                                }
                                PathFindStruct addPos;
                                addPos.pos = pos;
                                addPos.cameFrom = currentPos;
                                addPos.underground = isUnderground;
                                visited.append(addPos);
                            }
                        }
                    }
                }
            }
        }
        count++;
    }    

    //Create path
    bool done = false;
    QPoint nextTile = visited.last().cameFrom;
    bool nextUnderground = visited.last().underground;
    QVector<PathPoint> path;    //The path is constructed in reverse, starting with the end tile and working our way back to the start
    PathPoint p;
    p.pos = end.pos;
    p.underground = end.underground;
    path.append(p);
    while(!done){
        for (int i=0;i<visited.length();i++){
            if(visited.value(i).pos == nextTile && visited.value(i).underground == nextUnderground){
                p.pos = visited.value(i).pos;
                p.underground = visited.value(i).underground;
                path.append(p);
                nextTile = visited.value(i).cameFrom;                

                if(nextTile==start.pos && nextUnderground == start.underground){
                    p.pos = nextTile;
                    p.underground = start.underground;
                    path.append(p);
                    done = true;
                }

                if (nextTile == visited.value(i).pos){ //If we are moving vertically
                    nextUnderground = !nextUnderground; //Change underground
                }
                break;
            }
        }
    }

    if(debug){
        worker->consolePrint("pathlength: "+QString::number(path.length()));
        for (int i=0;i<path.length();i++){
            if(path.value(i).underground){
                worldBuilder->addOverlayToTileUnderground(path.value(i).pos,Overlay_Cross,2);
            }else{
                worldBuilder->addOverlayToTile(path.value(i).pos,Overlay_Cross,2);
            }
        }
    }

    //worldBuilder->prepareToUpdateTiles(QPoint(0,0),false);
/*
    //Debug print
    for (int i=0;i<visited.length();i++){
        worker->consolePrint("visited pos:"+QString::number(visited.value(i).pos.x())+","+QString::number(visited.value(i).pos.y()));
        worker->consolePrint("camefrom:"+QString::number(visited.value(i).cameFrom.x())+","+QString::number(visited.value(i).cameFrom.y())+" underground:"+QString::number(visited.value(i).underground));
        worker->consolePrint(" ");
    }

    for (int i=0;i<path.length();i++){
        worker->consolePrint("path: "+QString::number(path.value(i).pos.x())+","+QString::number(path.value(i).pos.y()));
    }
*/
    //worker->consolePrint("pathlength "+QString::number(path.length()));
    //worker->consolePrint("findPath() done");


    return path;
}

void Registry::setInfoBoxOpen(bool b){
    guiInfoBoxOpen = b;
}

void Registry::setAcceptedResourcesInStorage(int burrowId, int storageId, QVector<bool> acceptedResources){
    int burrowIndex = getBurrowIndex(burrowId);
    int storageIndex = getStorageIndex(burrowIndex,storageId);
    if (burrowIndex>=0 && burrowIndex<burrows.length()){
        if(storageIndex>=0 && storageIndex <burrows.value(burrowIndex).storages.length()){
            burrows[burrowIndex].storages[storageIndex].acceptedResources = acceptedResources;
        }
    }
}

void Registry::showOwner(int burrowId){
    int burrowIndex = getBurrowIndex(burrowId);

    for (int i=0;i<burrows.value(burrowIndex).riceGrasses.length();i++){
        worldBuilder->addOverlayToVisibleTile(burrows.value(burrowIndex).riceGrasses.value(i).pos,Overlay_Basket,1);
    }

    for (int i=0;i<burrows.value(burrowIndex).grasses.length();i++){
        worldBuilder->addOverlayToVisibleTile(burrows.value(burrowIndex).grasses.value(i).pos,Overlay_Basket,1);
    }

    for (int i=0;i<burrows.value(burrowIndex).bushes.length();i++){
        worldBuilder->addOverlayToVisibleTile(burrows.value(burrowIndex).bushes.value(i).pos,Overlay_Basket,1);
    }

    worldBuilder->prepareToUpdateTiles(worldBuilder->worldPosition,worldBuilder->isUnderground);
}

void Registry::hideOwner(int burrowId){
    int burrowIndex = getBurrowIndex(burrowId);

    for (int i=0;i<burrows.value(burrowIndex).riceGrasses.length();i++){
        worldBuilder->removeOverlayFromVisibleTile(burrows.value(burrowIndex).riceGrasses.value(i).pos,Overlay_Basket,1);
    }

    for (int i=0;i<burrows.value(burrowIndex).grasses.length();i++){
        worldBuilder->removeOverlayFromVisibleTile(burrows.value(burrowIndex).grasses.value(i).pos,Overlay_Basket,1);
    }

    for (int i=0;i<burrows.value(burrowIndex).bushes.length();i++){
        worldBuilder->removeOverlayFromVisibleTile(burrows.value(burrowIndex).bushes.value(i).pos,Overlay_Basket,1);
    }

    worldBuilder->prepareToUpdateTiles(worldBuilder->worldPosition,worldBuilder->isUnderground);
}

void Registry::printAllWombatsToConsole(){
    QString s = "";

    for (int i=0;i<wombats.length();i++){
        s = QString::number(wombats.value(i).id)+"|";
        s += wombats.value(i).name+"|";
        s += QString::number(wombats.value(i).sex)+"|";
        s += QString::number(wombats.value(i).age)+"|";
        s += QString::number(wombats.value(i).birthMonth)+"|";
        s += QString::number(wombats.value(i).burrowId)+"|";
        s += QString::number(wombats.value(i).pos.x())+"|";
        s += QString::number(wombats.value(i).pos.y())+"|";
        s += QString::number(wombats.value(i).underground)+"|";
        s += QString::number(wombats.value(i).inventory.length())+"|";
        for (int j=0;j<wombats.value(i).inventory.length();j++){
            s += QString::number(wombats.value(i).inventory.value(j).resource)+"|";
            s += QString::number(wombats.value(i).inventory.value(j).amount)+"|";
        }
        s += QString::number(wombats.value(i).health)+"|";
        s += QString::number(wombats.value(i).rested)+"|";
        s += QString::number(wombats.value(i).plump)+"|";
        worker->consolePrint(s);
    }
}
