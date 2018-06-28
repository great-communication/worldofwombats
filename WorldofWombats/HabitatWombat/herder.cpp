#include "herder.h"
#include "worldbuilder.h"
#include "worker.h"
#include "registry.h"
#include "herdmember.h"
#include "world.h"

Herder::Herder(QObject *parent) : QObject(parent){

}

void Herder::setup()
{

}

void Herder::addWombatToHerd(int wombatId, QPoint tilePos, QPoint pxOffset, qreal rotation){
    herdMember = new HerdMember(this);
    wombatIds.append(wombatId);
    herdMember->hIndex = wombatIds.length()-1;
    connect(worldBuilder,SIGNAL(s_newMonth()),herdMember,SLOT(growWombat()));
    herdMember->wombatId = wombatId;    
    herdMember->worldBuilder = worldBuilder;
    herdMember->worker = worker;
    herdMember->herder = this;    
    herdMember->originalTilePos = tilePos; //This should probably be taken from wombat.pos and not sent as a parameter    
    herdMember->currentPxOffset = pxOffset;
    //worker->consolePrint("adding wombat to herd, current px offset: "+QString::number(herdMember->currentPxOffset.x())+","+QString::number(herdMember->currentPxOffset.y()));
    herdMember->rotation = rotation;
    herdMember->registry = registry;
    wombatHerd.append(herdMember);    
    wombatHerd.last()->setup();
    //worker->consolePrint("adding wombat to herd id: "+QString::number(wombatId));
}

void Herder::removeWombatFromHerd(int index){
    //worker->consolePrint("removing wombat from herd, id: "+QString::number(wombatHerd[index]->wombatId));

    if(index>=0 && index<wombatHerd.length()){
        worker->removeWombatFromDisplay(wombatHerd[index]->wombatId);
        wombatIds.removeAt(index);
        wombatHerd[index]->timerTEMPFIXFUCK->stop();
        wombatHerd[index]->timerNewRandomTarget->stop();
        wombatHerd[index]->timerMoveToTarget->stop();
        wombatHerd[index]->deleteLater();
        wombatHerd.removeAt(index);

        for (int i=0;i<wombatHerd.length();i++){ // Update indexes
            wombatHerd[i]->hIndex = i;
        }
    }else{
        worker->consolePrint("Error removeWombatFromHerd");
        //worker->consolePrint("Error removeWombatFromHerd index: "+QString::number(index));
    }
}

void Herder::removeAllWombatsFromHerd(){
    QVector<int> toBeRemoved;
    for (int i=0;i<wombatIds.length();i++){
            toBeRemoved.append(i);
    }

    for (int i=toBeRemoved.length();i>0;i--){
        removeWombatFromHerd(i-1);
    }
}

void Herder::updateHerd(QPoint pos, int maxIndex){
   // worker->consolePrint("updating herd");
    QVector<int> toBeRemoved;
    QPoint p;

    if (maxIndex <=wombatHerd.length()){
        QPoint chunkSize = worldBuilder->world->getChunkSize();

        for (int i=0;i<maxIndex;i++){ //maxIndex because we don't want to update the wombats that were added this update            
            p = wombatHerd.value(i)->oldTilePos-worldBuilder->worldPosition;

            if (p.x() > (chunkSize.x()/2)-1 || p.x() < -(chunkSize.x()/2) || p.y() > (chunkSize.x()/2)-1 || p.y() < -(chunkSize.y()/2)){
                toBeRemoved.append(i);
            }
        }
    }else{
        worker->consolePrint("Error updateHerd");
    }

    //Remove wombats that are too far away
    //Have to remove from back or the indexes will not be valid anymore
    for (int i=toBeRemoved.length();i>0;i--){
        removeWombatFromHerd(toBeRemoved.value(i-1));        
    }
}

void Herder::updateVisibility(){/*
    for (int i=0;i<wombatHerd.length();i++){
        if (wombatHerd.value(i)->underground == worldBuilder->underground){
            //wombatHerd[i]->timerNewTarget->start(hwGameObject->getRandomNumber(2000,8000));
            wombatHerd[i]->visible = true;
            wombatHerd[i]->move = QPoint(0,0);
            wombatHerd[i]->updatePosition();
        }else{
            //wombatHerd[i]->timerNewTarget->stop();
            //wombatHerd[i]->timerMoveToTarget->stop();
            wombatHerd[i]->visible = false;
            wombatHerd[i]->move = QPoint(0,0);
            wombatHerd[i]->updatePosition();
        }
    }*/
}

void Herder::addStorageTileToHerd(int burrowId, QPoint pos){
    //worker->consolePrint("add storage display to herd");
    int indexToBeRemoved = None;
    for (int i=0;i<storageHerdFUCK.length();i++){
        if(storageHerdFUCK.value(i).pos == pos){
            indexToBeRemoved = i;
            break;
        }

    }

    if(indexToBeRemoved!=None){
        storageHerdFUCK.remove(indexToBeRemoved);
    }

    storageHerdStruct temp;
    temp.burrowId = burrowId;
    temp.pos = pos;
    storageHerdFUCK.append(temp);


    bool done = false;
    int storageIndex = -1;
    int tileIndex = -1;
    int burrowIndex = registry->getBurrowIndex(burrowId);

    for (int i=0;i<registry->burrows.value(burrowIndex).storages.length()&&!done;i++){
        for (int j=0;j<registry->burrows.value(burrowIndex).storages.value(i).allTiles.length()&&!done;j++){
            if(pos == registry->burrows.value(burrowIndex).storages.value(i).allTiles.value(j).pos){
                storageIndex = i;
                tileIndex = j;
                registry->burrows[burrowIndex].storages[i].allTiles[j].visible = true;
                done=true;
            }
        }
    }

    //console->print("indexes: "+QString::number(storageIndex)+","+QString::number(tileIndex)+","+QString::number(inventoryIndex));

    if(storageIndex!=-1 && tileIndex!=-1){
        //worker->consolePrint("update storage display");
        updateStorageDisplay(burrowIndex,storageIndex,tileIndex);
        //console->print("storage tile updated");
    }else{
        //worker->consolePrint("fail!");
    }

    //console->print("storage tile added to herd");

    /*int storageId = hwRegistry->getStorageId(pos, burrowId);
    bool alreadyAdded = false;
    if (storageId !=-1){
        for (int i=0;i<storageHerd.length();i++){
            if(storageId == storageHerd.value(i)){
                alreadyAdded = true;
                break;
            }
        }

        if (!alreadyAdded){
            storageHerd.append(storageId);            
        }

    }else{
        console->print("Error addStorageToHerd");
    }*/
}

void Herder::updateStorageDisplay(int burrowIndex, int storageIndex, int tileIndex){
    QPoint pos = registry->burrows[burrowIndex].storages[storageIndex].allTiles[tileIndex].pos;
    worldBuilder->removeOverlayFromVisibleTileUnderground(pos,Overlay_RiceGrass_Bundle04,1);
    worldBuilder->removeOverlayFromVisibleTileUnderground(pos,Overlay_Grass_Bundle01,1);
    worldBuilder->removeOverlayFromVisibleTileUnderground(pos,Overlay_Sticks_Bundle01,1);

    QVector<InventoryItem> inventory = registry->burrows[burrowIndex].storages[storageIndex].allTiles[tileIndex].inventory;

    //Temp fix. Inventory is not up to date when updateStorageDisplay is called, so need this fucking fix here
    QVector<int> itemsToBeRemoved;
    for (int i=0;i<inventory.length();i++){
        if(inventory.value(i).amount <=0){
            itemsToBeRemoved.append(i);
        }
    }

    for (int i=itemsToBeRemoved.length()-1;i>=0;i--){
        inventory.remove(itemsToBeRemoved.value(i));
    }
    //

    //worker->consolePrint("inventory length: "+QString::number(inventory.length()));

    if(inventory.length()>0){

        QVector<int> sizeValues;

        //Check total size used
        int totalSizeUsed = 0;
        for (int i=0;i<inventory.length();i++){
            int size = worldBuilder->global->getItemSize(inventory.value(i));
            sizeValues.append(size);
            totalSizeUsed += size;
        }
        if(totalSizeUsed>0){


            //Calculate how many positions we have total
            bool overlapLeft = false;
            bool overlapRight = false;

            overlapLeft = worldBuilder->hasOverlay(pos+QPoint(-1,0),Overlay_Storage,true);
            overlapRight = worldBuilder->hasOverlay(pos+QPoint(1,0),Overlay_Storage,true);

            QVector<int> backrow;
            QVector<int> frontrow;
            backrow.append(2);
            backrow.append(3);
            frontrow.append(6);


            int posCount = 3; //Number of image positions on each storage tile
            if(overlapLeft){
                posCount += 2;
                backrow.append(1);
                frontrow.append(5);
            }

            if(overlapRight){
                posCount += 2;
                backrow.append(4);
                frontrow.append(7);
            }


            // Calculate how many positions are used
            double percentageUsed = (double)totalSizeUsed / (double)gStorageSize;
            int positionsUsed = ceil(((double)posCount * percentageUsed));
            int sizePerPosition = gStorageSize / positionsUsed;

            //Now calculate with wich resources we fill these positions

            if(positionsUsed>0){
                int positionsLeft = positionsUsed;






                QVector<int> sizes;

                int largestSizeSoFar = 0;
                int indexOfLargestSize = 0;

                for (int i=0;i<inventory.length();i++){
                    int thisItemSize = worldBuilder->global->getItemSize(inventory.value(i));
                    if(thisItemSize>largestSizeSoFar){
                        largestSizeSoFar = thisItemSize;
                        indexOfLargestSize = i;
                    }
                    int positionsForThisItem = thisItemSize / sizePerPosition;
                    positionsLeft -= positionsForThisItem;
                    sizes.append(positionsForThisItem);
                }

                if (positionsLeft>0){
                    if(indexOfLargestSize>=0 && indexOfLargestSize<sizes.length()){
                        sizes[indexOfLargestSize] += positionsLeft;
                    }
                }

                QVector<int> riceGrassMiniTileNumbers;
                QVector<int> grassMiniTileNumbers;
                QVector<int> sticksMiniTileNumbers;

                //Now we know how many positions used by each resource
                //Then start filling the rows from back to front
                //First sticks, then grass, then ricegrass

                for (int i=0;i<inventory.length();i++){
                    if(inventory.value(i).resource == Resource_Sticks){                        
                        for (int j=0;j<sizes.value(i);j++){
                            if(backrow.length()>0){
                                sticksMiniTileNumbers.append(backrow.takeFirst());
                            }else if(frontrow.length()>0){
                                sticksMiniTileNumbers.append(frontrow.takeFirst());
                            }
                        }
                        break;
                    }
                }

                for (int i=0;i<inventory.length();i++){                    
                    if(inventory.value(i).resource == Resource_Grass){                        
                        for (int j=0;j<sizes.value(i);j++){
                            if(backrow.length()>0){
                                grassMiniTileNumbers.append(backrow.takeFirst());
                            }else if(frontrow.length()>0){
                                grassMiniTileNumbers.append(frontrow.takeFirst());
                            }
                        }
                        break;
                    }
                }

                for (int i=0;i<inventory.length();i++){
                    if(inventory.value(i).resource == Resource_RiceGrass){
                        for (int j=0;j<sizes.value(i);j++){
                            if(backrow.length()>0){
                                riceGrassMiniTileNumbers.append(backrow.takeFirst());
                            }else if(frontrow.length()>0){
                                riceGrassMiniTileNumbers.append(frontrow.takeFirst());
                            }
                        }
                        break;
                    }
                }




                if(riceGrassMiniTileNumbers.length()>0){
                    worldBuilder->addOverlayToVisibleTileUnderground(pos,Overlay_RiceGrass_Bundle04,1,riceGrassMiniTileNumbers);
                }

                if(grassMiniTileNumbers.length()>0){                    
                    worldBuilder->addOverlayToVisibleTileUnderground(pos,Overlay_Grass_Bundle01,1,grassMiniTileNumbers);
                }

                if(sticksMiniTileNumbers.length()>0){
                    worldBuilder->addOverlayToVisibleTileUnderground(pos,Overlay_Sticks_Bundle01,1,sticksMiniTileNumbers);
                }
            }
        }
    }

  /*
    worker->consolePrint("updating storage");
    int newLevel = 0;

    if(registry->burrows[burrowIndex].storages[storageIndex].allTiles[tileIndex].inventory[inventoryIndex].resource == Resource_RiceGrass){
        if(registry->burrows[burrowIndex].storages[storageIndex].allTiles[tileIndex].inventory[inventoryIndex].amount > 12){
            newLevel = 3;
        }else if(registry->burrows[burrowIndex].storages[storageIndex].allTiles[tileIndex].inventory[inventoryIndex].amount > 6){
            newLevel = 2;
        }else if (registry->burrows[burrowIndex].storages[storageIndex].allTiles[tileIndex].inventory[inventoryIndex].amount > 0){
            newLevel = 1;
        }
    }

    int oldLevel = 0;
    QPoint pos = registry->burrows[burrowIndex].storages[storageIndex].allTiles[tileIndex].pos;
    int index = -1;

    for (int i=0;i<storageHerdFUCK.length();i++){
        if (pos == storageHerdFUCK.value(i).pos){
            oldLevel = storageHerdFUCK.value(i).level;
            index = i;
            break;
        }
    }

    if(oldLevel!=newLevel){
        if(oldLevel<newLevel){
            if(oldLevel<3 && newLevel >= 3){               
                worldBuilder->addOverlayToVisibleTileUnderground(registry->burrows[burrowIndex].storages[storageIndex].allTiles[tileIndex].pos,RiceGrass_Bundle03,1);
            }

            if(oldLevel<2 && newLevel>=2){
                worldBuilder->addOverlayToVisibleTileUnderground(registry->burrows[burrowIndex].storages[storageIndex].allTiles[tileIndex].pos,RiceGrass_Bundle02,1);
            }

            if(oldLevel<1 && newLevel>=1){
                worldBuilder->addOverlayToVisibleTileUnderground(registry->burrows[burrowIndex].storages[storageIndex].allTiles[tileIndex].pos,RiceGrass_Bundle01,1);
                //worker->consolePrint("pos: "+QString::number(registry->burrows[burrowIndex].storages[storageIndex].allTiles[tileIndex].pos.x())+","+QString::number(registry->burrows[burrowIndex].storages[storageIndex].allTiles[tileIndex].pos.y()));
                //worker->consolePrint("tiletype: "+QString::number(RiceGrass_Bundle01));
                //worker->consolePrint("variation: "+QString::number(1));
            }
        }else{
            if(newLevel<3 && oldLevel >= 3){
                worldBuilder->removeOverlayFromVisibleTileUnderground(registry->burrows[burrowIndex].storages[storageIndex].allTiles[tileIndex].pos,RiceGrass_Bundle03,1);
            }

            if(newLevel<2 && oldLevel>=2){
                worldBuilder->removeOverlayFromVisibleTileUnderground(registry->burrows[burrowIndex].storages[storageIndex].allTiles[tileIndex].pos,RiceGrass_Bundle02,1);
            }

            if(newLevel<1 && oldLevel>=1){
                worldBuilder->removeOverlayFromVisibleTileUnderground(registry->burrows[burrowIndex].storages[storageIndex].allTiles[tileIndex].pos,RiceGrass_Bundle01,1);
            }
        }

        if(index!=-1){
            storageHerdFUCK[index].level = newLevel;
        }else{
            worker->consolePrint("Error updateStorageDisplay");
        }

        worldBuilder->prepareToUpdateTiles(worldBuilder->worldPosition,worldBuilder->underground);
    }*/
}

void Herder::removeStorageTileFromHerd(){
    QVector<int> toBeRemoved;

    for (int i=0;i<storageHerdFUCK.length();i++){
        QPoint p = storageHerdFUCK.value(i).pos-worldBuilder->worldPosition;

        if (p.x() > 15 || p.x() < -16 || p.y() > 15 || p.y() < -16){
            toBeRemoved.append(i);
        }

    }

    for (int k=toBeRemoved.length();k>0;k--){

        int burrowIndex = registry->getBurrowIndex(storageHerdFUCK.value(toBeRemoved.value(k-1)).burrowId);
        bool done = false;
        for (int i=0;i<registry->burrows.value(burrowIndex).storages.length()&&!done;i++){
            for (int j=0;j<registry->burrows.value(burrowIndex).storages.value(i).allTiles.length()&&!done;j++){
                if(storageHerdFUCK.value(toBeRemoved.value(k-1)).pos == registry->burrows.value(burrowIndex).storages.value(i).allTiles.value(j).pos){
                    registry->burrows[burrowIndex].storages[i].allTiles[j].visible = false;
                    done=true;
                }
            }
        }

        storageHerdFUCK.remove(toBeRemoved.value(k-1));
        //console->print("storage tile removed from herd");
    }



    /*  int storageId = hwRegistry->getStorageId(pos, burrowId);
    bool alreadyAdded = false;
    if (storageId !=-1){
        for (int i=0;i<storageHerd.length();i++){
            if(storageId == storageHerd.value(i)){
                alreadyAdded = true;
                break;
            }
        }

        if (!alreadyAdded){
            storageHerd.append(storageId);
            int burrowIndex = hwRegistry->getBurrowIndex(burrowId);
            int storageIndex = hwRegistry->getStorageIndex(burrowIndex,storageId);
            hwRegistry->burrows[burrowIndex].storages[storageIndex].visible = true;
        }

    }else{
        console->print("Error addStorageToHerd");
    }*/
}

void Herder::removeAllStorageTilesFromHerd(){
    QVector<int> toBeRemoved;

    for (int i=0;i<storageHerdFUCK.length();i++){
            toBeRemoved.append(i);
    }

    for (int k=toBeRemoved.length();k>0;k--){
        int burrowIndex = registry->getBurrowIndex(storageHerdFUCK.value(toBeRemoved.value(k-1)).burrowId);

        for (int i=0;i<registry->burrows.value(burrowIndex).storages.length();i++){
            for (int j=0;j<registry->burrows.value(burrowIndex).storages.value(i).allTiles.length();j++){
                if(storageHerdFUCK.value(toBeRemoved.value(k-1)).pos == registry->burrows.value(burrowIndex).storages.value(i).allTiles.value(j).pos){
                    registry->burrows[burrowIndex].storages[i].allTiles[j].visible = false;
                }
            }
        }

        storageHerdFUCK.remove(toBeRemoved.value(k-1));
        //console->print("all storage tile removed from herd");
    }
}

void Herder::taunt(){
    for (int i=0;i<wombatHerd.length();i++){
        if(wombatHerd.value(i)->currentTask == WombatAITask_Sleeping){
            QPoint diff = worldBuilder->worldPosition - wombatHerd.value(i)->oldTilePos;
            if(diff.manhattanLength()<2){
                wombatHerd[i]->wakeUp();
            }
        }
    }

}
