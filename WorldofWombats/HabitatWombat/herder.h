//Manages currently visible wombats
//Also manages currently visible storages
#ifndef HERDER_H
#define HERDER_H

#include <QObject>
#include <QTime>
#include <QTimer>
#include <QPoint>
#include <QVector>

class WorldBuilder;
class Worker;
class Registry;
class HerdMember;

class Herder : public QObject
{
    Q_OBJECT

public:
    explicit Herder(QObject *parent = 0);
    //GUI *hwGameUI;
    WorldBuilder *worldBuilder;
    Worker *worker;
    Registry *registry;
    HerdMember *herdMember;   
    QList<HerdMember*> wombatHerd; //Why list?
    QList<int> wombatIds;
    void setup();   
    void addWombatToHerd(int wombatId, QPoint tilePos, QPoint offsetInPixels, qreal rotation);
    void removeWombatFromHerd(int index);
    void removeAllWombatsFromHerd();
    void updateHerd(QPoint pos, int maxIndex);
    void updateVisibility();  
    void addStorageTileToHerd(int burrowId, QPoint pos);
    void removeStorageTileFromHerd();
    void removeAllStorageTilesFromHerd();
    void updateStorageDisplay(int burrowIndex, int storageIndex, int tileIndex);
    void taunt();

private slots:


private:    
    struct storageHerdStruct{
        int burrowId;
        QPoint pos;        
    };

    QVector<storageHerdStruct> storageHerdFUCK;

};

#endif // HERDER_H
