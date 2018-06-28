#include "imageloader.h"
#include <QImage>

ImageLoader::ImageLoader(QObject *parent) : QObject(parent){

}

void ImageLoader::setup(Global *global){
    this->global = global;
}

QVector<TerrainImagesStruct> ImageLoader::loadTerrains(){
    //For all terrains, and all variations, crop out the minitiles
    QVector<TerrainImagesStruct> terrainImages;
    for (int i=0;i<global->gTerrains.length();i++){
            QVector<QVector<QImage>> croppedVariants;
            for (int j=0;j<global->gTerrains.value(i).variations;j++){
                QImage tileset(":/HabitatWombat/Images/Tiles/HabitatWombat/Images/Tiles/"+QString::number(global->gTerrains.value(i).terrain)+"_"+QString::number(j+1)+".png");

                //Temp to show grid
                //if (i==0) {
                //    QImage fu(":/HabitatWombat/Images/Tiles/HabitatWombat/Images/Tiles/1_1 grid.png");
                //    tileset = fu;
                //}

                QVector<QImage> croppedMiniTiles;

                for (int i=0;i<3;i++){
                  for (int j=0;j<3;j++){
                      QRect rect(j*64, i*64, 64, 64);
                      croppedMiniTiles.append(tileset.copy(rect));
                  }
                }
                for (int i=0;i<3;i=i+2){
                  for (int j=4;j<7;j=j+2){
                      QRect rect(j*64, i*64, 64, 64);
                      croppedMiniTiles.append(tileset.copy(rect));
                  }
                }
                croppedVariants.append(croppedMiniTiles);
            }

            TerrainImagesStruct terrainImage;
            terrainImage.images = croppedVariants;
            terrainImage.terrain = global->gTerrains.value(i).terrain;
            terrainImages.append(terrainImage);
    }

    return terrainImages;
}

QVector<OverlayImagesStruct> ImageLoader::loadOverlays(){
    //For all overlays, and all variations, crop out the minitiles
    QVector<OverlayImagesStruct> overlayImages;
    for (int i=0;i<global->gOverlays.length();i++){
        if(global->gOverlays.value(i).overlay == Overlay_RiceGrass){
            //Own loading
        }else if(global->gOverlays.value(i).overlay == Overlay_Grass){
            //Own loading
        }else if(global->gOverlays.value(i).overlay == Overlay_Basket){
            //Own loading
        }else if(global->gOverlays.value(i).overlay == Overlay_RiceGrass_Bundle04){
            //Own loading
        }else if(global->gOverlays.value(i).overlay == Overlay_Grass_Bundle01){
            //Own loading
        }else if(global->gOverlays.value(i).overlay == Overlay_Sticks_Bundle01){
            //Own loading
        }else if(global->gOverlays.value(i).overlay == Overlay_Path){
            //Own loading
        }else{
            QVector<QVector<QImage>> croppedVariants;
            for (int j=0;j<global->gOverlays.value(i).variations;j++){
                QImage tileset(":/HabitatWombat/Images/Tiles/HabitatWombat/Images/Tiles/"+QString::number(global->gOverlays.value(i).overlay)+"_"+QString::number(j+1)+".png");

                QVector<QImage> croppedMiniTiles;
                //Crop out all minitiles from the tileset
                for (int i=0;i<3;i++){
                  for (int j=0;j<3;j++){
                      QRect rect(j*64, i*64, 64, 64);
                      croppedMiniTiles.append(tileset.copy(rect));
                  }
                }
                for (int i=0;i<3;i=i+2){
                  for (int j=4;j<7;j=j+2){
                      QRect rect(j*64, i*64, 64, 64);
                      croppedMiniTiles.append(tileset.copy(rect));
                  }
                }
                croppedVariants.append(croppedMiniTiles);
            }

            OverlayImagesStruct overlayImage;
            overlayImage.images = croppedVariants;
            overlayImage.overlay = global->gOverlays.value(i).overlay;
            overlayImages.append(overlayImage);
        }
    }

    return overlayImages;
}

QVector<QImage> ImageLoader::loadRiceGrass(){
    QVector<QImage> croppedRiceGrass;
    QImage tileset(":/HabitatWombat/Images/Tiles/HabitatWombat/Images/Tiles/"+QString::number(RiceGrass)+".png");
    for (int i=0;i<7;i++){
        for (int j=0;j<3;j++){
            QRect rect(i*64, j*64, 64, 64);
            croppedRiceGrass.append(tileset.copy(rect));
        }
    }
    return croppedRiceGrass;
}

QVector<QImage> ImageLoader::loadGrass(){
    QVector<QImage> croppedGrass;
    QImage tileset(":/HabitatWombat/Images/Tiles/HabitatWombat/Images/Tiles/"+QString::number(Grass)+".png");
    for (int i=0;i<7;i++){
        for (int j=0;j<3;j++){
            QRect rect(i*64, j*64, 64, 64);
            croppedGrass.append(tileset.copy(rect));
        }
    }
    return croppedGrass;
}

QVector<QImage> ImageLoader::loadPath(){
    QVector<QImage> croppedPaths;
    QImage tileset(":/HabitatWombat/Images/Tiles/HabitatWombat/Images/Tiles/paths_1.png");

    for (int i=0;i<5;i++){
        QRect rect(i*128, 0, 128, 128);
        croppedPaths.append(tileset.copy(rect));
    }

    for (int i=0;i<2;i++){
        for (int j=0;j<2;j++){
            QRect rect(i*256, 128+(j*256), 256, 256);
            croppedPaths.append(tileset.copy(rect));
        }
    }
    return croppedPaths;
}



QVector<QImage> ImageLoader::loadResourceIcons(){
    QVector<QImage> resourceIcons;
    QImage riceGrassBundle04Image(":/HabitatWombat/Images/Icons/HabitatWombat/Images/Icons/resource_ricegrass.png");
    resourceIcons.append(riceGrassBundle04Image);
    QImage grassBundle01Image(":/HabitatWombat/Images/Icons/HabitatWombat/Images/Icons/resource_grass.png");
    resourceIcons.append(grassBundle01Image);
    QImage sticksBundle01Image(":/HabitatWombat/Images/Icons/HabitatWombat/Images/Icons/resource_sticks.png");
    resourceIcons.append(sticksBundle01Image);
    return resourceIcons;
}









