//Global variables, enums and structs
#ifndef GLOBAL_H
#define GLOBAL_H

#include <QObject>
#include <QVector>
#include <QMetaType>

extern int gFrameRate;
extern int gTileSize;
extern int gStorageSize;
extern int gStorageBufferSize;
extern int gWombatInventorySize;


enum Terrain {
    Terrain_None = -1,
    Terrain_Dirt = 1,
    Terrain_Hole = 3,
    Terrain_UnDug = 4,
    Terrain_Tunnel = 5,
    Terrain_Burrow = 9,
};

Q_DECLARE_METATYPE(Terrain)

enum Overlay {
    Overlay_None = -1,
    Overlay_Bush = 2,
    Overlay_HoleLight = 6,
    Overlay_Grass = 7, //Own loading
    Overlay_RiceGrass = 8, //Own loading
    Overlay_Corpse = 11,
    Overlay_Nest = 12,
    Overlay_Path = 15, // Own loading
    Overlay_Cross = 99,
    Overlay_Storage = 100,
    Overlay_Basket = 91, // Own loading
    Overlay_RiceGrass_Bundle04 = 101, //Own loading
    Overlay_Grass_Bundle01 = 102, //Own loading
    Overlay_Sticks_Bundle01 = 103, //Own loading
};

enum TileType {  //When adding a new tileType, add it here, and to QVector<TileTypeStruct> gTileTypes
    None = -1,
    Dirt = 1,
    Bush = 2,
    Hole = 3,
    UnDug = 4,
    Tunnel = 5,
    HoleLight = 6,
    Grass = 7, //Own loading
    RiceGrass = 8, //Own loading
    Burrow = 9,
    Corpse = 11,
    Nest = 12,
    Path = 15, // Own loading
    RiceGrass_Bundle01 = 50, //remove
    RiceGrass_Bundle02 = 51, //remove
    RiceGrass_Bundle03 = 52, //remove
    Basket = 91,
    Cross = 99,
    Storage = 100,
    RiceGrass_Bundle04 = 101, //Own loading
    Grass_Bundle01 = 102, //Own loading
    Sticks_Bundle01 = 103, //Own loading
};

Q_DECLARE_METATYPE(TileType)

struct TileTypeStruct{
    TileType tileType;
    int variations;
    int zIndex;
};

struct TerrainStruct{
    Terrain terrain;
    int variations;
    int zIndex;
};

struct OverlayStruct{
    Overlay overlay;
    int variations;
    int zIndex;
};

enum SoundFX {
    Sound_Bush,
    Sound_BurrowCreated = 11,
    Sound_NewHome = 15,
    Sound_GameOver = 19,
    Sound_Death = 20,
    Sound_Taunt = 21,
    Sound_ClaimRiceGrass = 25, //Indexing will be wrong after this? Change 25 to 13213 or something
};

Q_DECLARE_METATYPE(SoundFX)

enum Resource {     //When adding a new resource, add it here, and to QVector<ResourceStruct> gResources
    Resource_None = -1,
    Resource_Sticks = 1,
    Resource_Grass = 2,
    Resource_RiceGrass = 3,
};

struct ResourceStruct {
    Resource resource;
    int size;
};

struct InventoryItem {
    Resource resource;
    int amount;
};

Q_DECLARE_METATYPE(InventoryItem)

struct VisibleTileStruct {
    int terrainType;
    int variation;
    int terrainTypeUnderground;
    QVector<int> miniTileNumbers;
    QVector<int> miniTileNumbersUnderground;
    QVector<int> overlayTerrainTypes;
    QVector<int> overlayVariations;
    QVector<QVector<int>> overlayMiniTileNumbers;
    QVector<int> overlayTerrainTypesUnderground;
    QVector<int> overlayVariationsUnderground;
    QVector<QVector<int>> overlayMiniTileNumbersUnderground;
    qreal rotation;
    int burrowId;
    QVector<QPoint> riceGrassImagePos;
    QVector<int> riceGrassImageIndex;
    QVector<QPoint> grassImagePos;
    QVector<int> grassImageIndex;
    QVector<int> pathImageIndex;
};

struct TileStruct {
    quint8 terrain;
    quint8 terrainVariation;
    quint8 terrainUnderground;
    QVector<quint8> overlay; //Length saved as separete variable
    QVector<quint8> overlayVariations;
    QVector<quint8> overlayUnderground; //Length saved as separete variable
    QVector<quint8> overlayVariationsUnderground;
    qint32 burrowId;
    qint32 tileOwner;
    QVector<qint32> wombatIds; //Length saved as separete variable
};

struct MoveStruct{
    QPoint pos;
    Terrain tileType;
    Terrain tileTypeUnderground;
    int burrowId;
    QVector<int> overlayTerrainTypes;
    QVector<int> overlayTerrainTypesUnderground;
};

Q_DECLARE_METATYPE(MoveStruct)

struct BuildCost{
    Overlay building;
    QVector<InventoryItem> itemsNeeded;
};

struct PathPoint {
    QPoint pos;
    bool underground;
};

Q_DECLARE_METATYPE(PathPoint)


enum WombatAIState {
    WombatAIState_Idle = 1,
    WombatAIState_Working = 2,
};
enum WombatAITask {
    WombatAITask_Moving = 1,
    WombatAITask_FetchingRiceGrass = 2,
    WombatAITask_FetchingGrass = 3,
    WombatAITask_FetchingSticks = 4,
    WombatAITask_ReturningRiceGrass = 5,
    WombatAITask_ReturningGrass = 6,
    WombatAITask_ReturningSticks = 7,
    WombatAITask_HarvestingRiceGrass = 8,
    WombatAITask_HarvestingGrass = 9,
    WombatAITask_HarvestingSticks = 10,
    WombatAITask_Idling = 11,
    WombatAITask_Sleeping = 12,
    WombatAITask_FindingSpotToSleep = 13,
    WombatAITask_Eating = 14,
    WombatAITask_GoingToEat = 15,
};

Q_DECLARE_METATYPE(WombatAITask)

enum WombatEmoticon {
    WombatEmoticon_None = -1,
    WombatEmoticon_Sleeping = 1,
    WombatEmoticon_Exclamation = 2,
};

Q_DECLARE_METATYPE(WombatEmoticon)

enum CauseOfDeath {
    CauseOfDeath_Unknown = -1,
    CauseOfDeath_OldAge = 1,
    CauseOfDeath_Starvation = 2,
};

Q_DECLARE_METATYPE(CauseOfDeath)

struct wombatPosBufferStruct { //Used by OpenGL
    int displayIndex;
    QPoint pxChange;
    qreal wombatRotation;
    bool wombatUnderground;
    int wombatAge; //This shouldn't really be here... should use separate system
};

struct TerrainImagesStruct {
    Terrain terrain;
    QVector<QVector<QImage>> images;
};

struct OverlayImagesStruct {
    Overlay overlay;
    QVector<QVector<QImage>> images;
};



class Global : public QObject
{
    Q_OBJECT

public:
    explicit Global(QObject *parent = 0);
    QVector<TileTypeStruct> gTileTypes;
    QVector<TerrainStruct> gTerrains;
    QVector<OverlayStruct> gOverlays;
    QVector<ResourceStruct> gResources;
    QVector<BuildCost> gBuildCost;
    void setup();    
    int getItemSize(InventoryItem item);
    int getResourceSize(Resource resource);
    int getTileTypeIndex(TileType tileType);
    int getVariantsPerTileType(TileType tileType);    
    int getZIndex(Terrain terrain);
    int getZIndex(Overlay overlay);
    QString getResourceName(Resource resource);    
    QString getName(Terrain terrain);
    QString getName(Overlay overlay);
    QVector<InventoryItem> getBuildCost(Overlay building);
    QString getObituary(CauseOfDeath causeOfDeath);

signals:


public slots:


private slots:


private:
    void createBuildCostArray();
    void createTileTypesArray();
    void createResourcesArray();
};

#endif // GLOBAL_H


