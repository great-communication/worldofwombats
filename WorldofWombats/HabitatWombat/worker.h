//This object lives in the 2nd thread and creates and handles the entire game back end.
//The front end and back end is completely separate,
//and can only communicate through Signal and Slots sent between
//the controller and worker

#ifndef WORKER_H
#define WORKER_H

#pragma warning (disable: 4100) //Disable compiler warning message "unreferenced formal parameter"

#include <QObject>
#include "global.h"
#include "worldbuilder.h"  // Move to cpp file?

class Global;
class WorldBuilder;

class Worker : public QObject
{
    Q_OBJECT

public:
    Global *global;
    WorldBuilder *worldBuilder;

    //Functions to send signals
    void toController();
    void consolePrint(const QString &output);
    void playSoundFX(SoundFX sound);

    //Called from WorldBuilder
    void updateTexture(QPoint positionsChanged, const QImage &backgroundImage, bool underground);
    void updateNeighboursArray(const QVector<MoveStruct> &neighbours);

    //Called from Registry
    void addWombatToDisplay(int wombatId, QPoint tilePos, QPoint startingOffsetPx, qreal rotation, bool underground, int age, WombatAITask currentTask);
    void addPosToWombatPosBuffer(int wombatId, QPoint pxChange, qreal rotation, bool underground, int age);
    void setLockMovement(bool b);
    void setLockInput(bool b);
    void setSelectedWombatVisibility(bool b);
    void displayWombatInfo(QVector<QString> strings, int wombatId, bool setPossessButtonVisible);
    void displayTileInfo(QVector<QString> strings);
    void setUnderground(bool b);
    void setViewPosition(QPoint pos);
    void setXYMoved(QPoint moved);
    void setSelectedWombatId(int wombatId);
    void setSelectedWombatRotation(qreal rotation);
    void setScrollTarget(int wombatId);
    void setUpdatingTiles(bool b);
    void displayBurrowList(QVector<QString> strings, QVector<int> burrowIds, QVector<int> selectedBurrowWombatIds);
    void showDeathNotice(QString wombatName, int wombatAge, int wombatCount, bool gameOver, CauseOfDeath causeOfDeath);
    void setLocationText(QString string);
    void displayOutOfRangeMessage(QPoint pos);
    void returnStorageInfoForShowStorageWidget(int burrowId, int storageId, QVector<bool> acceptedResources);
    void stopCollectResource();


    //Called from Herder
    void removeWombatFromDisplay(int wombatId);

    //Called from HerdMember
    void updateWombatSize(int index, int age);
    void setWombatTask(int index, WombatAITask currentTask);
    void setWombatEmoticon(int index, WombatEmoticon emoticon);

public slots:
    void fromController(); //Used fof debug
    void fromControllerWithParameters(QPoint pos); //Used for debug
    void setup(QString levelName, QSize backgroundSize);
    void updateBackgroundSize(QSize backgroundSize, bool underground);
    void startUpdate(QPoint viewPosition, bool underground);
    void changeTerrain(QPoint pos, Terrain terrain, int burrowId, bool update);
    void addOverlay(QPoint pos, Overlay overlay, int variation);
    void quitGame(QPoint viewPosition);
    void prepareWombatInfoForDisplay(int wombatId);
    void prepareTileInfoForDisplay(QPoint pos);
    void possess(int wombatId, QPoint viewPosition, QPoint moved, bool underground, qreal rotation);
    void scrollToTargetPosDone();
    void setSelectedWombatInRegistryUnderground(bool b);
    void getRandomBurrowName();
    void createBurrow(QPoint pos, QString burrowName);
    void prepareBurrowListForDisplay(int selectedBurrowId);
    void changeHomeBurrow(QPoint pos);
    void getBurrowName(QPoint pos);
    void addPosToBurrow(QPoint posBurrow, QPoint newPos);
    void getRandomWombatIdForPossess();
    void claimResource(QPoint pos, TileType tileType);
    void getWombatInventory(int wombatId);
    void getStorageInventory(int burrowId, int storageId);
    void addBuilding(QPoint pos,TileType buildingType,int variation);
    void moveItemFromWombat(InventoryItem item, int wombatId, int burrowId, int storageId, QPoint draggedLabelPos);
    void moveItemFromStorage(InventoryItem item, int wombatId, int burrowId, int storageId, QPoint draggedLabelPos);
    void setWInventoryWidgetData(bool wInventoryWidgetOpen, int wombatId);
    void setStorageWidgetData(bool storageWidgetOpen, int storageId);
    void prepareStorageInfoForShowStorageWidget(QPoint pos);
    void taunt();
    void setInfoBoxOpen(bool b);
    void findPathDebug(PathPoint startPos, PathPoint endPos);
    void removeItemFromWombatInventory(int wombatId, InventoryItem item);
    void removeItemFromStorage(int burrowId, int storageId, InventoryItem item);
    void addTunnelsToBurrow(int burrowId, QVector<QPoint> startPos);
    void setAcceptedResourcesInStorage(int burrowId, int storageId, QVector<bool> acceptedResources);
    void showOwner(int burrowId);
    void hideOwner(int burrowId);
    void setMovingInOpenGl(int wombatId, bool b);
    void getNameWombatId(int wombatId);
    void printAllWombatsToConsole();
    void collectResource(QPoint pos,TileType tileType);

signals:
    void s_toController();
    void s_consolePrint(const QString &output);
    void s_updateTexture(QPoint positionsChanged, const QImage &backgroundImage, bool underground);
    void s_updateNeighboursArray(const QVector<MoveStruct> &neighbours);

    void s_addWombatToDisplay(int wombatId, QPoint tilePos, QPoint startingOffsetPx, qreal rotation, bool underground, int age, WombatAITask currentTask);
    void s_addPosToWombatPosBuffer(int wombatId, QPoint pxChange, qreal rotation, bool underground, int age);
    void s_setLockMovement(bool b);
    void s_setLockInput(bool b);
    void s_setSelectedWombatVisibility(bool b);
    void s_displayWombatInfo(QVector<QString> strings, int wombatId, bool setPossessButtonVisible);
    void s_displayTileInfo(QVector<QString> strings);
    void s_removeWombatFromDisplay(int wombatId);
    void s_setUnderground(bool b);
    void s_setViewPosition(QPoint pos);
    void s_setXYMoved(QPoint moved);
    void s_setSelectedWombatId(int wombatId);
    void s_setSelectedWombatRotation(qreal rotation);
    void s_setScrollTarget(int wombatId);
    void s_setUpdatingTiles(bool b);
    void s_returnRandomBurrowName(QString burrowName);
    void s_displayBurrowList(QVector<QString> strings, QVector<int> burrowIds, QVector<int> selectedBurrowWombatIds);
    void s_setLocationText(QString burrowName);
    void s_returnRandomWombatIdForPossess(int wombatId);
    void s_showDeathNotice(QString wombatName, int wombatAge, int wombatCount, bool gameOver, CauseOfDeath causeOfDeath);
    void s_playSoundFX(SoundFX sound);
    void s_returnWombatInventory(const QVector<InventoryItem> &inventory, int selectedWombat);
    void s_returnStorageInventory(const QVector<InventoryItem> &inventory, int selectedWombat);
    void s_displayOutOfRangeMessage(QPoint pos);
    void s_returnStorageInfoForShowStorageWidget(int burrowId, int storageId, QVector<bool> acceptedResources);
    void s_updateWombatSize(int index, int age);
    void s_setWombatTask(int index, WombatAITask currentTask);
    void s_setWombatEmoticon(int index, WombatEmoticon emoticon);
    void s_stopCollectResource();

private:


};

#endif // WORKER_H
