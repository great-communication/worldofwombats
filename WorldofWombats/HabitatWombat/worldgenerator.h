//Generates new terrain based on perlin noise
#ifndef WORLDGENERATOR_H
#define WORLDGENERATOR_H

#include <QObject>
#include "global.h"
#include "libnoise/noise.h"
#include "libnoise/noiseutils.h"

class WorldBuilder;
class World;
class Level;
class Global;

class WorldGenerator : public QObject
{
    Q_OBJECT
public:
    explicit WorldGenerator(QObject *parent = nullptr);
    WorldBuilder *worldBuilder;
    World *world;
    Level *level;
    Global *global;
    void setup(WorldBuilder *worldBuilder, World *world, Level *level, Global *global);
    QVector<QVector<TileStruct> > createChunk(QPoint chunkPos);

signals:

public slots:

private:
    utils::NoiseMap getNoiseData(QPoint pos, int seed, int octaveCount, double frequency, double persistence);
};

#endif // WORLDGENERATOR_H
