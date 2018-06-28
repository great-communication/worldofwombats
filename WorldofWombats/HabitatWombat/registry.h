//Keeps track of all burrows and wombats and related functionality
#ifndef REGISTRY_H
#define REGISTRY_H

#include <QObject>
#include <QList>
#include "global.h"

class WorldBuilder;
class Global;
class Worker;
class Herder;

class Registry : public QObject
{
    Q_OBJECT

public:
    explicit Registry(QObject *parent = 0);    
    WorldBuilder *worldBuilder;
    Global *global;
    Worker *worker;
    Herder *herder;
    struct FrontierStruct {
        QPoint pos;
        int distance;
        int distanceFromStart;
        int distanceToTarget;
        bool underground;
    };
    struct PathFindStruct {
        QPoint pos;
        QPoint cameFrom;
        bool underground;
    };
    struct StorageTileStruct {
        QPoint pos;
        bool visible;
        QVector<InventoryItem> inventory;
    };
    struct StorageStruct {
        int id;        
        QVector<StorageTileStruct> allTiles;
        QVector<bool> acceptedResources;
    };

    struct BurrowResourceStruct {
        QPoint pos;
        int quantity;
        double quantityAvailable;
        double growth;
    };

    struct BurrowStruct {
        int id;
        QString name;
        int food;
        QVector<BurrowResourceStruct> riceGrasses;
        int size;
        int population; // This int is needed so we know how many populationWombatIds to read from file  //just use length you retard
        int entrances;  // This int is needed so we know how many entrancePos to read from file
        QVector<int> populationWombatIds;
        QVector<QPoint> entrancePos;
        QVector<QPoint> allNonEntrancePos;
        QVector<StorageStruct> storages;
        int workBuffer;
        int eatBuffer; // Is this save/loaded correctly?
        QVector<BurrowResourceStruct> grasses;
        QVector<BurrowResourceStruct> bushes;
        QVector<QPoint> nests;
    };


    struct WombatStruct {
        int id;
        QString name;
        bool sex;
        int age;
        int birthMonth;
        int burrowId;
        QPoint pos;
        bool underground; //If the wombat is currently underground
        QVector<InventoryItem> inventory;
        int health;
        int rested;
        int plump;
    };
    enum Sex {
        Female,
        Male,
    };
    QVector<int> burrowIds;
    QVector<BurrowStruct> burrows;
    QVector<int> wombatIds;
    QVector<WombatStruct> wombats;    
    int selectedWombat;
    bool guiInfoBoxOpen;
    int wombatIdGuiInfoBox;
    QVector<PathFindStruct> visited; // Temp so that worker can access and clear previous overlays, should be in findPath(), right?
    void setup();
    void saveBurrowsToFile();
    void saveWombatsToFile();
    void createBurrow(QPoint pos, QString burrowName);
    void createWombat(QPoint pos, bool underground, int burrowId);
    QString getRandomBurrowName();
    QString getRandomWombatName(bool sex);
    int getRandomWombatIdForPossess();
    void changeHomeBurrow(QPoint pos);
    void setSelectedWombat(int wombatId);
    int getWombatIndex(int wombatId);
    int getBurrowIndex(int burrowId);
    int getStorageIndex(int burrowIndex, int storageId);
    void selectedWombatDied(QString wombatName, int wombatAge, CauseOfDeath causeOfDeath);
    void processBirthList();
    void killWombats(QVector<int> killList, QVector<CauseOfDeath> causeOfDeath);
    void removeWombatFromRegistry(int wombatId);
    void saveBurrowsToTextFile();
    void addTunnelsToBurrow(int burrowId, QVector<QPoint> startPos);
    void addPosToBurrow(QPoint pos, QPoint newPos);
    void addEntrancePosToBurrow(int burrowId, QPoint pos);
    void changeWombatPos(int wombatId, QPoint newTilePos);
    void setPosOfDepossessedWombat(int wombatId, QPoint newTilePos);
    InventoryItem addItemToWombatInventory(int wombatId, Resource resource, int amount); //Change parameters to inventoryItem    
    InventoryItem getItemFromWombatInventory(int wombatId, InventoryItem item);
    void addBuilding(QPoint pos, Overlay building, int variation);
    InventoryItem addItemToStorage(int burrowIndex, int storageIndex, InventoryItem item);
    void removeItemFromStorageOverload(int burrowId, int storageId, InventoryItem item);
    void removeItemFromStorage(int burrowIndex, int storageIndex, InventoryItem item);
    int getStorageId(QPoint pos, int burrowId);
    QVector<InventoryItem> getStorageInventory(int burrowIndex, int storageIndex);
    QVector<InventoryItem> getWombatInventory(int wombatId);
    void prepareWombatInfoForDisplay(int wombatId);
    void prepareTileInfoForDisplay(QPoint pos);
    void possess(int wombatId, QPoint viewPosition, QPoint moved, bool underground, qreal rotation);
    void scrollToTargetPosDone();
    void setSelectedWombatInRegistryUnderground(bool b);
    void setWombatUnderground(int wombatId, bool underground);
    void prepareBurrowListForDisplay(int selectedBurrowId);
    QString getBurrowName(QPoint pos);   
    void claimResource(QPoint pos, TileType tileType);
    void collectResource(QPoint pos, TileType tileType);
    void collectRiceGrass(QPoint pos);
    void collectGrass(QPoint pos);
    void collectBush(QPoint pos);
    int sizeLeftInWombatInventory(int wombatId);
    void moveItemFromWombatToStorage(InventoryItem item, int wombatId, int burrowId, int storageId, QPoint draggedLabelPos);
    void moveItemFromStorageToWombat(InventoryItem item, int wombatId, int burrowId, int storageId, QPoint draggedLabelPos);
    bool checkIfInRange(int burrowIndex, int storageIndex);
    void setWInventoryWidgetData(bool wInventoryWidgetOpen, int wombatId);
    void setStorageWidgetData(bool storageWidgetOpen, int storageId);
    void prepareStorageInfoForShowStorageWidget(QPoint pos);
    QVector<PathPoint> findPath(PathPoint start, PathPoint end);  
    void setInfoBoxOpen(bool b);
    void setAcceptedResourcesInStorage(int burrowId, int storageId, QVector<bool> acceptedResources);
    void showOwner(int burrowId);
    void hideOwner(int burrowId);
    void printAllWombatsToConsole();

signals:
    void s_sendToConsole(QString string);
    void s_newMonthUpdateDone();

public slots:    
    void newMonth();

private slots:


private:    
    QVector<QString> burrowNames;
    QVector<QString> femaleNames;
    QVector<QString> maleNames;
    QVector<QString> femaleNicknames;
    QVector<QString> maleNicknames;       
    int wombatIdCount;
    bool guiWInventoryWidgetOpen;
    int guiWInventoryWidgetWombatId;
    bool guiStorageWidgetOpen;
    int guiStorageWidgetStorageId;
    void readTextFile(QString name, QVector<QString> &array);
    void loadBurrowsFromFile();
    void loadWombatsFromFile();
    bool fileExists(QString path);    
    QVector<QVector<QPoint> > exploreTunnel(QPoint pos);
    void createMiriamAndWarren();
    void makeHerdedWombatsTired();
    void makeWombatsOlder();
    void calculateWorkDone();
    void workBufferOverflow(int burrowIndex, int overflow);
    void growResources();    
    void addStorage(QPoint pos, int burrowId);
    void addNest(QPoint pos, int burrowId);
    void showCorpse(QPoint pos, qreal rotation, bool isUnderground);
    void calculateFoodUsed();
    void makeWombatsHungry();
    void feedWombat(int wombatIndex,int burrowIndex);
    InventoryItem collectRiceGrassForWorkBufferOverflow(int burrowIndex, int amount);
    InventoryItem collectGrassForWorkBufferOverflow(int burrowIndex, int amount);
    InventoryItem collectSticksForWorkBufferOverflow(int burrowIndex, int amount);
};

#endif // REGISTRY_H
