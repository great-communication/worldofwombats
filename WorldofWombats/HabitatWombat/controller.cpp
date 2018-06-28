#include "controller.h"
#include "gui.h"
#include "opengl.h"
#include "sound.h"
#include "Widgets/Game/console.h"
#include "Widgets/Game/burrowlist.h"
#include "Widgets/Game/createburrow.h"
#include "Widgets/Game/storagewidget.h"
#include "Widgets/Game/winventorywidget.h"


Controller::Controller() {
    Worker *worker = new Worker;
    worker->moveToThread(&workerThread);
    connect(&workerThread, &QThread::finished, worker, &QObject::deleteLater);
    connect(this, &Controller::s_toWorker, worker, &Worker::fromController);
    connect(this, &Controller::s_toWorkerWithParameters, worker, &Worker::fromControllerWithParameters);
    connect(this, &Controller::s_setup, worker, &Worker::setup);
    connect(this, &Controller::s_updateBackgroundSize, worker, &Worker::updateBackgroundSize);
    connect(this, &Controller::s_startUpdate, worker, &Worker::startUpdate);
    connect(this, &Controller::s_changeTerrain, worker, &Worker::changeTerrain);
    connect(this, &Controller::s_addOverlay, worker, &Worker::addOverlay);
    connect(this, &Controller::s_quitGame, worker, &Worker::quitGame);
    connect(this, &Controller::s_prepareWombatInfoForDisplay, worker, &Worker::prepareWombatInfoForDisplay);
    connect(this, &Controller::s_prepareTileInfoForDisplay, worker, &Worker::prepareTileInfoForDisplay);
    connect(this, &Controller::s_possess, worker, &Worker::possess);
    connect(this, &Controller::s_scrollToTargetPosDone, worker, &Worker::scrollToTargetPosDone);
    connect(this, &Controller::s_setSelectedWombatInRegistryUnderground, worker, &Worker::setSelectedWombatInRegistryUnderground);
    connect(this, &Controller::s_getRandomBurrowName, worker, &Worker::getRandomBurrowName);
    connect(this, &Controller::s_createBurrow, worker, &Worker::createBurrow);
    connect(this, &Controller::s_prepareBurrowListForDisplay, worker, &Worker::prepareBurrowListForDisplay);
    connect(this, &Controller::s_changeHomeBurrow, worker, &Worker::changeHomeBurrow);
    connect(this, &Controller::s_getBurrowName, worker, &Worker::getBurrowName);
    connect(this, &Controller::s_addPosToBurrow, worker, &Worker::addPosToBurrow);
    connect(this, &Controller::s_getRandomWombatIdForPossess, worker, &Worker::getRandomWombatIdForPossess);
    connect(this, &Controller::s_claimResource, worker, &Worker::claimResource);
    connect(this, &Controller::s_collectResource, worker, &Worker::collectResource);
    connect(this, &Controller::s_getWombatInventory, worker, &Worker::getWombatInventory);
    connect(this, &Controller::s_getStorageInventory, worker, &Worker::getStorageInventory);
    connect(this, &Controller::s_addBuilding, worker, &Worker::addBuilding);
    connect(this, &Controller::s_moveItemFromWombat, worker, &Worker::moveItemFromWombat);
    connect(this, &Controller::s_moveItemFromStorage, worker, &Worker::moveItemFromStorage);
    connect(this, &Controller::s_setWInventoryWidgetData, worker, &Worker::setWInventoryWidgetData);
    connect(this, &Controller::s_setStorageWidgetData, worker, &Worker::setStorageWidgetData);
    connect(this, &Controller::s_prepareStorageInfoForShowStorageWidget, worker, &Worker::prepareStorageInfoForShowStorageWidget);
    connect(this, &Controller::s_taunt, worker, &Worker::taunt);
    connect(this, &Controller::s_setInfoBoxOpen, worker, &Worker::setInfoBoxOpen);
    connect(this, &Controller::s_findPathDebug, worker, &Worker::findPathDebug);
    connect(this, &Controller::s_removeItemFromWombatInventory, worker, &Worker::removeItemFromWombatInventory);
    connect(this, &Controller::s_removeItemFromStorage, worker, &Worker::removeItemFromStorage);
    connect(this, &Controller::s_addTunnelsToBurrow, worker, &Worker::addTunnelsToBurrow);
    connect(this, &Controller::s_setAcceptedResourcesInStorage, worker, &Worker::setAcceptedResourcesInStorage);
    connect(this, &Controller::s_showOwner, worker, &Worker::showOwner);
    connect(this, &Controller::s_hideOwner, worker, &Worker::hideOwner);
    connect(this, &Controller::s_setMovingInOpenGl, worker, &Worker::setMovingInOpenGl);
    connect(this, &Controller::s_getNameWombatId, worker, &Worker::getNameWombatId);
    connect(this, &Controller::s_printAllWombatsToConsole, worker, &Worker::printAllWombatsToConsole);

    connect(worker, &Worker::s_toController, this, &Controller::fromWorker);
    connect(worker, &Worker::s_consolePrint, this, &Controller::consolePrint);
    connect(worker, &Worker::s_playSoundFX, this, &Controller::playSoundFX);
    connect(worker, &Worker::s_updateTexture, this, &Controller::updateTexture);   
    connect(worker, &Worker::s_updateNeighboursArray, this, &Controller::updateNeighboursArray);
    connect(worker, &Worker::s_addWombatToDisplay, this, &Controller::addWombatToDisplay);
    connect(worker, &Worker::s_addPosToWombatPosBuffer, this, &Controller::addPosToWombatPosBuffer);
    connect(worker, &Worker::s_setLockMovement, this, &Controller::setLockMovement);
    connect(worker, &Worker::s_setLockInput, this, &Controller::setLockInput);
    connect(worker, &Worker::s_setSelectedWombatVisibility, this, &Controller::setSelectedWombatVisibility);
    connect(worker, &Worker::s_displayWombatInfo, this, &Controller::displayWombatInfo);
    connect(worker, &Worker::s_displayTileInfo, this, &Controller::displayTileInfo);
    connect(worker, &Worker::s_removeWombatFromDisplay, this, &Controller::removeWombatFromDisplay);
    connect(worker, &Worker::s_setUnderground, this, &Controller::setUnderground);
    connect(worker, &Worker::s_setViewPosition, this, &Controller::setViewPosition);
    connect(worker, &Worker::s_setXYMoved, this, &Controller::setXYMoved);
    connect(worker, &Worker::s_setSelectedWombatId, this, &Controller::setSelectedWombatId);
    connect(worker, &Worker::s_setSelectedWombatRotation, this, &Controller::setSelectedWombatRotation);
    connect(worker, &Worker::s_setScrollTarget, this, &Controller::setScrollTarget);
    connect(worker, &Worker::s_returnRandomBurrowName, this, &Controller::returnRandomBurrowName);
    connect(worker, &Worker::s_displayBurrowList, this, &Controller::displayBurrowList);
    connect(worker, &Worker::s_setLocationText, this, &Controller::setLocationText);
    connect(worker, &Worker::s_returnRandomWombatIdForPossess, this, &Controller::returnRandomWombatIdForPossess);
    connect(worker, &Worker::s_showDeathNotice, this, &Controller::showDeathNotice);
    connect(worker, &Worker::s_returnWombatInventory, this, &Controller::returnWombatInventory);
    connect(worker, &Worker::s_returnStorageInventory, this, &Controller::returnStorageInventory);
    connect(worker, &Worker::s_displayOutOfRangeMessage, this, &Controller::displayOutOfRangeMessage);
    connect(worker, &Worker::s_returnStorageInfoForShowStorageWidget, this, &Controller::returnStorageInfoForShowStorageWidget);
    connect(worker, &Worker::s_updateWombatSize, this, &Controller::updateWombatSize);
    connect(worker, &Worker::s_setWombatTask, this, &Controller::setWombatTask);
    connect(worker, &Worker::s_setWombatEmoticon, this, &Controller::setWombatEmoticon);
    connect(worker, &Worker::s_stopCollectResource, this, &Controller::stopCollectResource);

    workerThread.start();
}

Controller::~Controller() {
    workerThread.quit();
    workerThread.wait();
}

//Signals sent to worker thread
void Controller::toWorker(){
    emit s_toWorker();
}

void Controller::toWorkerWithParameters(QPoint pos){
    emit s_toWorkerWithParameters(pos);
}

void Controller::setupWorker(QString levelName, QSize backgroundSize){
    emit s_setup(levelName,backgroundSize);
}

void Controller::updateBackgroundSize(QSize backgroundSize, bool underground){
    emit s_updateBackgroundSize(backgroundSize, underground);
}

void Controller::startUpdate(QPoint viewPosition, bool underground){
    emit s_startUpdate(viewPosition, underground);
}

void Controller::changeTerrain(QPoint pos, Terrain terrain, int burrowId, bool update){
    emit s_changeTerrain(pos, terrain, burrowId, update);
}

void Controller::addOverlay(QPoint pos, Overlay overlay, int variation){
    emit s_addOverlay(pos, overlay, variation);
}

void Controller::quitGame(QPoint viewPosition){
    emit s_quitGame(viewPosition);
}


void Controller::prepareWombatInfoForDisplay(int wombatId){
    emit s_prepareWombatInfoForDisplay(wombatId);
}

void Controller::prepareTileInfoForDisplay(QPoint pos){
    emit s_prepareTileInfoForDisplay(pos);
}

void Controller::possess(int wombatId, QPoint viewPosition, QPoint moved, bool underground, qreal rotation){
    emit s_possess(wombatId, viewPosition, moved, underground, rotation);
}

void Controller::scrollToTargetPosDone(){
    emit s_scrollToTargetPosDone();
}

void Controller::setSelectedWombatInRegistryUnderground(bool b){
    emit s_setSelectedWombatInRegistryUnderground(b);
}

void Controller::getRandomBurrowName(){
    emit s_getRandomBurrowName();
}

void Controller::createBurrow(QPoint pos, QString burrowName){
    emit s_createBurrow(pos, burrowName);
}

void Controller::prepareBurrowListForDisplay(int selectedBurrowId){
    emit s_prepareBurrowListForDisplay(selectedBurrowId);
}

void Controller::changeHomeBurrow(QPoint pos){
    emit s_changeHomeBurrow(pos);
}

void Controller::getBurrowName(QPoint pos){
    emit s_getBurrowName(pos);
}

void Controller::addPosToBurrow(QPoint posBurrow, QPoint newPos){
    emit s_addPosToBurrow(posBurrow,newPos);
}

void Controller::getRandomWombatIdForPossess(){
    emit s_getRandomWombatIdForPossess();
}

void Controller::claimResource(QPoint pos, TileType tileType){
    emit s_claimResource(pos,tileType);
}

void Controller::collectResource(QPoint pos, TileType tileType){
    emit s_collectResource(pos, tileType);
}

void Controller::getWombatInventory(int wombatId){
    emit s_getWombatInventory(wombatId);
}

void Controller::getStorageInventory(int burrowId, int storageId){
    emit s_getStorageInventory(burrowId, storageId);
}

void Controller::addBuilding(QPoint pos,TileType buildingType,int variation){
    emit s_addBuilding(pos,buildingType,variation);
}

void Controller::moveItemFromWombat(InventoryItem item, int wombatId, int burrowId, int storageId, QPoint draggedLabelPos){
    emit s_moveItemFromWombat(item,wombatId,burrowId,storageId,draggedLabelPos);
}

void Controller::moveItemFromStorage(InventoryItem item, int wombatId, int burrowId, int storageId, QPoint draggedLabelPos){
    emit s_moveItemFromStorage(item,wombatId,burrowId,storageId,draggedLabelPos);
}

void Controller::setWInventoryWidgetData(bool wInventoryWidgetOpen, int wombatId){
    emit s_setWInventoryWidgetData(wInventoryWidgetOpen, wombatId);
}

void Controller::setStorageWidgetData(bool storageWidgetOpen, int storageId){
    emit s_setStorageWidgetData(storageWidgetOpen, storageId);
}

void Controller::prepareStorageInfoForShowStorageWidget(QPoint pos){
    emit s_prepareStorageInfoForShowStorageWidget(pos);
}

void Controller::setInfoBoxOpen(bool b){
    emit s_setInfoBoxOpen(b);
}

void Controller::findPathDebug(PathPoint startPos, PathPoint endPos){
    emit s_findPathDebug(startPos,endPos);
}

void Controller::removeItemFromWombatInventory(int wombatId, InventoryItem item){
    emit s_removeItemFromWombatInventory(wombatId, item);
}

void Controller::removeItemFromStorage(int burrowId, int storageId, InventoryItem item){
    emit s_removeItemFromStorage(burrowId, storageId, item);
}

void Controller::addTunnelsToBurrow(int burrowId, QVector<QPoint> startPos){
    emit s_addTunnelsToBurrow(burrowId, startPos);
}

void Controller::setAcceptedResourcesInStorage(int burrowId, int storageId, QVector<bool> acceptedResources){
    emit s_setAcceptedResourcesInStorage(burrowId,storageId,acceptedResources);
}

void Controller::showOwner(int burrowId){
    emit s_showOwner(burrowId);
}

void Controller::hideOwner(int burrowId){
    emit s_hideOwner(burrowId);
}

void Controller::getNameWombatId(int wombatId){
    emit s_getNameWombatId(wombatId);
}

void Controller::printAllWombatsToConsole(){
    emit s_printAllWombatsToConsole();
}

//To Herder
void Controller::taunt(){
    emit s_taunt();
}

//To HerdMember
void Controller::setMovingInOpenGl(int wombatId, bool b){
    emit s_setMovingInOpenGl(wombatId, b);
}


//Slots called from worker thread
void Controller::fromWorker(){
    //Temp code remove
    //startUpdate(hwGameUI->viewPosition);
    //workerThread.sleep(5);
}

void Controller::consolePrint(const QString &output){
    console->print(output);
}

void Controller::playSoundFX(SoundFX sound){
    gui->sound->playSoundFX(sound);
}

//From WorldBuilder
void Controller::updateTexture(QPoint positionsChanged, const QImage &backgroundImage, bool underground){
    openGL->updateTexture(positionsChanged, backgroundImage, underground);
}

void Controller::updateNeighboursArray(const QVector<MoveStruct> &neighbours){
    gui->updateNeighboursArray(neighbours);
}

//From Registry
void Controller::addWombatToDisplay(int wombatId, QPoint tilePos, QPoint startingOffsetPx, qreal rotation, bool underground, int age, WombatAITask currentTask){
    openGL->addWombatToDisplay(wombatId, tilePos, startingOffsetPx, rotation, underground, age, currentTask);
}

void Controller::addPosToWombatPosBuffer(int wombatId, QPoint pxChange, qreal rotation, bool underground, int age){
    //openGL->updateWombatPosition(index, tilePos, rotation, underground, age);
    openGL->addPosToWombatPosBuffer(wombatId, pxChange, rotation, underground, age);
}

void Controller::setLockMovement(bool b){
    gui->setLockMovement(b);
}

void Controller::setLockInput(bool b){
    gui->setLockInput(b);
}

void Controller::setSelectedWombatVisibility(bool b){
    openGL->setSelectedWombatVisibility(b);
}

void Controller::displayWombatInfo(QVector<QString> strings, int wombatId, bool setPossessButtonVisible){
    gui->displayWombatInfo(strings, wombatId, setPossessButtonVisible);
}

void Controller::displayTileInfo(QVector<QString> strings){
    gui->displayTileInfo(strings);
}

void Controller::setUnderground(bool b){
    gui->setUnderground(b);
}

void Controller::setViewPosition(QPoint pos){
    gui->setViewPosition(pos);
}

void Controller::setXYMoved(QPoint moved){
    gui->setXYMoved(moved);
}

void Controller::setSelectedWombatId(int wombatId){
    openGL->setSelectedWombatId(wombatId);
}

void Controller::setSelectedWombatRotation(qreal rotation){
    openGL->setSelectedWombatRotation(rotation);
}

void Controller::setScrollTarget(int wombatId){
    gui->setScrollTarget(wombatId);
}

void Controller::setUpdatingTiles(bool b){
    gui->setUpdatingTiles(b);
}

void Controller::returnRandomBurrowName(QString burrowName){
    gui->createBurrowDialog->setburrowLineEditText(burrowName);
}

void Controller::displayBurrowList(QVector<QString> strings, QVector<int> burrowIds, QVector<int> selectedBurrowWombatIds){
    gui->burrowList->displayBurrowList(strings,burrowIds,selectedBurrowWombatIds);
}

void Controller::setLocationText(QString string){
    gui->setLocationText(string);
}

void Controller::returnRandomWombatIdForPossess(int wombatId){
    gui->possess(wombatId);
}

void Controller::showDeathNotice(QString wombatName, int wombatAge, int wombatCount, bool gameOver, CauseOfDeath causeOfDeath){
    gui->showDeathNotice(wombatName, wombatAge, wombatCount, gameOver, causeOfDeath);
}

void Controller::returnWombatInventory(const QVector<InventoryItem> &inventory, int selectedWombat){
    gui->wInventoryWidget->update(inventory, selectedWombat);
}

void Controller::returnStorageInventory(const QVector<InventoryItem> &inventory, int precentageFull){
    gui->storageWidget->update(inventory, precentageFull);
}

void Controller::displayOutOfRangeMessage(QPoint pos){
    gui->displayOutOfRangeMessage(pos);
}

void Controller::returnStorageInfoForShowStorageWidget(int burrowId, int storageId, QVector<bool> acceptedResources){
    gui->closeStorageWidget();
    gui->showStorageWidget(burrowId,storageId,acceptedResources);
}

void Controller::stopCollectResource(){
    gui->stopCollectResource();
}

//From Herder
void Controller::removeWombatFromDisplay(int wombatId){
    openGL->removeWombatFromDisplay(wombatId);
}

//From HerdMember
void Controller::updateWombatSize(int index, int age){
    openGL->updateWombatSize(index, age);
}

void Controller::setWombatTask(int index, WombatAITask currentTask){
    openGL->setWombatTask(index, currentTask);
}

void Controller::setWombatEmoticon(int index, WombatEmoticon emoticon){
    openGL->setWombatEmoticon(index, emoticon);
}
