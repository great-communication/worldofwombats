#include "worker.h"
#include "herdmember.h"
#include "registry.h"
#include "herder.h"
#include "imagepainter.h"


void Worker::setup(QString levelName, QSize backgroundSize) {
    global = new Global(this);  // Is it safe to have "this" as parent!?
    global->setup();

    worldBuilder = new WorldBuilder(this);
    worldBuilder->worker = this;
    worldBuilder->global = global;
    worldBuilder->setup(levelName, backgroundSize);

    consolePrint("Setup done");

    //QTimer *hej = new QTimer(this);
    //connect(hej, SIGNAL(timeout()), this, SLOT(test()));
    //hej->start(500);
}

//Signals sent to controller
void Worker::toController() {
    emit s_toController();
}

void Worker::consolePrint(const QString &output) {
    emit s_consolePrint(output);
}

void Worker::playSoundFX(SoundFX sound){
    emit s_playSoundFX(sound);
}

//From WorldBuilder
void Worker::updateTexture(QPoint positionsChanged, const QImage &backgroundImage, bool underground) {
    emit s_updateTexture(positionsChanged, backgroundImage, underground);
}

void Worker::updateNeighboursArray(const QVector<MoveStruct> &neighbours) {
    emit s_updateNeighboursArray(neighbours);
}

//From Registry
void Worker::addWombatToDisplay(int wombatId, QPoint tilePos, QPoint startingOffsetPx, qreal rotation, bool underground, int age, WombatAITask currentTask){
    emit s_addWombatToDisplay(wombatId, tilePos, startingOffsetPx, rotation, underground, age, currentTask);
}

void Worker::addPosToWombatPosBuffer(int wombatId, QPoint pxChange, qreal rotation, bool underground, int age){
    emit s_addPosToWombatPosBuffer(wombatId, pxChange, rotation, underground, age);
}

void Worker::setLockMovement(bool b){
    emit s_setLockMovement(b);
}

void Worker::setLockInput(bool b){
    emit s_setLockInput(b);
}

void Worker::setSelectedWombatVisibility(bool b){
    emit s_setSelectedWombatVisibility(b);
}

void Worker::displayWombatInfo(QVector<QString> strings, int wombatId, bool setPossessButtonVisible){
    emit s_displayWombatInfo(strings, wombatId, setPossessButtonVisible);
}

void Worker::displayTileInfo(QVector<QString> strings){
    emit s_displayTileInfo(strings);
}

void Worker::setUnderground(bool b){
    emit s_setUnderground(b);
}

void Worker::setViewPosition(QPoint pos){
    emit s_setViewPosition(pos);
}

void Worker::setXYMoved(QPoint moved){
    emit s_setXYMoved(moved);
}

void Worker::setSelectedWombatId(int wombatId){
    emit s_setSelectedWombatId(wombatId);
}

void Worker::setSelectedWombatRotation(qreal rotation){
    emit s_setSelectedWombatRotation(rotation);
}

void Worker::setScrollTarget(int wombatId){
    emit s_setScrollTarget(wombatId);
}

void Worker::setUpdatingTiles(bool b){
    emit s_setUpdatingTiles(b);
}

void Worker::displayBurrowList(QVector<QString> strings, QVector<int> burrowIds, QVector<int> selectedBurrowWombatIds){
    emit s_displayBurrowList(strings, burrowIds, selectedBurrowWombatIds);
}

void Worker::showDeathNotice(QString wombatName, int wombatAge, int wombatCount, bool gameOver, CauseOfDeath causeOfDeath){
    emit s_showDeathNotice(wombatName, wombatAge, wombatCount, gameOver, causeOfDeath);
}

void Worker::setLocationText(QString string){
    emit s_setLocationText(string);
}

void Worker::displayOutOfRangeMessage(QPoint pos){
    emit s_displayOutOfRangeMessage(pos);
}

void Worker::returnStorageInfoForShowStorageWidget(int burrowId, int storageId, QVector<bool> acceptedResources){
    emit s_returnStorageInfoForShowStorageWidget(burrowId, storageId, acceptedResources);
}

void Worker::setAcceptedResourcesInStorage(int burrowId, int storageId, QVector<bool> acceptedResources){
    worldBuilder->registry->setAcceptedResourcesInStorage(burrowId,storageId,acceptedResources);
}

void Worker::stopCollectResource(){
    emit s_stopCollectResource();
}

//From Herder
void Worker::removeWombatFromDisplay(int wombatId){
    emit s_removeWombatFromDisplay(wombatId);
}

//From HerdMember
void Worker::updateWombatSize(int index, int age){
    emit s_updateWombatSize(index, age);
}

void Worker::setWombatTask(int index, WombatAITask currentTask){
    emit s_setWombatTask(index,currentTask);
}

void Worker::setWombatEmoticon(int index, WombatEmoticon emoticon){
    emit s_setWombatEmoticon(index, emoticon);
}




//Slots called from controller
void Worker::fromController() {
    //For debugging

    //QFile file("yourFile.png");
    //file.open(QIODevice::WriteOnly);
    //worldBuilder->croppedPaths[6].save(&file, "PNG");

    //consolePrint("wombats in registry: "+QString::number(worldBuilder->registry->wombats.length()));
    //consolePrint("wombatherd: "+QString::number(worldBuilder->herder->wombatHerd.length()));

/*
    QVector<int> killList;
    QVector<CauseOfDeath> causeOfDeath;
    killList.append(0);
    causeOfDeath.append(CauseOfDeath_OldAge);
    worldBuilder->registry->killWombats(killList,causeOfDeath);
*/

    //consolePrint("entrancelenght: "+QString::number(worldBuilder->registry->burrows.value(0).entrancePos.length()));
    //consolePrint("entrances: "+QString::number(worldBuilder->registry->burrows.value(0).entrances));

    /*
    consolePrint("wombatlength: "+QString::number(worldBuilder->registry->wombats.length()));
    consolePrint("herdlength: "+QString::number(worldBuilder->herder->wombatHerd.length()));

    for (int i=0;i<worldBuilder->herder->wombatHerd.length();i++){
        consolePrint("id in herd: "+QString::number(worldBuilder->herder->wombatHerd.value(i)->wombatId));
    }

*/




    /*
    consolePrint("herdlength: "+QString::number(worldBuilder->herder->wombatHerd.length()));
    worldBuilder->herder->removeWombatFromHerd(13);
    consolePrint("herdlength2: "+QString::number(worldBuilder->herder->wombatHerd.length()));
    worldBuilder->herder->removeWombatFromHerd(19);
    consolePrint("herdlength2: "+QString::number(worldBuilder->herder->wombatHerd.length()));*/

    /*
    consolePrint("path empty: "+QString::number(worldBuilder->herder->wombatHerd[0]->currentPath.isEmpty()));
    consolePrint("pathlength: "+QString::number(worldBuilder->herder->wombatHerd[0]->currentPath.length()));

    if(worldBuilder->herder->wombatHerd[0]->currentPath.length()>0){
        consolePrint("firstpos: "+QString::number(worldBuilder->herder->wombatHerd[0]->currentPath.value(0).pos.x())+","+QString::number(worldBuilder->herder->wombatHerd[0]->currentPath.value(0).pos.y()));
    }*/

    //Put wombat to sleep
   // int wombatIdd = worldBuilder->herder->wombatHerd[0]->wombatId;
    //int wombatIndex = worldBuilder->registry->getWombatIndex(wombatIdd);
   // worldBuilder->registry->wombats[wombatIndex].rested = 10;


    //worldBuilder->registry->displayStorageBuffer(0);

    //int hej = worldBuilder->getTileType(QPoint(-2,-2),false);
    //        consolePrint("tileType: "+QString::number(hej));

    //worldBuilder->herder->removeAllWombatsFromHerd();
}

void Worker::fromControllerWithParameters(QPoint pos) {
    //For debugging
    //worldBuilder->world->getTile(pos);

    consolePrint("tile position: "+QString::number(pos.x())+","+QString::number(pos.y()));
    QPoint chunkPosition = worldBuilder->qPointDivisionWithFloor(pos,QPoint(32,32));
    consolePrint("chunk position: "+QString::number(chunkPosition.x())+","+QString::number(chunkPosition.y()));
}

//To WorldBuilder
void Worker::startUpdate(QPoint viewPosition, bool underground) {
    worldBuilder->prepareToUpdateTiles(viewPosition, underground);
}

void Worker::updateBackgroundSize(QSize backgroundSize, bool underground) {
    worldBuilder->imagePainter->updateBackgroundSize(backgroundSize, underground);
}

void Worker::changeTerrain(QPoint pos, Terrain terrain, int burrowId, bool update) {
    worldBuilder->changeTerrain(pos, terrain, burrowId, update);
}

void Worker::addOverlay(QPoint pos, Overlay overlay, int variation){
    worldBuilder->addOverlayToTile(pos, overlay, variation);
}

void Worker::quitGame(QPoint viewPosition){
    worldBuilder->quitGame(viewPosition);
}

//To Registry
void Worker::prepareWombatInfoForDisplay(int wombatId){
    worldBuilder->registry->prepareWombatInfoForDisplay(wombatId);
}

void Worker::prepareTileInfoForDisplay(QPoint pos){
    worldBuilder->registry->prepareTileInfoForDisplay(pos);
}

void Worker::possess(int wombatId, QPoint viewPosition, QPoint moved, bool underground, qreal rotation){
    worldBuilder->registry->possess(wombatId, viewPosition, moved, underground, rotation);
}

void Worker::scrollToTargetPosDone(){
    worldBuilder->registry->scrollToTargetPosDone();
}

void Worker::setSelectedWombatInRegistryUnderground(bool b){
    worldBuilder->registry->setSelectedWombatInRegistryUnderground(b);
}

void Worker::getRandomBurrowName(){
    QString burrowName = worldBuilder->registry->getRandomBurrowName();
    emit s_returnRandomBurrowName(burrowName);
}

void Worker::createBurrow(QPoint pos, QString burrowName){
    worldBuilder->registry->createBurrow(pos, burrowName);
}

void Worker::prepareBurrowListForDisplay(int selectedBurrowId){
    worldBuilder->registry->prepareBurrowListForDisplay(selectedBurrowId);
}

void Worker::changeHomeBurrow(QPoint pos){
    worldBuilder->registry->changeHomeBurrow(pos);
}

void Worker::getBurrowName(QPoint pos){
    QString burrowName = worldBuilder->registry->getBurrowName(pos);
    setLocationText(burrowName);
}

void Worker::addPosToBurrow(QPoint posBurrow, QPoint newPos){
    worldBuilder->registry->addPosToBurrow(posBurrow,newPos);
}

void Worker::getRandomWombatIdForPossess(){
    int wombatId = worldBuilder->registry->getRandomWombatIdForPossess();
    emit s_returnRandomWombatIdForPossess(wombatId);
}

void Worker::claimResource(QPoint pos, TileType tileType){
    worldBuilder->registry->claimResource(pos,tileType);
}

void Worker::getWombatInventory(int wombatId){
    const QVector<InventoryItem> &inventory = worldBuilder->registry->getWombatInventory(wombatId);
    int selectedWombat = worldBuilder->registry->selectedWombat;
    emit s_returnWombatInventory(inventory,selectedWombat);
}

void Worker::getStorageInventory(int burrowId, int storageId){
    int burrowIndex = worldBuilder->registry->getBurrowIndex(burrowId);    
    int storageIndex = worldBuilder->registry->getStorageIndex(burrowIndex,storageId);
    const QVector<InventoryItem> &inventory = worldBuilder->registry->getStorageInventory(burrowIndex,storageIndex);
    //Calculate how full the storage is
    int tilesCount = worldBuilder->registry->burrows[burrowIndex].storages[storageIndex].allTiles.length();
    int totalSize = gStorageSize*tilesCount;
    int usedSize = 0;

    for (int j=0;j<tilesCount;j++){
        for (int k=0;k<worldBuilder->registry->burrows[burrowIndex].storages[storageIndex].allTiles[j].inventory.length();k++){
            usedSize += global->getItemSize(worldBuilder->registry->burrows[burrowIndex].storages[storageIndex].allTiles[j].inventory.value(k));
        }
    }

    int percentageFull = ((double)usedSize/(double)totalSize)*100;

    emit s_returnStorageInventory(inventory,percentageFull);
}

void Worker::addBuilding(QPoint pos,TileType buildingType,int variation){
    //Temp fix
    Overlay building;
    if (buildingType == Storage){
        building = Overlay_Storage;
    }else if(buildingType == Nest){
        building = Overlay_Nest;
    }
    //

    worldBuilder->registry->addBuilding(pos,building,variation);
}

void Worker::moveItemFromWombat(InventoryItem item, int wombatId, int burrowId, int storageId, QPoint draggedLabelPos){
    worldBuilder->registry->moveItemFromWombatToStorage(item, wombatId, burrowId, storageId, draggedLabelPos);
}

void Worker::moveItemFromStorage(InventoryItem item, int wombatId, int burrowId, int storageId, QPoint draggedLabelPos){
    worldBuilder->registry->moveItemFromStorageToWombat(item, wombatId, burrowId, storageId, draggedLabelPos);
}

void Worker::setWInventoryWidgetData(bool wInventoryWidgetOpen, int wombatId){
    worldBuilder->registry->setWInventoryWidgetData(wInventoryWidgetOpen, wombatId);
}

void Worker::setStorageWidgetData(bool storageWidgetOpen, int storageId){
    worldBuilder->registry->setStorageWidgetData(storageWidgetOpen,storageId);
}

void Worker::prepareStorageInfoForShowStorageWidget(QPoint pos){
    worldBuilder->registry->prepareStorageInfoForShowStorageWidget(pos);
}

void Worker::setInfoBoxOpen(bool b){
    worldBuilder->registry->setInfoBoxOpen(b);
}

void Worker::findPathDebug(PathPoint startPos, PathPoint endPos){
    //Print path

    for (int i=0;i<worldBuilder->registry->visited.length();i++){
        if(worldBuilder->registry->visited.value(i).underground){
            worldBuilder->removeOverlayFromTileUnderground(worldBuilder->registry->visited.value(i).pos,Overlay_Cross,1);
        }else{
            worldBuilder->removeOverlayFromTile(worldBuilder->registry->visited.value(i).pos,Overlay_Cross,1);
        }
    }
    consolePrint("hej");


    worldBuilder->registry->findPath(startPos,endPos);
}

void Worker::removeItemFromWombatInventory(int wombatId, InventoryItem item){
    worldBuilder->registry->addItemToWombatInventory(wombatId, item.resource, -item.amount);
}

void Worker::removeItemFromStorage(int burrowId, int storageId, InventoryItem item){
    worldBuilder->registry->removeItemFromStorageOverload(burrowId, storageId, item);
}

void Worker::addTunnelsToBurrow(int burrowId, QVector<QPoint> startPos){
    worldBuilder->registry->addTunnelsToBurrow(burrowId, startPos);
}

void Worker::showOwner(int burrowId){
    worldBuilder->registry->showOwner(burrowId);
}

void Worker::hideOwner(int burrowId){
    worldBuilder->registry->hideOwner(burrowId);
}

void Worker::getNameWombatId(int wombatId){
    QString name = "None found";
    for (int i=0;i<worldBuilder->registry->wombats.length();i++){
        if(wombatId = worldBuilder->registry->wombats.value(i).id){
            name = worldBuilder->registry->wombats.value(i).name;
            break;
        }
    }

    consolePrint("wombatId "+QString::number(wombatId)+"= name:"+name);
}

void Worker::printAllWombatsToConsole(){
    worldBuilder->registry->printAllWombatsToConsole();
}

void Worker::collectResource(QPoint pos, TileType tileType){
    worldBuilder->registry->collectResource(pos,tileType);
}

//To Herder
void Worker::taunt(){
    worldBuilder->herder->taunt();
}

//To Herdmember
void Worker::setMovingInOpenGl(int wombatId, bool b){
    int index = None;

     for (int i=0;i<worldBuilder->herder->wombatHerd.length();i++){
         if(worldBuilder->herder->wombatHerd.value(i)->wombatId == wombatId){
            index = i;
            break;
         }
     }

     if(index!=None){
        worldBuilder->herder->wombatHerd[index]->setMovingInOpenGl(b);
     }
}





