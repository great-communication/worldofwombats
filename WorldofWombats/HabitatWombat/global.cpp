#include "global.h"

int gFrameRate = 1000/60; // 16 fps
int gTileSize = 128; // 128px * 128px

int gStorageSize = 100; // Size per tile. So a 4 tile granary will hold 400 units
int gStorageBufferSize = 1000; //Old??? Remove? When the buffer is not full, it relies on herded wombats to empty it. If there are no herded wombats the buffer will fill up, and when it overflows the resources are added to the storages automatically.

int gWombatInventorySize = 100;




Global::Global(QObject *parent) : QObject(parent){

}

void Global::setup(){
    createBuildCostArray();
    createTileTypesArray();
    createResourcesArray();
}

void Global::createBuildCostArray(){
    BuildCost buildCost;
    buildCost.building = Overlay_Storage;
    QVector<InventoryItem> items;
    InventoryItem item;
    item.resource = Resource_Sticks;
    item.amount = 5;
    items.append(item);
    buildCost.itemsNeeded = items;
    gBuildCost.append(buildCost);

    BuildCost buildCost2;
    buildCost2.building = Overlay_Nest;
    QVector<InventoryItem> items2;
    InventoryItem item2;
    item2.resource = Resource_Grass;
    item2.amount = 5;
    items2.append(item2);
    buildCost2.itemsNeeded = items2;
    gBuildCost.append(buildCost2);
}

void Global::createTileTypesArray(){
    gTerrains={
        //Name                  //Variations    //Z-index
        { Terrain_Dirt,     1,              0 },
        { Terrain_Hole,     1,              0 },
        { Terrain_UnDug,    1,              0 },
        { Terrain_Tunnel,   1,              0 },
        { Terrain_Burrow,   1,              0 }
    };

    gOverlays={
        //Name                  //Variations    //Z-index
        { Overlay_Path,         1,              1 },
        { Overlay_Grass,        1,              3 },
        { Overlay_Bush,         2,              5 },
        { Overlay_RiceGrass,    1,              6 },
        { Overlay_Nest,         1,              6 },
        { Overlay_HoleLight,    1,              7 },
        { Overlay_Storage,      1,              10 },
        { Overlay_Sticks_Bundle01, 1,              14 }, // remove bundle from name
        { Overlay_Grass_Bundle01,  1,              15 },
        { Overlay_RiceGrass_Bundle04,       1,              16 },
        { Overlay_Corpse,       2,              19 },
        { Overlay_Cross,        2,              20 },
        { Overlay_Basket,       1,              21 }
    };

    TileTypeStruct tDirt, oBush, tHole, tUnDug, tTunnel, oTunnelLit, oGrass, oRiceGrass, tBurrow, oBurrowLit, oCorpse, oNest, oPath, oStorage, oRiceGrassBundle01, oRiceGrassBundle02, oRiceGrassBundle03, oCross, oBasket, oRiceGrassBundle04, oGrassBundle01, oSticksBundle01;

    tDirt.tileType = Dirt;
    tDirt.variations = 1;
    oBush.tileType = Bush;
    oBush.variations = 2;
    tHole.tileType = Hole;
    tHole.variations = 1;
    tUnDug.tileType = UnDug;
    tUnDug.variations = 1;
    tTunnel.tileType = Tunnel;
    tTunnel.variations = 1;
    oTunnelLit.tileType = HoleLight;
    oTunnelLit.variations = 1;
    oGrass.tileType = Grass;
    oGrass.variations = 1;
    oRiceGrass.tileType = RiceGrass;
    oRiceGrass.variations = 1;
    tBurrow.tileType = Burrow;
    tBurrow.variations = 1;
    oBurrowLit.tileType = HoleLight;
    oBurrowLit.variations = 1;
    oCorpse.tileType = Corpse;
    oCorpse.variations = 2;
    oNest.tileType = Nest;
    oNest.variations = 1;
    oPath.tileType = Path;
    oPath.variations = 1;
    oStorage.tileType = Storage;
    oStorage.variations = 1;
    oRiceGrassBundle01.tileType = RiceGrass_Bundle01;
    oRiceGrassBundle01.variations = 1;
    oRiceGrassBundle02.tileType = RiceGrass_Bundle02;
    oRiceGrassBundle02.variations = 1;
    oRiceGrassBundle03.tileType = RiceGrass_Bundle03;
    oRiceGrassBundle03.variations = 1;
    oCross.tileType = Cross;
    oCross.variations = 2;
    oCross.tileType = Basket;
    oCross.variations = 1;
    oRiceGrassBundle04.tileType = RiceGrass_Bundle04;
    oRiceGrassBundle04.variations = 1;
    oGrassBundle01.tileType = Grass_Bundle01;
    oGrassBundle01.variations = 1;
    oSticksBundle01.tileType = Sticks_Bundle01;
    oSticksBundle01.variations = 1;

    tDirt.zIndex = 0;
    tHole.zIndex = 0;
    tUnDug.zIndex = 0;
    tTunnel.zIndex = 0;
    tBurrow.zIndex = 0;
    oPath.zIndex = 1;
    oGrass.zIndex = 3;
    oBush.zIndex = 5;
    oRiceGrass.zIndex = 6;    
    oNest.zIndex = 6;
    oTunnelLit.zIndex = 7;
    oBurrowLit.zIndex = 7;
    oStorage.zIndex = 10;
    oRiceGrassBundle01.zIndex = 12;
    oRiceGrassBundle02.zIndex = 13;
    oRiceGrassBundle03.zIndex = 14;
    oSticksBundle01.zIndex = 14;
    oGrassBundle01.zIndex = 15;
    oRiceGrassBundle04.zIndex = 16;
    oCorpse.zIndex = 19;
    oCross.zIndex = 20;
    oBasket.zIndex = 21;



    gTileTypes.append(tDirt);
    gTileTypes.append(oBush);
    gTileTypes.append(tHole);
    gTileTypes.append(tUnDug);
    gTileTypes.append(tTunnel);
    gTileTypes.append(oTunnelLit);
    gTileTypes.append(oGrass);
    gTileTypes.append(oRiceGrass);
    gTileTypes.append(tBurrow);
    gTileTypes.append(oBurrowLit);
    gTileTypes.append(oCorpse);
    gTileTypes.append(oNest);
    gTileTypes.append(oPath);
    gTileTypes.append(oStorage);
    gTileTypes.append(oRiceGrassBundle01);
    gTileTypes.append(oRiceGrassBundle02);
    gTileTypes.append(oRiceGrassBundle03);
    gTileTypes.append(oCross);
    gTileTypes.append(oBasket);
    gTileTypes.append(oRiceGrassBundle04);
    gTileTypes.append(oGrassBundle01);
    gTileTypes.append(oSticksBundle01);
}

void Global::createResourcesArray(){
    ResourceStruct r;
    r.resource = Resource_Sticks;
    r.size = 4;
    gResources.append(r);

    r.resource = Resource_Grass;
    r.size = 4;
    gResources.append(r);

    r.resource = Resource_RiceGrass;
    r.size = 4; // Size per RiceGrass. So 25 RiceGrasses take up 100 units of space
    gResources.append(r);
}

int Global::getItemSize(InventoryItem item){
    int itemSize = 0;
    for (int i=0;i<gResources.length();i++){ // Check size for this resource
        if (item.resource == gResources[i].resource){
           itemSize = gResources[i].size * item.amount;
           return itemSize;
        }
    }

    return itemSize;
}

int Global::getResourceSize(Resource resource){
    int resourceSize = 0;
    for (int i=0;i<gResources.length();i++){ // Check size for this resource
        if (resource == gResources[i].resource){
           resourceSize = gResources[i].size;
           return resourceSize;
        }
    }

    return resourceSize;
}

int Global::getTileTypeIndex(TileType tileType){
    int index = 0;
    for (int i=0;i<gTileTypes.length();i++){
        if (gTileTypes.value(i).tileType == tileType){            
            index = i;
            break;
        }
    }
    return index;
}

int Global::getVariantsPerTileType(TileType tileType){
    int variation = 1;
    for (int i=0;i<gTileTypes.length();i++){
        if (gTileTypes.value(i).tileType == tileType){
            variation = gTileTypes.value(i).variations;
            break;
        }
    }
    return variation;
}

int Global::getZIndex(Terrain terrain){
    int zIndex = 0;
    for (int i=0;i<gTerrains.length();i++){
        if (gTerrains.value(i).terrain == terrain){
            zIndex = gTerrains.value(i).zIndex;
            break;
        }
    }
    return zIndex;
}

int Global::getZIndex(Overlay overlay){
    int zIndex = 0;
    for (int i=0;i<gOverlays.length();i++){
        if (gOverlays.value(i).overlay == overlay){
            zIndex = gOverlays.value(i).zIndex;
            break;
        }
    }
    return zIndex;
}

QString Global::getResourceName(Resource resource){
 QString s;

    switch(resource){
        case Resource_Sticks:
          s = "Sticks";
          break;
        case Resource_Grass:
          s = "Grass";
          break;
        case Resource_RiceGrass:
           s = "Rice Grass";
           break;
       default :
          s = "Unknown";
    }
    return s;
}

QString Global::getName(Terrain terrain){
 QString s;

    switch(terrain){
        case Terrain_None:
          s = "None";
          break;
        case Terrain_Dirt:
          s = "Dirt";
          break;
        case Terrain_Hole:
           s = "Entrance";
           break;
        case Terrain_UnDug:
           s = "Dirt";
           break;
        case Terrain_Tunnel:
           s = "Tunnel";
           break;
        case Terrain_Burrow:
           s = "Burrow";
           break;
       default :
          s = "Unknown";
    }

    return s;
}

QString Global::getName(Overlay overlay){
 QString s;

    switch(overlay){
        case Overlay_None:
          s = "None";
          break;
        case Overlay_Bush:
           s = "Bush";
           break;
        case Overlay_HoleLight:
           s = "Exit";
           break;
        case Overlay_Grass:
           s = "Grass";
           break;
        case Overlay_RiceGrass:
           s = "Delicious Rice Grass!";
           break;
        case Overlay_Storage:
           s = "Storage";
           break;
        case Overlay_Nest:
           s = "Nest";
           break;
       default :
          s = "Unknown";
    }

    return s;
}

QVector<InventoryItem> Global::getBuildCost(Overlay building){
    for (int i=0;i<gBuildCost.length();i++){
        if(building == gBuildCost.value(i).building){
            return gBuildCost.value(i).itemsNeeded;
        }
    }

    //If couldn't find, return nothing
    QVector<InventoryItem> items;
    InventoryItem item;
    item.resource = Resource_None;
    item.amount = 0;
    items.append(item);

    return items;
}

QString Global::getObituary(CauseOfDeath causeOfDeath){
    QString s;

       switch(causeOfDeath){
           case CauseOfDeath_OldAge:
             s = "'s spirit has reentered the Dreamtime";
             break;
           case CauseOfDeath_Starvation:
             s = "starved to death";
             break;
           case CauseOfDeath_Unknown:
              s = "died of unknown reasons";
              break;
          default :
             s = "";
       }

       return s;

}
