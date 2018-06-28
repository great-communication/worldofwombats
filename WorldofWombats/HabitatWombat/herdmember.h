//Keeps track of and moves a wombat that is currently visible
//Managed by HW_Herder
#ifndef HERDMEMBER_H
#define HERDMEMBER_H

#include <QObject>
#include <QTime>
#include <QTimer>
#include <QPoint>
#include "global.h"

class WorldBuilder;
class Worker;
class Herder;
class Registry;

class HerdMember : public QObject
{
    Q_OBJECT
public:
    explicit HerdMember(QObject *parent = nullptr);
    WorldBuilder *worldBuilder;
    Worker *worker;
    Herder *herder;
    Registry *registry;
    QTimer *timerNewRandomTarget;
    QTimer *timerMoveToTarget;    
    int wombatId;    
    QPoint originalTilePos;     //This is the original worldPosition of the wombat
    QPoint oldTilePos;    
    QPoint currentPxOffset;  //This is the offset in pixels from the originalTilePos
    qreal rotation;
    QPoint pxChange;    //This is the updated screen location sent to glwidget, e.g. x = 1px, y = -1px;
    bool underground;    
    int hIndex; //Where is this used? try to remove
    int burrowId;
    QTimer *timerTEMPFIXFUCK;
    void setup();    
    QPoint getWorldPosFromPxOffsetPos(QPoint posPx);
    QPointF getInternalPosPx(QPointF pos);   
    WombatAIState currentState;    
    WombatAITask currentTask;
    QString oldTaskString;
    QString taskString;
    QVector<PathPoint> currentPath;
    bool movingInOpenGl;
    void setMovingInOpenGl(bool b);
    void wakeUp();


signals:

public slots:
    void growWombat();

private slots:
    void newRandomTarget();
    void moveToTarget();
    void timerGOFUCKYOURSELF();

private:    
    QPoint newTilePos;
    QPoint oldPosPx;
    QPoint targetPosPx;
    int moveXDirection;
    int moveYDirection;
    bool left, up, down, right;
    int age;
    int birthMonth;
    int xMoveMax = 128;
    int xMoveMin = 0;
    int yMoveMax = 128;
    int yMoveMin = 0;
    QPoint internalPos;    
    QPoint storagePos;
    int idleCount;
    int idleTime;
    int myWorkShare;
    int riceGrassesLeftToTake, grassesLeftToTake, sticksLeftToTake;
    int workCount;
    int amountToEat;
    int noMoreRiceGrassCount, noMoreGrassCount, noMoreSticksCount;
    bool tempFixFUCKCheckTask, tempFixFUCKEat, tempFixFUCKStartSleep, tempFixFUCKHarvestRiceGrass, tempFixFUCKHarvestGrass, tempFixFUCKHarvestSticks, tempFixFUCKWakeUpExclamation;
    int storageIdFUCK;
    QPoint sticksLocationFUCK;
    void rotateWombat();
    void goBackToOldTarget();
    void checkTask();
    Resource checkIfFetchTaskIsAvailable(int burrowIndex);
    void fetchRiceGrass(int burrowIndex);
    void fetchGrass(int burrowIndex);
    void fetchSticks(int burrowIndex);
    void newTargetFromPath();
    void targetMiddleOfTile();
    void targetEdgeOfTile();
    void returnToRiceGrassStorage();
    void returnToGrassStorage();
    void returnToSticksStorage();
    void harvestRiceGrass();
    void harvestGrass();
    void harvestSticks();
    void returnRiceGrassToStorage();
    void returnGrassToStorage();
    void returnSticksToStorage();
    QPoint checkSurroundingTilesForRiceGrass();
    QPoint checkSurroundingTilesForGrass();
    QPoint checkSurroundingTilesForSticks();
    void setTargetTile(QPoint pos);
    void taskIdling();
    void taskFindingSpotToSleep();
    void taskSleeping();
    void taskFetchingRiceGrass();
    void taskFetchingGrass();
    void taskFetchingSticks();
    void taskHarvestingRiceGrass();
    void taskHarvestingGrass();
    void taskHarvestingSticks();
    void taskReturningRiceGrass();
    void taskReturningGrass();
    void taskReturningSticks();
    void continueIdle();
    void startSleep();
    void findSpotToSleep();
    void taskGoingToEat();
    void taskEating();
    void eat();
    void goToEat();
    void setTaskString(QString s);    
    void startTimerMoveToTarget();
    void getAndSetNewPath(PathPoint start, PathPoint end);
};

#endif // HERDMEMBER_H
