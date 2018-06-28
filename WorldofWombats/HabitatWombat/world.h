//Stores the loaded world and related functionality
#ifndef WORLD_H
#define WORLD_H

#include <QObject>
#include "global.h"

class WorldBuilder;
class Level;
class Global;

class World : public QObject
{
    Q_OBJECT
public:
    explicit World(QObject *parent = nullptr);
    WorldBuilder *worldBuilder;
    Level *level;
    Global *global;
    void setup(WorldBuilder *worldBuilder, Level *level, Global *global);
    TileStruct &getTile(QPoint pos);
    void saveAllSectors();
    QPoint getChunkSize();

signals:

public slots:

private:
    QPoint chunkSize;
    QPoint sectorSizeInChunks;
    QPoint sectorSizeInTiles;
    int maxSectorCount;
    struct SectorStruct {
        QPoint id;
        qint64 lastAccessed;
        QVector<QVector<bool>> doesChunkExist;
        QVector<QVector<QVector<QVector<TileStruct>>>> tiles;
    };
    QVector<SectorStruct> loadedSectors;
    void createSector(QPoint sectorId);
    void saveSector(int sectorIndex, QPoint sectorId);
    void loadSector(QPoint sectorId);
    void unloadOldSectors();
    int getSectorIndex(QPoint sectorId);
};

#endif // WORLD_H
