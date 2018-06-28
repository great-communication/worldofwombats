//This is the interface that is controlling the 2nd thread
#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>
#include <QThread>
#include "worker.h"

class Console;
class GUI;
class OpenGL;

class Controller : public QObject
{
    Q_OBJECT
    QThread workerThread;
public:
    Controller();
    ~Controller();
    Console *console;
    GUI *gui;
    OpenGL *openGL;

    //Functions to send signals
    void toWorker(); //Used for debug
    void toWorkerWithParameters(QPoint pos); //Used for debug
    void setupWorker(QString levelName, QSize backgroundSize);

    //To WorldBuilder
    void updateBackgroundSize(QSize backgroundSize, bool underground);
    void startUpdate(QPoint viewPosition, bool underground);
    void changeTerrain(QPoint pos, Terrain tileType, int burrowId, bool update);
    void addOverlay(QPoint pos, Overlay overlay, int variation);
    void quitGame(QPoint viewPosition);

    //To Registry
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
    void collectResource(QPoint pos, TileType tileType);
    void getWombatInventory(int wombatId);
    void getStorageInventory(int burrowId, int storageId);
    void addBuilding(QPoint pos, TileType buildingType, int variation);
    void moveItemFromWombat(InventoryItem item, int wombatId, int burrowId, int storageId, QPoint draggedLabelPos);
    void moveItemFromStorage(InventoryItem item, int wombatId, int burrowId, int storageId, QPoint draggedLabelPos);
    void setWInventoryWidgetData(bool wInventoryWidgetOpen, int wombatId);
    void setStorageWidgetData(bool storageWidgetOpen, int storageId);
    void prepareStorageInfoForShowStorageWidget(QPoint pos);
    void setInfoBoxOpen(bool b);
    void findPathDebug(PathPoint startPos, PathPoint endPos);
    void removeItemFromWombatInventory(int wombatId, InventoryItem item);
    void removeItemFromStorage(int burrowId, int storageId, InventoryItem item);
    void addTunnelsToBurrow(int burrowId, QVector<QPoint> startPos);
    void setAcceptedResourcesInStorage(int burrowId, int storageId, QVector<bool> acceptedResources);
    void showOwner(int burrowId);
    void hideOwner(int burrowId);
    void getNameWombatId(int wombatId);
    void printAllWombatsToConsole();

    //ToHerder
    void taunt();

    //To HerdMember
    void setMovingInOpenGl(int wombatId, bool b);

public slots:
    //Communication from back end
    void fromWorker();
    void consolePrint(const QString &output);
    void playSoundFX(SoundFX sound);

    //From WorldBuilder
    void updateTexture(QPoint positionsChanged, const QImage &backgroundImage, bool underground);
    void updateNeighboursArray(const QVector<MoveStruct> &neighbours);

    //From Registry
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
    void returnRandomBurrowName(QString burrowName);
    void displayBurrowList(QVector<QString> strings, QVector<int> burrowIds, QVector<int> selectedBurrowWombatIds);
    void setLocationText(QString string);
    void returnRandomWombatIdForPossess(int wombatId);
    void showDeathNotice(QString wombatName, int wombatAge, int wombatCount, bool gameOver, CauseOfDeath causeOfDeath);
    void returnWombatInventory(const QVector<InventoryItem> &inventory, int selectedWombat);
    void returnStorageInventory(const QVector<InventoryItem> &inventory, int selectedWombat);
    void returnStorageInfoForShowStorageWidget(int burrowId, int storageId, QVector<bool> acceptedResources);
    void displayOutOfRangeMessage(QPoint pos);   
    void stopCollectResource();

    //From Herder
    void removeWombatFromDisplay(int wombatId);

    //From HerdMember
    void updateWombatSize(int index, int age);
    void setWombatTask(int index, WombatAITask currentTask);
    void setWombatEmoticon(int index, WombatEmoticon emoticon);

signals:
    //Communication to back end
    void s_toWorker();
    void s_toWorkerWithParameters(QPoint pos);
    void s_setup(QString levelName, QSize backgroundSize);

    //To WorldBuilder
    void s_updateBackgroundSize(QSize backgroundSize, bool underground);
    void s_startUpdate(QPoint viewPosition, bool underground);
    void s_changeTerrain(QPoint pos, Terrain terrain, int burrowId, bool update);
    void s_addOverlay(QPoint pos, Overlay overlay, int variation);
    void s_quitGame(QPoint viewPosition);

    //To Registry
    void s_prepareWombatInfoForDisplay(int wombatId);
    void s_prepareTileInfoForDisplay(QPoint pos);
    void s_possess(int wombatId, QPoint viewPosition, QPoint moved, bool underground, qreal rotation);
    void s_scrollToTargetPosDone();
    void s_setSelectedWombatInRegistryUnderground(bool b);
    void s_getRandomBurrowName();
    void s_createBurrow(QPoint pos, QString name);
    void s_prepareBurrowListForDisplay(int selectedBurrowId);
    void s_changeHomeBurrow(QPoint pos);
    void s_getBurrowName(QPoint pos);
    void s_addPosToBurrow(QPoint posBurrow, QPoint newPos);
    void s_getRandomWombatIdForPossess();
    void s_claimResource(QPoint pos, TileType tileType);
    void s_getWombatInventory(int wombatId);
    void s_getStorageInventory(int burrowId, int storageId);
    void s_addBuilding(QPoint pos,TileType buildingType,int variation);
    void s_moveItemFromWombat(InventoryItem item, int wombatId, int burrowId, int storageId, QPoint draggedLabelPos);
    void s_moveItemFromStorage(InventoryItem item, int wombatId, int burrowId, int storageId, QPoint draggedLabelPos);
    void s_setWInventoryWidgetData(bool wInventoryWidgetOpen, int wombatId);
    void s_setStorageWidgetData(bool storageWidgetOpen, int storageId);
    void s_prepareStorageInfoForShowStorageWidget(QPoint pos);
    void s_setInfoBoxOpen(bool b);
    void s_findPathDebug(PathPoint startPos, PathPoint endPos);
    void s_removeItemFromWombatInventory(int wombatId, InventoryItem item);
    void s_removeItemFromStorage(int burrowIndex, int storageIndex, InventoryItem item);
    void s_addTunnelsToBurrow(int burrowId, QVector<QPoint> startPos);
    void s_setAcceptedResourcesInStorage(int burrowId, int storageId, QVector<bool> acceptedResources);
    void s_showOwner(int burrowId);
    void s_hideOwner(int burrowId);
    void s_getNameWombatId(int wombatId);
    void s_printAllWombatsToConsole();
    void s_collectResource(QPoint pos,TileType tileType);

    //To Herder
    void s_taunt();

    //To HerdMember
    void s_setMovingInOpenGl(int wombatId, bool b);
};

#endif // CONTROLLER_H
