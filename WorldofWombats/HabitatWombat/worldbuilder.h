//Game Back-End
#ifndef WORLDBUILDER_H
#define WORLDBUILDER_H

#include <QObject>
#include "global.h"
#include <QImage>

class Global;
class Registry;
class Herder;
class Worker;
class Level;
class World;
class ImagePainter;

class WorldBuilder : public QObject
{
    Q_OBJECT

public:
    explicit WorldBuilder(QObject *parent = 0);    
    Global *global;
    Worker *worker;    
    Registry *registry;
    Herder *herder;
    Level *level;
    World *world;
    ImagePainter *imagePainter;
    QPoint worldPosition;
    int month;
    bool gameOver;
    QTimer *timerNewMonth;   
    QVector<QVector<VisibleTileStruct>> visibleTiles;
    bool isUnderground;
    void setup(QString levelName, QSize backgroundSize);    
    void prepareToUpdateTiles(QPoint viewPosition, bool isUnderground);
    void prepareToUpdateTilesForTeleport(QPoint targetPos, bool isUnderground);
    void updateAllVisibleTiles();
    void updateTile(QPoint indexes);
    void changeTerrain(QPoint pos, Terrain terrain, int burrowId, bool update);
    void addOverlayToTile(QPoint pos, Overlay overlay, int variation);
    void removeOverlayFromTile(QPoint pos, Overlay overlay, int variation);
    void addOverlayToTileUnderground(QPoint pos, Overlay overlay, int variation);
    void removeOverlayFromTileUnderground(QPoint pos, Overlay overlay, int variation);
    bool hasOverlay(QPoint pos, Overlay overlay, bool isUnderground);
    int getOverlayVariation(QPoint pos, Overlay overlay, bool isUnderground);
    void addOverlayToVisibleTile(QPoint pos, Overlay overlay, int variation);
    void addOverlayToVisibleTileUnderground(QPoint pos, Overlay overlay, int variation, QVector<int> miniTileNumbers);
    void removeOverlayFromVisibleTile(QPoint pos, Overlay overlay, int variation);
    void removeOverlayFromVisibleTileUnderground(QPoint pos, Overlay overlay, int variation);
    void addWombatIdToTile(QPoint pos, int wombatId);
    void removeWombatIdFromTile(QPoint pos, int wombatId);
    bool hasTerrain(Terrain baseTerrain, bool isUnderground, QPoint pos);
    Terrain getTerrain(QPoint pos, bool isUnderground);
    int getBurrowId(QPoint pos);
    int getOwner(int x, int y);
    QVector<int> getTileWombatIds(QPoint pos);
    void setOwner(int x, int y, int riceGrassOwner);
    int getRandomNumber(const int min, const int max);
    void setWorldPosition(QPoint pos);
    void setGameOver();
    VisibleTileStruct& getVisibleTile(QPoint pos);
    void quitGame(QPoint viewPosition);    
    bool fileExists(QString path);
    QPoint qPointModulo(QPoint dividend, QPoint divisor);
    QPoint qPointMultiplication(QPoint factor, QPoint factor2);
    QPoint qPointDivisionWithFloor(QPoint numerator, QPoint denominator);


signals:    
    void s_setIsBlocked(bool blocked);
    void s_setViewPositon(QPoint pos);
    void s_newMonth();

public slots:
    void newMonth();

private slots:


private:    

    enum Compass {
        NorthWest,
        West,
        SouthWest,
        North,
        South,
        NorthEast,
        East,
        SouthEast,
    };
    enum MiniTileType {
        CornerTile,
        EdgeTile,
    };    
    enum Miscellaneous {
        NotLoaded = -1,
    };    
    int timeSpeed;
    QPoint positionsChanged;    
    void clearVisibleTiles();
    void updateNeighboursArray(QPoint viewPosition);
    void removeTiles(QPoint pos);
    QVector<int> calculateMiniTileNumbers(int terrainType, QPoint pos, bool overlay, bool isUnderground);
    int reMap(QBitArray miniTile);
    TileStruct tempfixxx;
};

#endif // WORLDBUILDER_H
