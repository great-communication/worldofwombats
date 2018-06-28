#include "worldgenerator.h"
#include "worldbuilder.h"
#include "world.h"
#include "level.h"
#include <QDebug>  //Remove

WorldGenerator::WorldGenerator(QObject *parent) : QObject(parent)
{

}

void WorldGenerator::setup(WorldBuilder *worldBuilder, World *world, Level *level, Global *global){
    this->worldBuilder = worldBuilder;
    this->world = world;
    this->level = level;
    this->global = global;
}


QVector<QVector<TileStruct>> WorldGenerator::createChunk(QPoint chunkPos){ // Remember to clean up, it's a mess...

    QPoint chunkSize = world->getChunkSize();
    QVector<QVector<TileStruct>> chunk;
    chunk.resize(chunkSize.x());
    for (int i=0;i<chunk.length();i++){
        chunk[i].resize(chunkSize.y());
    }

    int seed = level->getSeed();
    utils::NoiseMap heightMap = getNoiseData(chunkPos, seed, 6, 12.0, 0.75);
    utils::NoiseMap heightMap2 = getNoiseData(chunkPos, seed+1, 6, 2.0, 0.5);
    utils::NoiseMap heightMap3 = getNoiseData(chunkPos, seed+2, 6, 24.0, 0.5);
    utils::NoiseMap heightMap4 = getNoiseData(chunkPos, seed+3, 6, 12.0, 0.75);

    //Create dirt and bush
    for (int i=0;i<chunkSize.x();i++){
        for (int j=0;j<chunkSize.y();j++){
            QVector<quint8> overlays;
            QVector<quint8> overlayVariations;

            bool riceGrassTemp = false;
            int terrainType = Dirt;
            int riceGrassVariation = 5;

            float noiseValue = heightMap.GetValue(i,j);
            if (noiseValue<-0.2){
                terrainType = Bush;
            }else if ((noiseValue>-0.1 && noiseValue<0.0)||(noiseValue>0.1 && noiseValue<0.2)||(noiseValue>0.5 && noiseValue<0.6)||(noiseValue>0.7 && noiseValue<0.8)||(noiseValue>0.9 && noiseValue<1.0)){
                terrainType = Bush;
            }else if ((noiseValue>0.24 && noiseValue<0.26)||(noiseValue>0.44 && noiseValue<0.46)||(noiseValue>0.64 && noiseValue<0.66)||(noiseValue>0.84 && noiseValue<0.86)){
                terrainType = Bush;
            }

            float noiseValue2 = heightMap2.GetValue(i,j);
            if (noiseValue2<-0.9){
                terrainType = Bush;
            }else if ((noiseValue2>0.15 && noiseValue2<0.2)){
                terrainType = Dirt;
            }

            // A random number between 0 and 9
            QString sNoiseValue = QString::number(noiseValue);
            QString sRandNum1 = "0";
            if(sNoiseValue.length()>4){
                sRandNum1 = sNoiseValue.at(4);
            }
            int rand1 = sRandNum1.toInt();

            //Ricegrass generation 1  common but small
            float noiseValue3 = heightMap3.GetValue(i,j);
            if (noiseValue3>0.5){
                if(terrainType==Dirt){
                    riceGrassTemp = true;
                    //Decide riceGrassVariation
                    // Random number between min and min+range
                    // Range decided by rand1
                    int min = 5;
                    double range = 0;

                    if(rand1<5){
                        range = 5;
                    }else if(rand1<8){
                        range = 15;
                    }else{
                        range = 25;
                    }

                    double temp = noiseValue;

                    if(temp<0.0){
                        temp = temp*(-1.0);
                    }
                    //if(temp>1){
                    //    temp = 1.0;
                    //}
                    riceGrassVariation = (temp*range)+min;
                    //worker->consolePrint("riceGrassVariation "+QString::number(riceGrassVariation));
                }
            }

            //Ricegrass generation 2  rare but big islands
            if (noiseValue2>1.1){
                riceGrassTemp = true;
                terrainType = Dirt;

                int min = 5;
                double range = 0;

                if(rand1<3){
                    range = 5;
                }else if(rand1<7){
                    range = 15;
                }else{
                    range = 25;
                }

                if (noiseValue2>1.15){
                    range += 20;
                }

                if (noiseValue2>1.2){
                    range += 20;
                }

                if (noiseValue2>1.25){
                    range += 35;
                }

                QString sNoiseValue2 = QString::number(noiseValue);
                QString sRandNum2 = "0";

                if(sNoiseValue2.length()>5){
                    sRandNum2 = sNoiseValue2.remove(0,5);
                }
                sRandNum2.prepend("0.");
                double temp = sRandNum2.toDouble();
                riceGrassVariation = (temp*range)+min;
                //worker->consolePrint("variation: "+QString::number(riceGrassVariation));
            }


            if (riceGrassTemp && terrainType == Dirt){
                overlays.append((quint8)RiceGrass);
                overlayVariations.append((quint8)riceGrassVariation);
            }

    //Grass******************
            if(!riceGrassTemp){
                float noiseValue4 = heightMap4.GetValue(i,j);
                bool grass = false;
                if(terrainType!=Bush){
                    if (noiseValue4<-0.2){
                        grass = true;
                    }else if ((noiseValue4>-0.1 && noiseValue4<0.0)||(noiseValue4>0.1 && noiseValue4<0.2)||(noiseValue4>0.5 && noiseValue4<0.6)||(noiseValue4>0.7 && noiseValue4<0.8)||(noiseValue4>0.9 && noiseValue4<1.0)){
                        grass = true;
                    }else if ((noiseValue4>0.24 && noiseValue4<0.26)||(noiseValue4>0.44 && noiseValue4<0.46)||(noiseValue4>0.64 && noiseValue4<0.66)||(noiseValue4>0.84 && noiseValue4<0.86)){
                        grass = true;
                    }
                }

                if (grass){
                    overlays.append((quint8)Grass);
                    int grassVariation = worldBuilder->getRandomNumber(5,50);
                    overlayVariations.append((quint8)grassVariation);
                }
            }
    //******************

            int variation = worldBuilder->getRandomNumber(1, global->getVariantsPerTileType((TileType)terrainType));

            if (terrainType==Bush){ //Bush temp fix
                overlays.append((quint8)Bush);
                variation = worldBuilder->getRandomNumber(20,25);
                overlayVariations.append((quint8)variation);
                terrainType = Dirt;
                variation = 1;
            }

            TileStruct tile;
            tile.terrain = (quint8)terrainType;
            tile.terrainVariation = (quint8)variation;
            tile.terrainUnderground = (quint8)UnDug;
            tile.overlay = overlays;
            tile.overlayVariations = overlayVariations;
            QVector<quint8> overlaysUnderground;
            tile.overlayUnderground = overlaysUnderground;
            QVector<quint8> overlayVariationsUnderground;
            tile.overlayVariationsUnderground = overlayVariationsUnderground;
            tile.burrowId = (qint32)None;
            tile.tileOwner = (qint32)None;
            QVector<qint32> wombatIds;
            tile.wombatIds = wombatIds;
            chunk[i][j] = tile;
         }
    }

    if (chunkPos.x()==0 && chunkPos.y()==0){ //Start chunk
        QVector<quint8> overlays;
        QVector<quint8> overlayVariations;
        chunk[0][0].overlay = overlays; //Start position is always dirt = no overlays
        chunk[0][0].overlayVariations = overlayVariations;
        chunk[1][0].overlay = overlays; //Start position is always dirt = no overlays
        chunk[1][0].overlayVariations = overlayVariations;

    }

    return chunk;
}

utils::NoiseMap WorldGenerator::getNoiseData(QPoint pos, int seed, int octaveCount, double frequency, double persistence){
    noise::module::Perlin myModule;
    myModule.SetSeed(seed);
    myModule.SetOctaveCount(octaveCount);
    myModule.SetFrequency (frequency);
    myModule.SetPersistence (persistence);
    utils::NoiseMap heightMap;
    utils::NoiseMapBuilderPlane heightMapBuilder;
    heightMapBuilder.SetSourceModule (myModule);
    heightMapBuilder.SetDestNoiseMap (heightMap);
    QPoint chunkSize = world->getChunkSize();
    heightMapBuilder.SetDestSize (chunkSize.x(), chunkSize.y());

    double lowerXBound = (double)pos.x();
    lowerXBound = lowerXBound/2;
    double upperXBound = lowerXBound+0.5;
    double lowerZBound = (double)pos.y();
    lowerZBound = lowerZBound/2;
    double upperZBound = lowerZBound+0.5;

    heightMapBuilder.SetBounds (lowerXBound,upperXBound,lowerZBound,upperZBound);
    heightMapBuilder.Build();

    return heightMap;
}
