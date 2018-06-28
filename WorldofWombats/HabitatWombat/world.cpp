// The world is organized into sectors, chunks and tiles
#include "world.h"
#include "worldbuilder.h"
#include "level.h"
#include "worker.h"
#include "worldgenerator.h"
#include <QFile>
#include <QDir>
#include <QDateTime>

World::World(QObject *parent) : QObject(parent){  

}

void World::setup(WorldBuilder *worldBuilder, Level *level, Global *global){
    this->worldBuilder = worldBuilder;
    this->level = level;
    this->global = global;
    sectorSizeInChunks = QPoint(16,16); //16x16 chunks per sector
    chunkSize = QPoint(32,32);  //32x32 tiles per chunk
    sectorSizeInTiles = this->worldBuilder->qPointMultiplication(sectorSizeInChunks,chunkSize); //16x16 chunks with 32x32 tiles each
    maxSectorCount = 4; // Maximum number of sectors that can be loaded at the same time
}

void World::createSector(QPoint sectorId){
    //Creates an empty sector file
    //It is filled with data by createChunk()
    QString levelName = level->getName();
    QDir dir("hw/"+levelName+"/sectors");
    if (!dir.exists()) {
        dir.mkpath(".");
    }

    QString filename = "hw/"+levelName+"/sectors/s_"+QString::number(sectorId.x())+","+QString::number(sectorId.y());
    QFile file( filename );
    if (file.open(QIODevice::WriteOnly))
    {
        QDataStream out(&file);   // we will serialize the data into the file

        //Write header
        bool doesChunkExist = false;
        for (int i=0;i<sectorSizeInChunks.x();i++){
            for (int j=0;j<sectorSizeInChunks.y();j++){
                out << doesChunkExist;
            }
        }

        file.close();
        worldBuilder->worker->consolePrint("Created sector "+QString::number(sectorId.x())+","+QString::number(sectorId.y()));
   }
}

void World::saveAllSectors(){
    for (int i=0;i<loadedSectors.length();i++){
        saveSector(i, loadedSectors.value(i).id);
    }
}

void World::saveSector(int sectorIndex, QPoint sectorId){
    QString levelName = level->getName();
    QDir dir("hw/"+levelName+"/sectors");
    if (!dir.exists()) {
        dir.mkpath(".");
    }

    QString filename = "hw/"+levelName+"/sectors/s_"+QString::number(sectorId.x())+","+QString::number(sectorId.y());
    QFile file( filename );
    if (file.open(QIODevice::WriteOnly))
    {
        QDataStream out(&file);   // We will serialize the data into the file

        //Write header
        for (int i=0;i<sectorSizeInChunks.x();i++){
            for (int j=0;j<sectorSizeInChunks.y();j++){
                out << loadedSectors[sectorIndex].doesChunkExist[i][j];
            }
        }

        //Write tiles
        for (int i=0;i<sectorSizeInChunks.x();i++){
            for (int j=0;j<sectorSizeInChunks.y();j++){
                if (loadedSectors[sectorIndex].doesChunkExist[i][j]){
                    for (int k=0;k<chunkSize.x();k++){
                        for (int l=0;l<chunkSize.y();l++){
                            //Check global if you want to see structure of TileStruct
                            //Array lengths are saved as separate variables:
                            //QVector<quint8> overlayTerrainType; //Array length saved as quint8
                            //QVector<quint8> overlayTerrainTypeUnderground; //Array length saved as quint8
                            //QVector<qint32> wombatIds; //Array length saved as qint32

                            TileStruct &tileRef = loadedSectors[sectorIndex].tiles[i][j][k][l];                            
                            out << (quint8)tileRef.terrain;
                            out << (quint8)tileRef.terrainVariation;
                            out << (quint8)tileRef.terrainUnderground;
                            out << (quint8)tileRef.overlay.length();
                            for (int m=0;m<tileRef.overlay.length();m++){
                                out << (quint8)tileRef.overlay.value(m);
                                out << (quint8)tileRef.overlayVariations.value(m);
                            }                            
                            out << (quint8)tileRef.overlayUnderground.length();
                            for (int m=0;m<tileRef.overlayUnderground.length();m++){
                                out << (quint8)tileRef.overlayUnderground.value(m);
                                out << (quint8)tileRef.overlayVariationsUnderground.value(m);
                            }                            
                            out << (qint32)tileRef.burrowId;
                            out << (qint32)tileRef.tileOwner;
                            out << (qint32)tileRef.wombatIds.length();
                            for (int m=0;m<tileRef.wombatIds.length();m++){                                
                                out << (qint32)tileRef.wombatIds.value(m);
                            }

                        }
                    }
                }
            }
        }

        file.close();
        worldBuilder->worker->consolePrint("Saved sector "+QString::number(sectorId.x())+","+QString::number(sectorId.y()));
   }
}



void World::loadSector(QPoint sectorId){
    QString levelName = level->getName();
    QString fileName = "hw/"+levelName+"/sectors/s_"+QString::number(sectorId.x())+","+QString::number(sectorId.y());

    if (worldBuilder->fileExists(fileName)==false){
        createSector(sectorId);
    }

    SectorStruct sector;
    sector.id = sectorId;
    sector.lastAccessed = QDateTime::currentMSecsSinceEpoch();
    //Setting array sizes
    sector.doesChunkExist.resize(chunkSize.x());
    for (int i=0;i<sector.doesChunkExist.length();i++){
        sector.doesChunkExist[i].resize(chunkSize.y());
    }
    sector.tiles.resize(sectorSizeInChunks.x());
    for (int i=0;i<sector.tiles.length();i++){
        sector.tiles[i].resize(sectorSizeInChunks.y());
        for (int j=0;j<sector.tiles[i].length();j++){
            sector.tiles[i][j].resize(chunkSize.x());
            for (int k=0;k<sector.tiles[i][j].length();k++){
                sector.tiles[i][j][k].resize(chunkSize.y());
            }
        }
    }

    QFile file(fileName);

    if (file.open(QIODevice::ReadOnly)){
        QDataStream stream(&file);

        // Read header
        for (int i=0;i<sectorSizeInChunks.x();i++){
            for (int j=0;j<sectorSizeInChunks.y();j++){
                bool doesChunkExist;
                stream >> doesChunkExist;
                sector.doesChunkExist[i][j] = doesChunkExist;
            }
        }

        //Read chunks
        for (int i=0;i<sectorSizeInChunks.x();i++){
            for (int j=0;j<sectorSizeInChunks.y();j++){
                if (sector.doesChunkExist[i][j]){
                    for (int k=0;k<chunkSize.x();k++){
                        for (int l=0;l<chunkSize.y();l++){
                            TileStruct tile;
                            stream >> tile.terrain;
                            stream >> tile.terrainVariation;
                            stream >> tile.terrainUnderground;
                            quint8 arrayLength8;
                            stream >> arrayLength8;
                            QVector<quint8> overlays;
                            QVector<quint8> overlayVariations;
                            quint8 temp8;

                            for (int m=0;m<arrayLength8;m++){
                                stream >> temp8;
                                overlays.append(temp8);
                                stream >> temp8;
                                overlayVariations.append(temp8);
                            }

                            tile.overlay = overlays;
                            tile.overlayVariations = overlayVariations;

                            stream >> arrayLength8;
                            QVector<quint8> overlaysUnderground;
                            QVector<quint8> overlayVariationsUnderground;

                            for (int m=0;m<arrayLength8;m++){
                                stream >> temp8;
                                overlaysUnderground.append(temp8);
                                stream >> temp8;
                                overlayVariationsUnderground.append(temp8);
                            }

                            tile.overlayUnderground = overlaysUnderground;
                            tile.overlayVariationsUnderground = overlayVariationsUnderground;

                            stream >> tile.burrowId;
                            stream >> tile.tileOwner;

                            qint32 arrayLength32;
                            stream >> arrayLength32;

                            QVector<qint32> wombatIds;
                            qint32 temp32;

                            for (int m=0;m<arrayLength32;m++){
                                stream >> temp32;
                                wombatIds.append(temp32);
                            }

                            tile.wombatIds = wombatIds;

                            sector.tiles[i][j][k][l] = tile;
                        }
                    }
                }
            }
        }

        loadedSectors.append(sector);
        unloadOldSectors();
        worldBuilder->worker->consolePrint("Sector loaded: "+QString::number(sectorId.x())+","+QString::number(sectorId.y()));

        file.close();
    }
}



void World::unloadOldSectors() {
    if(loadedSectors.length()>maxSectorCount){
        int indexToRemove = None;
        qint64 oldestAccess = 9223372036854775807; //Max value for int64
        for (int i=0;i<loadedSectors.length();i++){
            if(loadedSectors.value(i).lastAccessed < oldestAccess){
                oldestAccess = loadedSectors.value(i).lastAccessed;
                indexToRemove = i;
            }
        }

        if(indexToRemove!=None){
            QPoint sectorId = loadedSectors.value(indexToRemove).id;
            saveSector(indexToRemove,sectorId);
            loadedSectors.remove(indexToRemove);
            worldBuilder->worker->consolePrint("Unloaded sector: "+QString::number(sectorId.x())+","+QString::number(sectorId.y()));
        }
    }
}

TileStruct& World::getTile(QPoint pos){
    QPoint sectorId = worldBuilder->qPointDivisionWithFloor(pos,sectorSizeInTiles);
    QPoint posWithinSector = worldBuilder->qPointModulo(pos,sectorSizeInTiles);
    if(posWithinSector.x()<0){ //There are no negative array positions within the sector
        posWithinSector.rx() = sectorSizeInTiles.x()+posWithinSector.x();
    }
    if(posWithinSector.y()<0){
        posWithinSector.ry() = sectorSizeInTiles.y()+posWithinSector.y();
    }
    QPoint chunkIndex = worldBuilder->qPointDivisionWithFloor(posWithinSector,chunkSize);
    QPoint tileIndex = worldBuilder->qPointModulo(posWithinSector,chunkSize);
/*
    worldBuilder->worker->consolePrint("input pos: "+QString::number(pos.x())+","+QString::number(pos.y()));
    worldBuilder->worker->consolePrint("sectorId: "+QString::number(sectorId.x())+","+QString::number(sectorId.y()));
    worldBuilder->worker->consolePrint("chunk index: "+QString::number(chunkIndex.x())+","+QString::number(chunkIndex.y()));
    worldBuilder->worker->consolePrint("tile index: "+QString::number(tileIndex.x())+","+QString::number(tileIndex.y()));
*/

    int sectorIndex = getSectorIndex(sectorId);

    if(sectorIndex == None){
        loadSector(sectorId);
        sectorIndex = loadedSectors.length()-1;
    }

    bool doesChunkExist = loadedSectors.value(sectorIndex).doesChunkExist.value(chunkIndex.x()).value(chunkIndex.y());
    if (doesChunkExist==false){
        QPoint chunkPos = worldBuilder->qPointDivisionWithFloor(pos,chunkSize);
        WorldGenerator worldGenerator;
        worldGenerator.setup(worldBuilder,this,level,global);
        loadedSectors[sectorIndex].tiles[chunkIndex.x()][chunkIndex.y()] = worldGenerator.createChunk(chunkPos);
        loadedSectors[sectorIndex].doesChunkExist[chunkIndex.x()][chunkIndex.y()] = true;
        //worldBuilder->worker->consolePrint("Created chunk "+QString::number(chunkIndex.x())+","+QString::number(chunkIndex.y())+" in sector "+QString::number(sectors.value(sectorIndex).id.x())+","+QString::number(sectors.value(sectorIndex).id.y()));
    }
    loadedSectors[sectorIndex].lastAccessed = QDateTime::currentMSecsSinceEpoch();
    return loadedSectors[sectorIndex].tiles[chunkIndex.x()][chunkIndex.y()][tileIndex.x()][tileIndex.y()];
}

int World::getSectorIndex(QPoint sectorId){
    int sectorIndex = None;
    for (int i=0;i<loadedSectors.length();i++){
        if(sectorId == loadedSectors.value(i).id){
            sectorIndex = i;
            break;
        }
    }
    return sectorIndex;
}

QPoint World::getChunkSize(){
    return chunkSize;
}
