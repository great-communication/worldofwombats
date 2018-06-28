#include "worldbuilder.h"
#include <QFileInfo>
#include <QTextStream>
#include <iostream>
#include <QBitArray>
#include <QtMath>
#include "worker.h"
#include "registry.h"
#include "herder.h"
#include "level.h"
#include "world.h"
#include "imagepainter.h"

WorldBuilder::WorldBuilder(QObject *parent) : QObject(parent){

}

int WorldBuilder::getRandomNumber(const int min, const int max){
    return ((qrand() % ((max + 1) - min)) + min);
}

void WorldBuilder::setup(QString levelName, QSize backgroundSize){
    isUnderground = false;
    worldPosition = QPoint(0,0);        
    positionsChanged = QPoint(0,0);
    month = 1;  //Start month
    timeSpeed = 20000; //Length of each month in milliseconds
    gameOver = false;

    qsrand(static_cast<uint>(QTime::currentTime().msec()));
    getRandomNumber(1, 100);    

    level = new Level(this);
    level->setup(this, levelName);
    worker->setViewPosition(worldPosition);
    worker->setUnderground(isUnderground);

    world = new World(this);
    world->setup(this, level, global);

    imagePainter = new ImagePainter(this);
    imagePainter->setup(global, world, this, backgroundSize);

    //gui->setMoveMaxMin();
    clearVisibleTiles();    

    registry = new Registry(this);
    registry->worldBuilder = this;
    registry->global = global;
    registry->worker = worker;

    herder = new Herder(this);
    herder->worldBuilder = this;
    herder->worker = worker;
    herder->registry = registry;

    registry->herder = herder;

    connect(this,SIGNAL(s_newMonth()),registry,SLOT(newMonth()));
    registry->setup();

    updateAllVisibleTiles();
    updateNeighboursArray(worldPosition);

    QImage backgroundImage = imagePainter->prepareBackgroundImage(isUnderground);
    // For some reason I can not initilize openGL at this point, so just put it on a timer.
    QTimer::singleShot(100, [=] {
            worker->updateTexture(positionsChanged, backgroundImage, isUnderground);
        });

    //Ugly fix
    int burrowId = getBurrowId(worldPosition);
    if (burrowId!=-1){
        int burrowIndex = registry->getBurrowIndex(burrowId);
        QString name = registry->burrows.value(burrowIndex).name;
        worker->setLocationText(name);
        worker->setUnderground(isUnderground);
        //    if(!hwGameUI->burrowMenuOpen){
        //        hwGameUI->showBurrowMenu();
        //    }       
    }
    //

    if (gameOver){
        worker->setLocationText(QString("GAME OVER!"));
        worker->setLockMovement(true);
        worker->setSelectedWombatVisibility(false);
    }else{        
        timerNewMonth = new QTimer(this);
        connect(timerNewMonth, SIGNAL(timeout()), this, SLOT(newMonth()));
        timerNewMonth->start(timeSpeed);
    }
}

void WorldBuilder::setGameOver(){
    gameOver=true;
    timerNewMonth->stop();
    worker->setLockMovement(true);
    worker->playSoundFX(Sound_GameOver);
}

void WorldBuilder::clearVisibleTiles(){
    QPoint chunkSize = world->getChunkSize();
    visibleTiles.clear();

    for (int i=0;i<chunkSize.x();i++){
        QVector<VisibleTileStruct> column;
        for (int j=0;j<chunkSize.y();j++){
            VisibleTileStruct tile;
            tile.terrainType = NotLoaded;
            column.append(tile);
        }
        visibleTiles.append(column);
    }
}




bool WorldBuilder::fileExists(QString path) {
    QFileInfo check_file(path);
    // Check if file exists and if yes: Is it really a file and no directory?
    if (check_file.exists() && check_file.isFile()) {
        return true;
    } else {
        return false;
    }
}



void WorldBuilder::prepareToUpdateTiles(QPoint viewPosition, bool isUnderground){
    //bool undergroundChanged = false;
    //if(this->underground != underground){
    //    undergroundChanged = true;
    //}
    this->isUnderground = isUnderground;
    positionsChanged = viewPosition-worldPosition;
    removeTiles(viewPosition);           // Remove tiles no longer visible
    setWorldPosition(viewPosition);
    //hwHerder->removeStorageTileFromHerd(); // blablabla !

    int wombatCount = herder->wombatHerd.length();

    updateAllVisibleTiles();
    updateNeighboursArray(viewPosition);   

    QImage backgroundImage = imagePainter->prepareBackgroundImage(isUnderground);
    worker->updateTexture(positionsChanged, backgroundImage, isUnderground);

    herder->updateHerd(worldPosition,wombatCount);

    //if(undergroundChanged){
    //    herder->updateVisibility();
    //}
}

void WorldBuilder::updateNeighboursArray(QPoint viewPosition){
    //The neighbours array is used by isBlocked() and others in the GUI
    QVector<MoveStruct> neighbours;
    for (int i=-1;i<2;i++){
        for (int j=-1;j<2;j++){
            QPoint pos = QPoint(viewPosition.x()+i,viewPosition.y()+j);
            VisibleTileStruct &visibleTile = getVisibleTile(pos);
            MoveStruct neighbour;
            neighbour.pos = pos;
            neighbour.tileType = (Terrain)visibleTile.terrainType;
            neighbour.tileTypeUnderground = (Terrain)visibleTile.terrainTypeUnderground;
            neighbour.burrowId = visibleTile.burrowId;
            neighbour.overlayTerrainTypes = visibleTile.overlayTerrainTypes;
            neighbour.overlayTerrainTypesUnderground = visibleTile.overlayTerrainTypesUnderground;
            neighbours.append(neighbour);
        }
    }

    QVector<QPoint> extraTilesNeeded; //Fix for dig underground.....
    extraTilesNeeded.append(QPoint(2,0));
    extraTilesNeeded.append(QPoint(0,2));
    extraTilesNeeded.append(QPoint(-2,0));
    extraTilesNeeded.append(QPoint(0,-2));

    for (int i=0;i<extraTilesNeeded.length();i++){
        QPoint pos = QPoint(viewPosition.x()+extraTilesNeeded.value(i).x(),viewPosition.y()+extraTilesNeeded.value(i).y());
        VisibleTileStruct &visibleTile = getVisibleTile(pos);
        MoveStruct neighbour;
        neighbour.pos = pos;
        neighbour.tileType = (Terrain)visibleTile.terrainType;
        neighbour.tileTypeUnderground = (Terrain)visibleTile.terrainTypeUnderground;
        neighbour.burrowId = visibleTile.burrowId;
        neighbour.overlayTerrainTypes = visibleTile.overlayTerrainTypes;
        neighbour.overlayTerrainTypesUnderground = visibleTile.overlayTerrainTypesUnderground;
        neighbours.append(neighbour);
    }

    worker->updateNeighboursArray(neighbours);
}

void WorldBuilder::prepareToUpdateTilesForTeleport(QPoint targetPos, bool isUnderground){
    this->isUnderground = isUnderground;
    worker->consolePrint("targetpos "+QString::number(targetPos.x())+","+QString::number(targetPos.y()));
    herder->removeAllWombatsFromHerd();
    herder->removeAllStorageTilesFromHerd();
    clearVisibleTiles();           // Remove all tiles
    setWorldPosition(targetPos);
    updateAllVisibleTiles();
    updateNeighboursArray(targetPos);
    QImage backgroundImage = imagePainter->prepareBackgroundImage(isUnderground);
    worker->updateTexture(QPoint(0,0), backgroundImage, isUnderground);
    worker->setLockMovement(false);
    worker->setLockInput(false);
}



void WorldBuilder::removeTiles(QPoint pos){
    //Remove tiles no longer visible
    QPoint chunkSize = world->getChunkSize();
    if (pos.x()<worldPosition.x()){
        for (int i=0;i<worldPosition.x()-pos.x();i++){
            visibleTiles.pop_back();            //Remove old tile
            QVector<VisibleTileStruct> column;
            for (int j=0;j<chunkSize.y();j++){
                VisibleTileStruct tile;                //Create new empty tile
                tile.terrainType = NotLoaded;
                column.append(tile);
            }
            visibleTiles.prepend(column);            
        }
    }else if(pos.x()>worldPosition.x()){
        for (int i=0;i<pos.x()-worldPosition.x();i++){
            visibleTiles.pop_front();            
            QVector<VisibleTileStruct> column;
            for (int j=0;j<chunkSize.y();j++){
                VisibleTileStruct tile;
                tile.terrainType = NotLoaded;
                column.append(tile);
            }
            visibleTiles.append(column);
        }

    }

    if(pos.y()<worldPosition.y()){
        for (int i=0;i<worldPosition.y()-pos.y();i++){
            for (int j=0;j<chunkSize.x();j++){
                QVector<VisibleTileStruct> column;
                column = visibleTiles[j];
                column.pop_back();
                VisibleTileStruct tile;
                tile.terrainType = NotLoaded;
                column.prepend(tile);
                visibleTiles[j]=column;
            }
        }
    }else if(pos.y()>worldPosition.y()){
        for (int i=0;i<pos.y()-worldPosition.y();i++){
            for (int j=0;j<chunkSize.x();j++){
                QVector<VisibleTileStruct> column;
                column = visibleTiles[j];
                column.pop_front();
                VisibleTileStruct tile;
                tile.terrainType = NotLoaded;
                column.append(tile);
                visibleTiles[j]=column;
            }
        }
    }
}

void WorldBuilder::setWorldPosition(QPoint pos){
    worldPosition = pos;
}

void WorldBuilder::updateAllVisibleTiles(){
    QPoint chunkSize = world->getChunkSize();
    for (int i=0;i<chunkSize.x();i++){
        for (int j=0;j<chunkSize.y();j++){
            updateTile(QPoint(i,j));
        }
    }
}

void WorldBuilder::updateTile(QPoint indexes){
    int i = indexes.x();
    int j = indexes.y();
    QPoint posOffset = QPoint(indexes.x()-16,indexes.y()-16); //WorldPos is always at visibleTiles index 16,16
    QPoint pos = worldPosition+posOffset;

    QVector<int> miniTileNumbers;
    QVector<int> miniTileNumbersUnderground;
    VisibleTileStruct tile;

    if (visibleTiles[i][j].terrainType==NotLoaded){
        TileStruct &query = world->getTile(pos);

        if (query.terrain == Bush || query.terrain == Grass){
            miniTileNumbers = calculateMiniTileNumbers(query.terrain,pos,false,false);
            miniTileNumbersUnderground.append(1); //1,3,7,9 are miniTileNumbers for the standard corner pieces
            miniTileNumbersUnderground.append(3);
            miniTileNumbersUnderground.append(7);
            miniTileNumbersUnderground.append(9);

        }else{
            miniTileNumbers.append(1);
            miniTileNumbers.append(3);
            miniTileNumbers.append(7);
            miniTileNumbers.append(9);

        }
        if (query.terrainUnderground==Tunnel || query.terrainUnderground==Burrow){
            miniTileNumbersUnderground = calculateMiniTileNumbers(query.terrainUnderground,pos,false,false);
        }else{
            miniTileNumbersUnderground=miniTileNumbers;
        }

        //Overlays
        bool hasGrass = false;
        bool hasRiceGrass = false;
        QVector<int> overlays; //Needed to convert quint8 to qint32?
        QVector<int> variations;
        QVector<QVector<int>> overlayMiniTileNumbers;
        if (query.overlay.length()>0){
            for (int k=0;k<query.overlay.length();k++){
                overlays.append(query.overlay.value(k));
                variations.append(query.overlayVariations.value(k));
                if(query.overlay.value(k)==Corpse || query.overlay.value(k)==Cross){
                    QVector<int> rrre;
                    rrre.append(1);
                    rrre.append(3);
                    rrre.append(7);
                    rrre.append(9);
                    overlayMiniTileNumbers.append(rrre);
                }else if(query.overlay.value(k)==Path){ // TempFix
                    QVector<int> rrre;
                    rrre.append(pos.x());
                    rrre.append(pos.y());
                    rrre.append(pos.x());
                    rrre.append(pos.y());
                    overlayMiniTileNumbers.append(rrre);
                }else if(query.overlay.value(k)==RiceGrass){
                    hasRiceGrass = true;
                }else if(query.overlay.value(k)==Grass){
                    hasGrass = true;
                }else{
                    QVector<int> rrre = calculateMiniTileNumbers(query.overlay.value(k),pos,true,false);
                    overlayMiniTileNumbers.append(rrre);
                }
            }
        }

        QVector<int> overlaysUnderground;
        QVector<int> variationsUnderground;
        QVector<QVector<int>> overlayMiniTileNumbersUnderground;       
        if (query.overlayUnderground.length()>0){
            for (int k=0;k<query.overlayUnderground.length();k++){
                overlaysUnderground.append(query.overlayUnderground.value(k));
                variationsUnderground.append(query.overlayVariationsUnderground.value(k));
                if(query.overlayUnderground.value(k)==Corpse || query.overlayUnderground.value(k)==Cross || query.overlayUnderground.value(k)==HoleLight){
                    QVector<int> rrre;
                    rrre.append(1);
                    rrre.append(3);
                    rrre.append(7);
                    rrre.append(9);
                    overlayMiniTileNumbersUnderground.append(rrre);
                }else{
                    QVector<int> rrre = calculateMiniTileNumbers(query.overlayUnderground.value(k),pos,true,true);
                    overlayMiniTileNumbersUnderground.append(rrre);                    
                }
            }
        }


        tile.terrainType = query.terrain;
        tile.variation = query.terrainVariation;
        tile.terrainTypeUnderground = query.terrainUnderground;
        tile.miniTileNumbers=miniTileNumbers;
        tile.miniTileNumbersUnderground=miniTileNumbersUnderground;
        tile.rotation = 0;        
        tile.overlayTerrainTypes = overlays;
        tile.overlayVariations = variations;
        tile.overlayMiniTileNumbers = overlayMiniTileNumbers;
        tile.overlayTerrainTypesUnderground = overlaysUnderground;
        tile.overlayVariationsUnderground = variationsUnderground;
        tile.overlayMiniTileNumbersUnderground = overlayMiniTileNumbersUnderground;
        tile.burrowId = query.burrowId;


        if(hasRiceGrass){
            if (visibleTiles[i][j].riceGrassImagePos.length()>0){
                tile.riceGrassImagePos = visibleTiles[i][j].riceGrassImagePos;
                tile.riceGrassImageIndex = visibleTiles[i][j].riceGrassImageIndex;
            }
        }

        if(hasGrass){
            if (visibleTiles[i][j].grassImagePos.length()>0){
                tile.grassImagePos = visibleTiles[i][j].grassImagePos;
                tile.grassImageIndex = visibleTiles[i][j].grassImageIndex;
            }
        }

        visibleTiles[i][j] = tile;

        for (int k=0;k<tile.overlayTerrainTypesUnderground.length();k++){
            if(tile.overlayTerrainTypesUnderground.value(k) == Storage){
                int burrowId = getBurrowId(pos);
                herder->addStorageTileToHerd(burrowId,pos);
                break;
            }
        }

        if (query.wombatIds.length()>0){ //Check if there are wombats on this tile            
            for (int k=0;k<query.wombatIds.length();k++){ //Loop through all wombats on this tile
                //Check if Herder is already managing wombat                
                if (registry->selectedWombat != query.wombatIds.value(k)){ //Do not add selected wombat to heard
                    int index = herder->wombatIds.indexOf(query.wombatIds.value(k));
                    if (index==None){
                        herder->addWombatToHerd(query.wombatIds.value(k),pos,QPoint(0,0),0);
                    }
                }
            }
        }
    }
}

//Each tile is made out of 4 minitiles which are chosen so that they line up correctly with neigbouring tiles
QVector<int> WorldBuilder::calculateMiniTileNumbers(int terrainType, QPoint pos, bool overlay, bool underground){
    QBitArray sameType(8);
    int count = 0;
    int queryIndex = 0;
    int terrainType2 = -1;
    int terrainType3 = -1;
    if (terrainType==Tunnel){
        queryIndex = 2;
        terrainType2=HoleLight;
    }else if(terrainType==HoleLight){
        queryIndex = 2;
        terrainType2=Tunnel;
    }else if (terrainType==Burrow){
        queryIndex = 2;
        terrainType2=HoleLight;
        terrainType3=Storage;
    }

    // Check if neighbours same type
    for (int i=-1;i<2;i++){ //Loop through columns
        for (int j=-1;j<2;j++){ //Loop through rows
            if (!(i==0 && j==0)){ //ignore self (pos 0,0)
                TileStruct &query = world->getTile(pos+QPoint(i,j));

                //Clean this shit up
                if (overlay){                    
                    bool sameOverlay = false;
                    if(underground){
                        for (int k=0;k<query.overlayUnderground.length();k++){

                            if (terrainType==query.overlayUnderground.value(k)){
                                sameOverlay = true;
                                break;
                            }
                        }
                    }else{
                        for (int k=0;k<query.overlay.length();k++){

                            if (terrainType==query.overlay.value(k)){
                                sameOverlay = true;
                                break;
                            }
                        }
                    }

                    if (sameOverlay){
                        sameType.setBit(count,true);
                        count++;
                    }else{
                        sameType.setBit(count,false);
                        count++;
                    }


                }else{

                    if (queryIndex == 0){
                        if (terrainType==query.terrain||terrainType2==query.terrain||terrainType3==query.terrain){
                            sameType.setBit(count,true);
                            count++;
                        }else{
                            sameType.setBit(count,false);
                            count++;
                        }

                    }else if (queryIndex == 2){
                        if (terrainType==query.terrainUnderground||terrainType2==query.terrainUnderground||terrainType3==query.terrainUnderground){
                            sameType.setBit(count,true);
                            count++;
                        }else{
                            sameType.setBit(count,false);
                            count++;
                        }

                    }

                }
            }
        }
    }   

    //Set bit codes
    QBitArray miniTileA(4);
    if (sameType.at(North)==1 && sameType.at(West)==1 && sameType.at(NorthWest)==0){  //Check if edge tile or corner tile
        miniTileA.setBit(0, CornerTile);
        miniTileA.setBit(1, sameType.at(North));
        miniTileA.setBit(2, sameType.at(West));
        miniTileA.setBit(3, sameType.at(NorthWest));
    }else{
        miniTileA.setBit(0, EdgeTile);
        miniTileA.setBit(1, EdgeTile);
        miniTileA.setBit(2, sameType.at(North));
        miniTileA.setBit(3, sameType.at(West));
    }

    QBitArray miniTileB(4);
    if (sameType.at(North)==1 && sameType.at(East)==1 && sameType.at(NorthEast)==0){  //Check if edge tile or corner tile
        miniTileB.setBit(0, sameType.at(East));
        miniTileB.setBit(1, CornerTile);
        miniTileB.setBit(2, sameType.at(NorthEast));
        miniTileB.setBit(3, sameType.at(North));
    }else{
        miniTileB.setBit(0, sameType.at(East));
        miniTileB.setBit(1, EdgeTile);
        miniTileB.setBit(2, sameType.at(North));
        miniTileB.setBit(3, EdgeTile);
    }

    QBitArray miniTileC(4);
    if (sameType.at(South)==1 && sameType.at(West)==1 && sameType.at(SouthWest)==0){  //Check if edge tile or corner tile
        miniTileC.setBit(0, CornerTile);
        miniTileC.setBit(1, sameType.at(SouthWest));
        miniTileC.setBit(2, CornerTile);
        miniTileC.setBit(3, sameType.at(South));
    }else{
        miniTileC.setBit(0, EdgeTile);
        miniTileC.setBit(1, sameType.at(South));
        miniTileC.setBit(2, EdgeTile);
        miniTileC.setBit(3, sameType.at(West));
    }

    QBitArray miniTileD(4);
    if (sameType.at(South)==1 && sameType.at(East)==1 && sameType.at(SouthEast)==0){  //Check if edge tile or corner tile
        miniTileD.setBit(0, sameType.at(SouthEast));
        miniTileD.setBit(1, CornerTile);
        miniTileD.setBit(2, sameType.at(South));
        miniTileD.setBit(3, CornerTile);
    }else{
        miniTileD.setBit(0, sameType.at(East));
        miniTileD.setBit(1, sameType.at(South));
        miniTileD.setBit(2, EdgeTile);
        miniTileD.setBit(3, EdgeTile);
    }

    //Convert bitcodes to miniTileNumbers
    QVector<int> miniTileNumbers;
    miniTileNumbers.append(reMap(miniTileA));    
    miniTileNumbers.append(reMap(miniTileB));   
    miniTileNumbers.append(reMap(miniTileC));    
    miniTileNumbers.append(reMap(miniTileD));    

    return miniTileNumbers;
}

int WorldBuilder::reMap(QBitArray miniTile){
    QString code;    
    int miniTileNumber;
    for (int i=0;i<4;i++){     
        code = code+QString::number(miniTile.at(i));
    }

    if (code == "1100"){
        miniTileNumber = 1;
    }else if (code == "1110"){
        miniTileNumber = 4;
    }else if (code == "1010"){
        miniTileNumber = 7;
    }else if (code == "1101"){
        miniTileNumber = 2;
    }else if (code == "1111"){
        miniTileNumber = 5;
    }else if (code == "1011"){
        miniTileNumber = 8;
    }else if (code == "0101"){
        miniTileNumber = 3;
    }else if (code == "0111"){
        miniTileNumber = 6;
    }else if (code == "0011"){
        miniTileNumber = 9;
    }else if (code == "0010"){
        miniTileNumber = 10;
    }else if (code == "1001"){
        miniTileNumber = 12;
    }else if (code == "0001"){
        miniTileNumber = 11;
    }else if (code == "0110"){
        miniTileNumber = 13;
    }/*else if (code == "0100"){
        tile = 15;
    }else if (code == "1000"){
        tile = 16;
    }*/else {
        miniTileNumber =0;
    } 
    return miniTileNumber;
}

void WorldBuilder::changeTerrain(QPoint pos, Terrain terrain, int burrowId, bool update){
    TileStruct &tile = world->getTile(pos);

    if (terrain == Terrain_Hole){
        if (tile.terrainUnderground == Terrain_Burrow){
            registry->addEntrancePosToBurrow(burrowId,pos);
            removeOverlayFromTile(pos,Overlay_RiceGrass,1);
            removeOverlayFromTile(pos,Overlay_Grass,1);
            removeOverlayFromTile(pos,Overlay_Path,1);
            addOverlayToTileUnderground(pos,Overlay_HoleLight,1);
            tile.terrain = (quint8)Terrain_Hole;
        }else{            
            if (burrowId == None){
                tile.terrainUnderground = (quint8)Terrain_Tunnel;
            }else{
                registry->addEntrancePosToBurrow(burrowId,pos);
                tile.terrainUnderground = (quint8)Terrain_Burrow;
                tile.burrowId = (qint32)burrowId;
            }
            removeOverlayFromTile(pos,Overlay_RiceGrass,1);
            removeOverlayFromTile(pos,Overlay_Grass,1);
            removeOverlayFromTile(pos,Overlay_Path,1);
            addOverlayToTileUnderground(pos,Overlay_HoleLight,1);
            tile.terrain = (quint8)Terrain_Hole;
        }

    }else if (terrain==Terrain_Tunnel){
        tile.terrainUnderground = (quint8)Terrain_Tunnel;
    }else if (terrain==Terrain_Burrow){
        tile.terrainUnderground = (quint8)terrain;
        tile.burrowId = (qint32)burrowId;
    }else{
        tile.terrain = (quint8)terrain;
    }

    if (update){ //Mark tiles for reloading so that they are updated
        for (int i=-1;i<2;i++){  // Probably you should make sure these indexes are valid? Is it possible to change a tile at the edge of the visibleTiles? Maybe not...
            for (int j=-1;j<2;j++){
                VisibleTileStruct &visibleTile = getVisibleTile(QPoint(pos.x()+i,pos.y()+j));
                visibleTile.terrainType = NotLoaded;                
            }
        }
    }
}

void WorldBuilder::addOverlayToTile(QPoint pos, Overlay overlay, int variation){
    TileStruct &tile = world->getTile(pos);

    bool hasThisOverlay = false;
    //Check if tile already has this overlay
    for (int i=0;i<tile.overlay.length();i++){
        if (tile.overlay.value(i) == overlay){
            tile.overlayVariations[i] = (quint8)variation; // If already added, just update variation
            hasThisOverlay = true;
            break;
        }
    }

    if (!hasThisOverlay){
        int zIndex = global->getZIndex(overlay);
        bool isInserted = false;
        for (int i=0;i<tile.overlay.length();i++){   //Insert on correct pos in array according to zIndex
            int zIndex2 = global->getZIndex((Overlay)tile.overlay.value(i));
            if (zIndex<zIndex2){
                tile.overlay.insert(i,(quint8)overlay);
                tile.overlayVariations.insert(i,(quint8)variation);
                isInserted = true;
                break;
            }
        }

        if (!isInserted){
            tile.overlay.append((quint8)overlay);
            tile.overlayVariations.append((quint8)variation);
        }
    }else{        
        worker->consolePrint("Error addOverlayToTile");
    }

    for (int i=-1;i<2;i++){
        for (int j=-1;j<2;j++){
            VisibleTileStruct &visibleTile = getVisibleTile(QPoint(pos.x()+i,pos.y()+j));
            visibleTile.terrainType = NotLoaded;
        }
    }    
}

void WorldBuilder::removeOverlayFromTile(QPoint pos, Overlay overlay, int variation){
    TileStruct &tile = world->getTile(pos);

    int toBeRemoved = -1;
    for (int i=0;i<tile.overlay.length();i++){ //Check if overlay exists
        if ((Overlay)tile.overlay.value(i) == overlay){
            //tile.overlayTerrainVariationsUnderground[i] = (quint8)variation; // If already added, just update variation
            toBeRemoved = i;
            break;
        }
    }

    if (toBeRemoved!=-1){
        tile.overlay.remove(toBeRemoved);
        tile.overlayVariations.remove(toBeRemoved);

        for (int i=-1;i<2;i++){  // Probably you should make sure these indexes are valid? Is it possible to change a tile at the edge of the visibleTiles? Maybe not...
            for (int j=-1;j<2;j++){
                VisibleTileStruct &visibleTile = getVisibleTile(QPoint(pos.x()+i,pos.y()+j));
                visibleTile.terrainType = NotLoaded;
            }
        }
    }    
}

void WorldBuilder::addOverlayToTileUnderground(QPoint pos, Overlay overlay, int variation){
    TileStruct &tile = world->getTile(pos);

    bool hasThisOverlay = false;
    //Check if tile already has this overlay
    for (int i=0;i<tile.overlayUnderground.length();i++){
        if (tile.overlayUnderground.value(i) == overlay){
            tile.overlayVariationsUnderground[i] = (quint8)variation; // If already added, just update variation
            hasThisOverlay = true;
            break;
        }
    }

    if (!hasThisOverlay){
        int zIndex = global->getZIndex(overlay);
        bool isInserted = false;
        for (int i=0;i<tile.overlayUnderground.length();i++){ //Insert on correct pos in array according to zIndex
            int zIndex2 = global->getZIndex((Overlay)tile.overlayUnderground.value(i));
            if (zIndex<zIndex2){
                tile.overlayUnderground.insert(i,(quint8)overlay);
                tile.overlayVariationsUnderground.insert(i,(quint8)variation);
                isInserted = true;
                break;
            }
        }

        if (!isInserted){
            tile.overlayUnderground.append((quint8)overlay);
            tile.overlayVariationsUnderground.append((quint8)variation);
        }

        for (int i=-1;i<2;i++){  // Probably you should make sure these indexes are valid? Is it possible to change a tile at the edge of the visibleTiles? Maybe not...
            for (int j=-1;j<2;j++){
                VisibleTileStruct &visibleTile = getVisibleTile(QPoint(pos.x()+i,pos.y()+j));
                visibleTile.terrainType = NotLoaded;
            }
        }
    }    
}

void WorldBuilder::removeOverlayFromTileUnderground(QPoint pos, Overlay overlay, int variation){
    TileStruct &tile = world->getTile(pos);

    int toBeRemoved = -1;
    for (int i=0;i<tile.overlayUnderground.length();i++){ //Check if overlay exists
        if ((Overlay)tile.overlayUnderground.value(i) == overlay){
            //tile.overlayTerrainVariationsUnderground[i] = variation; // If already added, just update variation
            toBeRemoved = i;
            break;
        }
    }

    if (toBeRemoved!=-1){
        tile.overlayUnderground.remove(toBeRemoved);
        tile.overlayVariationsUnderground.remove(toBeRemoved);

        for (int i=-1;i<2;i++){
            for (int j=-1;j<2;j++){
                VisibleTileStruct &visibleTile = getVisibleTile(QPoint(pos.x()+i,pos.y()+j));
                visibleTile.terrainType = NotLoaded;
            }
        }
    }    
}

bool WorldBuilder::hasOverlay(QPoint pos, Overlay overlay, bool isUnderground){
    TileStruct &tile = world->getTile(pos);

    if(!isUnderground){
        for (int i=0;i<tile.overlay.length();i++){
            if ((Overlay)tile.overlay.value(i) == overlay){
                return true;
            }
        }
    }else{
        for (int i=0;i<tile.overlayUnderground.length();i++){
            if ((Overlay)tile.overlayUnderground.value(i) == overlay){
                return true;
            }
        }
    }

    return false;
}

int WorldBuilder::getOverlayVariation(QPoint pos, Overlay overlay, bool underground){
    TileStruct &tile = world->getTile(pos);

    if(!underground){
        for (int i=0;i<tile.overlay.length();i++){
            if ((Overlay)tile.overlay.value(i) == overlay){
                return tile.overlayVariations.value(i);
            }
        }
    }else{
        for (int i=0;i<tile.overlayUnderground.length();i++){
            if ((Overlay)tile.overlayUnderground.value(i) == overlay){
                return tile.overlayVariationsUnderground.value(i);
            }
        }
    }

    return None;
}

void WorldBuilder::addOverlayToVisibleTile(QPoint pos, Overlay overlay, int variation){
    //Minitile numbers are never calculated?
    VisibleTileStruct &tile = getVisibleTile(pos);

    bool hasThisOverlay = false;
    for (int i=0;i<tile.overlayTerrainTypes.length();i++){   //Check if already added
        if (tile.overlayTerrainTypes.value(i) == overlay){
            tile.overlayVariations[i] = variation; // If already added, just update variation
            hasThisOverlay = true;
            break;
        }
    }

    if (hasThisOverlay==false){
        int zIndex = global->getZIndex(overlay);
        bool inserted = false;
        for (int i=0;i<tile.overlayTerrainTypes.length();i++){   //Insert on correct pos in array according to zIndex
            int zIndex2 = global->getZIndex((Overlay)tile.overlayTerrainTypes.value(i));
            if (zIndex<zIndex2){
                tile.overlayTerrainTypes.insert(i,(int)overlay);
                tile.overlayVariations.insert(i,(int)variation);
                inserted = true;
                break;
            }
        }

        if (!inserted){
            tile.overlayTerrainTypes.append((int)overlay);
            tile.overlayVariations.append((int)variation);            
        }
    }else{
        worker->consolePrint("Error addOverlayToVisibleTile");
    }
}

void WorldBuilder::addOverlayToVisibleTileUnderground(QPoint pos, Overlay overlay, int variation, QVector<int> miniTileNumbers){
    VisibleTileStruct &tile = getVisibleTile(pos);

    bool hasThisOverlay = false;
    for (int i=0;i<tile.overlayTerrainTypesUnderground.length();i++){   //Check if already added
        if (tile.overlayTerrainTypesUnderground.value(i) == overlay){
            tile.overlayVariationsUnderground[i] = variation; // If already added, just update variation
            hasThisOverlay = true;
            break;
        }
    }

    if (hasThisOverlay==false){
        int zIndex = global->getZIndex(overlay);
        bool inserted = false;
        for (int i=0;i<tile.overlayTerrainTypesUnderground.length();i++){   //Insert on correct pos in array according to zIndex
            int zIndex2 = global->getZIndex((Overlay)tile.overlayTerrainTypesUnderground.value(i));
            if (zIndex<zIndex2){
                tile.overlayTerrainTypesUnderground.insert(i,(int)overlay);
                tile.overlayVariationsUnderground.insert(i,(int)variation);
                if(overlay == RiceGrass_Bundle04){//Temp!!! fix
                    tile.overlayMiniTileNumbersUnderground.insert(i,miniTileNumbers);
                }else if(overlay == Grass_Bundle01){//Temp!!! fix
                    tile.overlayMiniTileNumbersUnderground.insert(i,miniTileNumbers);
                }else if(overlay == Sticks_Bundle01){//Temp!!! fix
                    tile.overlayMiniTileNumbersUnderground.insert(i,miniTileNumbers);
                }

                inserted = true;
                break;
            }
        }

        if (!inserted){
            tile.overlayTerrainTypesUnderground.append((int)overlay);
            tile.overlayVariationsUnderground.append((int)variation);
            if(overlay == RiceGrass_Bundle04){//Temp!!! fix
                tile.overlayMiniTileNumbersUnderground.append(miniTileNumbers);
            }else if(overlay == Grass_Bundle01){//Temp!!! fix
                tile.overlayMiniTileNumbersUnderground.append(miniTileNumbers);
            }else if(overlay == Sticks_Bundle01){//Temp!!! fix
                tile.overlayMiniTileNumbersUnderground.append(miniTileNumbers);
            }
        }
    }else{
        worker->consolePrint("Error addOverlayToVisibleTileUnderground");
    }
}

void WorldBuilder::removeOverlayFromVisibleTile(QPoint pos, Overlay overlay, int variation){
    VisibleTileStruct &tile = getVisibleTile(pos);

    int toBeRemoved = -1;
    for (int i=0;i<tile.overlayTerrainTypes.length();i++){ //Check if overlay exists
        if (tile.overlayTerrainTypes.value(i) == overlay){
            toBeRemoved = i;
            break;
        }
    }

    if (toBeRemoved!=-1){
        tile.overlayTerrainTypes.remove(toBeRemoved);
        tile.overlayVariations.remove(toBeRemoved);
    }
}

void WorldBuilder::removeOverlayFromVisibleTileUnderground(QPoint pos, Overlay overlay, int variation){
    VisibleTileStruct &tile = getVisibleTile(pos);

    int toBeRemoved = -1;
    for (int i=0;i<tile.overlayTerrainTypesUnderground.length();i++){ //Check if overlay exists
        if (tile.overlayTerrainTypesUnderground.value(i) == overlay){
            toBeRemoved = i;
            break;
        }
    }

    if (toBeRemoved!=-1){
        tile.overlayTerrainTypesUnderground.remove(toBeRemoved);
        tile.overlayVariationsUnderground.remove(toBeRemoved);
        tile.overlayMiniTileNumbersUnderground.remove(toBeRemoved);
    }
}


void WorldBuilder::addWombatIdToTile(QPoint pos, int wombatId){
    TileStruct &tile = world->getTile(pos);
    tile.wombatIds.append((qint32)wombatId);
    //worker->consolePrint("adding wombat "+QString::number(wombatId)+" to tile "+QString::number(pos.x())+","+QString::number(pos.y()));
}

void WorldBuilder::removeWombatIdFromTile(QPoint pos, int wombatId){
    TileStruct &tile = world->getTile(pos);

    int index = tile.wombatIds.indexOf(wombatId);
    if(index!=None){
        tile.wombatIds.remove(index);
    }else{
        worker->consolePrint("Error removeWombatIdFromTile");
    }
    //worker->consolePrint("remove wombat "+QString::number(wombatId)+" from tile "+QString::number(pos.x())+","+QString::number(pos.y()));
}

bool WorldBuilder::hasTerrain(Terrain terrain, bool isUnderground, QPoint pos){
    TileStruct &tile = world->getTile(pos);
    Terrain tileTerrain = Terrain_None;

    if (!isUnderground){
        tileTerrain = (Terrain)tile.terrain;
    }else{
        tileTerrain = (Terrain)tile.terrainUnderground;
    }

    if(terrain == tileTerrain){
        return true;
    }else{
        return false;
    }
}

Terrain WorldBuilder::getTerrain(QPoint pos, bool isUnderground){
    TileStruct &tile = world->getTile(pos);

    if (!isUnderground){
        return (Terrain)tile.terrain;
    }else{
        return (Terrain)tile.terrainUnderground;
    }
}

int WorldBuilder::getBurrowId(QPoint pos){
    TileStruct &tile = world->getTile(pos);

    if (tile.terrainUnderground == Burrow){
        return tile.burrowId;
    }else{
        //Send back None if no burrow
        return None;
    }
}

QVector<int> WorldBuilder::getTileWombatIds(QPoint pos){
    TileStruct &tile = world->getTile(pos);
    if(tile.wombatIds.length()>0){
        return tile.wombatIds;
    }else{
        QVector<int> empty;
        empty.append(None);
        return empty;
    }
}

int WorldBuilder::getOwner(int x, int y){
    TileStruct &tile = world->getTile(QPoint(x,y));
    return tile.tileOwner;
}

void WorldBuilder::setOwner(int x, int y, int owner){
    TileStruct &tile = world->getTile(QPoint(x,y));
    tile.tileOwner = (qint32)owner;
}

QPoint WorldBuilder::qPointModulo(QPoint dividend, QPoint divisor){
    QPoint reminder;
    reminder.rx() = dividend.x() % divisor.x();
    reminder.ry() = dividend.y() % divisor.y();
    return reminder;
}

QPoint WorldBuilder::qPointMultiplication(QPoint factor, QPoint factor2){
    QPoint product;
    product.rx() = factor.x() * factor2.x();
    product.ry() = factor.y() * factor2.y();
    return product;
}

QPoint WorldBuilder::qPointDivisionWithFloor(QPoint numerator, QPoint denominator){
    QPoint quotient;
    if(denominator.x()!=0){ //Check that we are not dividing with zero
        quotient.rx() = qFloor((double)numerator.x() / (double)denominator.x());
    }else{
        quotient.rx() = 0;
    }
    if(denominator.y()!=0){ //Check that we are not dividing with zero
        quotient.ry() = qFloor((double)numerator.y() / (double)denominator.y());
    }else{
        quotient.ry() = 0;
    }
    return quotient;
}

void WorldBuilder::newMonth(){
    if (month==12){
        month = 0;
    }

    month++;
    emit s_newMonth(); //Is picked up by Registry and all HerdMembers
}

VisibleTileStruct& WorldBuilder::getVisibleTile(QPoint pos){
    QPoint chunkSize = world->getChunkSize();
    //worker->consolePrint("visible tile pos: "+QString::number(pos.x())+","+QString::number(pos.y()));
    QPoint index = (pos-worldPosition)+QPoint((chunkSize.x()/2),(chunkSize.y()/2)); // Middle of visibleTile array will always be worldPosition, so find offset and add that to arrayindex of middle
    //worker->consolePrint("visible tile index: "+QString::number(index.x())+","+QString::number(index.y()));
    if (index.x() >=0 && index.x() <chunkSize.x() && index.y() >=0 && index.y()<chunkSize.y()){
         return visibleTiles[index.x()][index.y()];
    }else{
        worker->consolePrint("Error getVisibleTile");
        return visibleTiles[15][15];
    }
}

void WorldBuilder::quitGame(QPoint viewPosition){
    setWorldPosition(viewPosition);
    level->save();
    registry->saveBurrowsToFile();
    registry->saveWombatsToFile();
    world->saveAllSectors();
}
