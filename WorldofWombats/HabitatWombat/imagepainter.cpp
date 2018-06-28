#include "imagepainter.h"
#include "imageloader.h"
#include "world.h"
#include "worldBuilder.h"
#include <QPainter>
#include <QImage>

ImagePainter::ImagePainter(QObject *parent) : QObject(parent){

}

void ImagePainter::setup(Global *global, World *world, WorldBuilder *worldBuilder, QSize backgroundSize){
    this->global = global;
    this->world = world;
    this->worldBuilder = worldBuilder;
    this->backgroundSize = backgroundSize;
    ImageLoader imageLoader;
    imageLoader.setup(this->global);
    terrainImages = imageLoader.loadTerrains();
    overlayImages = imageLoader.loadOverlays();
    RiceGrassImages = imageLoader.loadRiceGrass();
    GrassImages = imageLoader.loadGrass();
    PathImages = imageLoader.loadPath();
    resourceIcons = imageLoader.loadResourceIcons();
}

void ImagePainter::updateBackgroundSize(QSize backgroundSize, bool underground){
    QSize diff = this->backgroundSize-backgroundSize;
    if(diff.width()>=gTileSize*2 || diff.width()<=-gTileSize*2 || diff.height()>=gTileSize*2 || diff.height()<=-gTileSize*2){
        this->backgroundSize = backgroundSize;
        worldBuilder->prepareToUpdateTiles(worldBuilder->worldPosition, underground);
    }
}

QImage ImagePainter::prepareBackgroundImage(bool isUnderground){
    QPoint chunkSize = world->getChunkSize();
    QImage imageBackground = QImage(backgroundSize,QImage::Format_RGB32);

    QPainter painter;
    painter.begin(&imageBackground);

    int iStart = (chunkSize.x()-(backgroundSize.width()/gTileSize))/2;
    int iEnd = iStart + (backgroundSize.width()/gTileSize);
    int jStart = (chunkSize.y()-(backgroundSize.height()/gTileSize))/2;
    int jEnd = jStart + (backgroundSize.height()/gTileSize);

    //First paint the base image
    for (int i=iStart;i<iEnd;i++){
        for (int j=jStart;j<jEnd;j++){

            int tileType;
            int variation;
            QVector<int> miniTileNumbers;

            if (!isUnderground){
                tileType = worldBuilder->visibleTiles[i][j].terrainType;
                variation = worldBuilder->visibleTiles[i][j].variation;
                miniTileNumbers = worldBuilder->visibleTiles[i][j].miniTileNumbers;
            }else{
                tileType = worldBuilder->visibleTiles[i][j].terrainTypeUnderground;
                variation = 1;
                miniTileNumbers = worldBuilder->visibleTiles[i][j].miniTileNumbersUnderground;
            }

            //Paint minitiles one by one
            for (int m=0;m<4;m++){  // 4 MiniTiles per Tile
                QImage image = getMiniTileImage(tileType, variation, miniTileNumbers.value(m));
                int k = 0; int l = 0; if (m==0){k = 0; l = 0;}else if(m==1){k = 1; l = 0;}else if(m==2){k = 0; l = 1;}else if(m==3){k = 1; l = 1;} // Temp fix
                painter.drawImage(((i-iStart)*128)+(k*64),((j-jStart)*128)+(l*64),image);
            }
        }
    }

    //Then paint all overlays on top of the base image
    QImage overlay = QImage(QSize(gTileSize,gTileSize),QImage::Format_ARGB32);
    overlay.fill(qRgba(0, 0, 0, 0));

    for (int i=iStart;i<iEnd;i++){
        for (int j=jStart;j<jEnd;j++){
            QVector<int> overlayTerrainTypes;
            QVector<int> overlayVariations;
            QVector<QVector<int>> overlayMiniTileNumbers;

            if (!isUnderground){
                overlayTerrainTypes = worldBuilder->visibleTiles[i][j].overlayTerrainTypes;
                overlayVariations = worldBuilder->visibleTiles[i][j].overlayVariations;
                overlayMiniTileNumbers = worldBuilder->visibleTiles[i][j].overlayMiniTileNumbers;
            }else{
                overlayTerrainTypes = worldBuilder->visibleTiles[i][j].overlayTerrainTypesUnderground;
                overlayVariations = worldBuilder->visibleTiles[i][j].overlayVariationsUnderground;
                overlayMiniTileNumbers = worldBuilder->visibleTiles[i][j].overlayMiniTileNumbersUnderground;
            }

            //Create tile overlay
            int overlap = 0; //Temp variable, fix...

            for (int n=0;n<overlayTerrainTypes.length();n++){
                bool fuckyou = false;
                if (overlayTerrainTypes.value(n)==RiceGrass){
                    overlay = getRiceGrassImage(i, j, overlayTerrainTypes.value(n), overlayVariations.value(n), overlayMiniTileNumbers.value(n));
                    overlap = 64;
                    fuckyou=true;
                }else if(overlayTerrainTypes.value(n)==Grass){
                    overlay = getGrassImage(i, j, overlayTerrainTypes.value(n), overlayVariations.value(n), overlayMiniTileNumbers.value(n));
                    overlap = 64;
                    fuckyou=true;
                }else if(overlayTerrainTypes.value(n)==Basket){
                    QImage temp(":/HabitatWombat/Images/Icons/HabitatWombat/Images/Icons/basket.png");
                    painter.drawImage(((i-iStart)*128)+32,((j-jStart)*128)+32,temp);
                }else if(overlayTerrainTypes.value(n)==RiceGrass_Bundle04){
                    QImage temp = getRiceGrassBundle04(i,j,overlayTerrainTypes.value(n), overlayVariations.value(n), overlayMiniTileNumbers.value(n));
                    painter.drawImage(((i-iStart)*128),((j-jStart)*128),temp);
                }else if(overlayTerrainTypes.value(n)==Grass_Bundle01){
                    QImage temp = getGrassBundle01(i,j,overlayTerrainTypes.value(n), overlayVariations.value(n), overlayMiniTileNumbers.value(n));
                    painter.drawImage(((i-iStart)*128),((j-jStart)*128),temp);
                }else if(overlayTerrainTypes.value(n)==Sticks_Bundle01){
                    QImage temp = getSticksBundle01(i,j,overlayTerrainTypes.value(n), overlayVariations.value(n), overlayMiniTileNumbers.value(n));
                    painter.drawImage(((i-iStart)*128),((j-jStart)*128),temp);
                }else if(overlayTerrainTypes.value(n)==Path){
                    QImage temp = getPathImage(i,j,overlayTerrainTypes.value(n), overlayVariations.value(n), overlayMiniTileNumbers.value(n));
                    painter.drawImage((((i-iStart)*128)-64),(((j-jStart)*128))-64,temp);
                }else{
                    for (int m=0;m<4;m++){  // 4 MiniTiles per Tile
                        //if(overlayTerrainTypes.value(n)==Bush){
                        //    worker->consolePrint("bush minitile numbers: "+QString::number(overlayMiniTileNumbers.value(n).value(m)));
                       // }
                        QImage temp = getMiniTileImage(overlayTerrainTypes.value(n), overlayVariations.value(n), overlayMiniTileNumbers.value(n).value(m));
                        int k = 0; int l = 0; if (m==0){k = 0; l = 0;}else if(m==1){k = 1; l = 0;}else if(m==2){k = 0; l = 1;}else if(m==3){k = 1; l = 1;} // Temp fix
                        //QPainter painter2;
                        //painter2.begin(&overlay);
                        //painter2.drawImage(k*64,l*64,temp);
                        //painter2.end();
                        painter.drawImage(((i-iStart)*128)+(k*64),((j-jStart)*128)+(l*64),temp);
                    }

                    overlap = 0;
                }

                //Tile overlay created, now draw it into the background image
                if(fuckyou){
                    painter.drawImage(((i-iStart)*128)-overlap,((j-jStart)*128)-overlap,overlay); // Overlap -64 is offset from getRiceGrassImage
                }
            }
        }
    }
    painter.end();

    return imageBackground.mirrored().rgbSwapped();
}

QImage ImagePainter::mergeImages(QImage baseImage, QImage overlay){

    QImage mergedImage = QImage(baseImage.size(), QImage::Format_ARGB32_Premultiplied);
    QPainter painter(&mergedImage);

    painter.setCompositionMode(QPainter::CompositionMode_Source);
    painter.fillRect(mergedImage.rect(), Qt::transparent);

    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
    painter.drawImage(0, 0, baseImage);

    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
    painter.drawImage(0, 0, overlay);

    painter.end();

    return mergedImage;
}


QImage ImagePainter::getMiniTileImage(int tileType, int variation, int miniTileNumber){

    int tIndex = 0;
    bool isTerrain = false;
    bool isOverlay = false;

    for (int i=0;i<terrainImages.length();i++){
        if(terrainImages.value(i).terrain == tileType){
            tIndex = i;
            isTerrain = true;
        }
    }

    if(!isTerrain){
        for (int i=0;i<overlayImages.length();i++){
            if(overlayImages.value(i).overlay == tileType){
                tIndex = i;
                isOverlay = true;
            }
        }
    }

    if(tileType == Bush){ //Ugly fix...
        if(variation <23){
            variation = 1;
        }else{
            variation = 2;
        }
    }

    int vIndex = variation-1;
    int mIndex = miniTileNumber-1;   

    if(isTerrain){
        if (tIndex>=0 && vIndex>=0 && mIndex>=0){ //Checking that the indexes are valid
            if (tIndex<terrainImages.length()){
                if (vIndex<terrainImages[tIndex].images.length()){
                    if (mIndex<terrainImages[tIndex].images[vIndex].length()){
                        return terrainImages[tIndex].images[vIndex][mIndex];  //Early return
                    }
                }
            }
        }
    }else if(isOverlay){
        if (tIndex>=0 && vIndex>=0 && mIndex>=0){ //Checking that the indexes are valid
            if (tIndex<overlayImages.length()){
                if (vIndex<overlayImages[tIndex].images.length()){
                    if (mIndex<overlayImages[tIndex].images[vIndex].length()){
                        return overlayImages[tIndex].images[vIndex][mIndex];  //Early return
                    }
                }
            }
        }
    }

    QImage img = terrainImages[0].images[0][0];
    return img;  // Fallback return if indexes are not valid
}

QImage ImagePainter::getRiceGrassImage(int i, int j, int tileType, int variation, QVector<int> miniTileNumbers){
    QImage overlay = QImage(QSize(gTileSize+128,gTileSize+128),QImage::Format_ARGB32);
    overlay.fill(qRgba(0, 0, 0, 0));
    QPainter painter;
    painter.begin(&overlay);

    if (worldBuilder->visibleTiles[i][j].riceGrassImagePos.length()>0){
        //If already calculated, just pull from array
        for (int k=0;k<worldBuilder->visibleTiles[i][j].riceGrassImagePos.length();k++){
            int x = worldBuilder->visibleTiles[i][j].riceGrassImagePos[k].x();
            int y = worldBuilder->visibleTiles[i][j].riceGrassImagePos[k].y();
            int index = worldBuilder->visibleTiles[i][j].riceGrassImageIndex[k];
            painter.drawImage(x,y,RiceGrassImages[index]);
        }
    }else{
        //Otherwise calculate number of rice grass, image index and pos...
        //variation = quantity between 5-100;
        /*
         1-3 = 1 * 5 = 5
         4-9 = 2 * 5 = 10
         10-18 = 3 * 5 = 15
         19-21 = 5 * 5 = 25
        */
        QVector<QPoint> riceGrassImagePos;
        QVector<int> riceGrassImageIndex;
        variation = variation*2;
        QVector<QRect> rect;

        int quota = 0;

        while (quota<=variation){
            int spaceLeft = variation-quota;
            int min = 0;
            int max = 0;
            int index = 0;
            if(spaceLeft>=25){
                min = 0;
                max = 20;
                index = worldBuilder->getRandomNumber(min,max);

            }else if (spaceLeft>=15){
                min = 0;
                max = 17;
                index = worldBuilder->getRandomNumber(min,max);

            }else if (spaceLeft>=10){
                min = 0;
                max = 8;
                index = worldBuilder->getRandomNumber(min,max);
            }else if (spaceLeft>=5){
                min = 0;
                max = 2;
                index = worldBuilder->getRandomNumber(min,max);
            }else{
                break;
            }

            if(index>=0 && index<=2){
                quota += 5;
            }else if (index >=3 && index <=8){
                quota += 10;
            }else if (index >=9 && index <=17){
                quota += 15;
            }else if (index >=18 && index <=20){
                quota += 25;
            }

            int leftLimit = 48;
            int rightLimit = 144;
            int upLimit = 48;
            int downLimit = 144;

            if(miniTileNumbers.value(0) == 4 || miniTileNumbers.value(0) == 5){
                //Overlap up
                upLimit -= 48;
            }

            if(miniTileNumbers.value(2) == 4 || miniTileNumbers.value(2) == 5){
                //Overlap down
                downLimit += 48;
            }

            if(miniTileNumbers.value(0) == 2 || miniTileNumbers.value(0) == 5){
                //Overlap left
                leftLimit -= 48;
            }

            if(miniTileNumbers.value(1) == 2 || miniTileNumbers.value(1) == 5){
                //Overlap right
                rightLimit += 48;
            }

            bool blocked = true;
            int safety = 0;
            int x = 48;
            int y = 48;

            while(blocked && safety<100){
                x = worldBuilder->getRandomNumber(leftLimit, rightLimit);
                y = worldBuilder->getRandomNumber(upLimit, downLimit);

                QRect test = QRect(x,y,16,16);

                blocked = false;

                for (int i=0;i<rect.length();i++){
                    blocked = rect.value(i).intersects(test);
                    if(blocked){
                        break;
                    }
                }
                safety++;
            }

            if(!blocked){
                rect.append(QRect(x,y,16,16));
                QImage temp = RiceGrassImages[index];
                painter.drawImage(x,y,temp);
                riceGrassImagePos.append(QPoint(x,y));
                riceGrassImageIndex.append(index);
            }else{
                //worker->consolePrint("still blocked");
            }

        }
        worldBuilder->visibleTiles[i][j].riceGrassImagePos = riceGrassImagePos;
        worldBuilder->visibleTiles[i][j].riceGrassImageIndex = riceGrassImageIndex;
    }

    painter.end();

    return overlay;
}

QImage ImagePainter::getGrassImage(int i, int j, int tileType, int variation, QVector<int> miniTileNumbers){
    QImage overlay = QImage(QSize(gTileSize+128,gTileSize+128),QImage::Format_ARGB32);
    overlay.fill(qRgba(0, 0, 0, 0));
    QPainter painter;
    painter.begin(&overlay);

    if (worldBuilder->visibleTiles[i][j].grassImagePos.length()>0){
        //If already calculated, just pull from array
        for (int k=0;k<worldBuilder->visibleTiles[i][j].grassImagePos.length();k++){
            int x = worldBuilder->visibleTiles[i][j].grassImagePos[k].x();
            int y = worldBuilder->visibleTiles[i][j].grassImagePos[k].y();
            int index = worldBuilder->visibleTiles[i][j].grassImageIndex[k];
            painter.drawImage(x,y,GrassImages[index]);
        }
    }else{
        //Otherwise calculate number of rice grass, image index and pos...
        //variation = quantity between 5-100;
        /*
         1-3 = 1 * 5 = 5
         4-9 = 2 * 5 = 10
         10-18 = 3 * 5 = 15
         19-21 = 5 * 5 = 25
        */
        QVector<QPoint> riceGrassImagePos;
        QVector<int> riceGrassImageIndex;
        variation = variation*2;
        QVector<QRect> rect;

        int quota = 0;

        while (quota<=variation){
            int spaceLeft = variation-quota;
            int min = 0;
            int max = 0;
            int index = 0;
            if(spaceLeft>=25){
                min = 0;
                max = 20;
                index = worldBuilder->getRandomNumber(min,max);

            }else if (spaceLeft>=15){
                min = 0;
                max = 17;
                index = worldBuilder->getRandomNumber(min,max);

            }else if (spaceLeft>=10){
                min = 0;
                max = 8;
                index = worldBuilder->getRandomNumber(min,max);
            }else if (spaceLeft>=5){
                min = 0;
                max = 2;
                index = worldBuilder->getRandomNumber(min,max);
            }else{
                break;
            }

            if(index>=0 && index<=2){
                quota += 5;
            }else if (index >=3 && index <=8){
                quota += 10;
            }else if (index >=9 && index <=17){
                quota += 15;
            }else if (index >=18 && index <=20){
                quota += 25;
            }

            int leftLimit = 48;
            int rightLimit = 144;
            int upLimit = 48;
            int downLimit = 144;

            if(miniTileNumbers.value(0) == 4 || miniTileNumbers.value(0) == 5){
                //Overlap up
                upLimit -= 48;
            }

            if(miniTileNumbers.value(2) == 4 || miniTileNumbers.value(2) == 5){
                //Overlap down
                downLimit += 48;
            }

            if(miniTileNumbers.value(0) == 2 || miniTileNumbers.value(0) == 5){
                //Overlap left
                leftLimit -= 48;
            }

            if(miniTileNumbers.value(1) == 2 || miniTileNumbers.value(1) == 5){
                //Overlap right
                rightLimit += 48;
            }

            bool blocked = true;
            int safety = 0;
            int x = 48;
            int y = 48;

            while(blocked && safety<100){
                x = worldBuilder->getRandomNumber(leftLimit, rightLimit);
                y = worldBuilder->getRandomNumber(upLimit, downLimit);

                QRect test = QRect(x,y,16,16);

                blocked = false;

                for (int i=0;i<rect.length();i++){
                    blocked = rect.value(i).intersects(test);
                    if(blocked){
                        break;
                    }
                }
                safety++;
            }

            if(!blocked){
                rect.append(QRect(x,y,16,16));
                QImage temp = GrassImages[index];
                painter.drawImage(x,y,temp);
                riceGrassImagePos.append(QPoint(x,y));
                riceGrassImageIndex.append(index);
            }else{
                //worker->consolePrint("still blocked");
            }

        }
        worldBuilder->visibleTiles[i][j].grassImagePos = riceGrassImagePos;
        worldBuilder->visibleTiles[i][j].grassImageIndex = riceGrassImageIndex;
    }

    painter.end();

    return overlay;
}

QImage ImagePainter::getPathImage(int i, int j, int tileType, int variation, QVector<int> miniTileNumbers){
    QPoint pos = QPoint(miniTileNumbers.value(0),miniTileNumbers.value(1));

    QImage overlay = QImage(QSize(gTileSize+256,gTileSize+256),QImage::Format_ARGB32);
    overlay.fill(qRgba(0, 0, 0, 0));
    QPainter painter;
    painter.begin(&overlay);


    if (worldBuilder->visibleTiles[i][j].pathImageIndex.length()>0){
        //If already calculated, just pull from array
        for (int k=0;k<worldBuilder->visibleTiles[i][j].pathImageIndex.length();k++){
            int x = 0;
            int y = 0;
            int index = worldBuilder->visibleTiles[i][j].pathImageIndex[k];
            if(index <= 4){
                x = 64;
                y = 64;
            }
            painter.drawImage(x,y,PathImages[index]);
        }

    }else{
        //Otherwise calculate
        QVector<bool> tileHasPath;
        QVector<int> indexes;

        //Check all neighbouring tiles for paths
        for (int i=-1;i<2;i++){ //Loop through columns
            for (int j=-1;j<2;j++){ //Loop through rows
                if (!(i==0 && j==0)){ //ignore self (pos 0,0)
                    TileStruct &query = world->getTile(pos+QPoint(i,j));
                    bool sameOverlay = false;
                    for (int k=0;k<query.overlay.length();k++){
                        if (query.overlay.value(k)==Path){
                            sameOverlay = true;
                            break;
                        }
                    }

                    if (sameOverlay){
                        tileHasPath.append(true);
                        //worker->consolePrint("tile has path");
                    }else{
                        tileHasPath.append(false);
                    }
                }
            }
        }


        if(tileHasPath.value(0)==true){
            painter.drawImage(0,0,PathImages[5]); //Images are different sizes? sometimes 128x128, sometimes 256x256... is it a problem?
            indexes.append(5);
        }
        if (tileHasPath.value(1)==true){
            painter.drawImage(64,64,PathImages[3]);
            indexes.append(3);
        }
        if (tileHasPath.value(2)==true){
            painter.drawImage(0,0,PathImages[8]);
            indexes.append(8);
        }
        if (tileHasPath.value(3)==true){
            painter.drawImage(64,64,PathImages[0]);
            indexes.append(0);
        }
        if (tileHasPath.value(4)==true){
            painter.drawImage(64,64,PathImages[2]);
            indexes.append(2);
        }
        if (tileHasPath.value(5)==true){
            painter.drawImage(0,0,PathImages[7]);
            indexes.append(7);
        }
        if (tileHasPath.value(6)==true){
            painter.drawImage(64,64,PathImages[1]);
            indexes.append(1);
        }
        if (tileHasPath.value(7)==true){
            painter.drawImage(0,0,PathImages[6]);
            indexes.append(6);
        }

         painter.drawImage(64,64,PathImages[4]); //Draw path in middle
         indexes.append(4);
         worldBuilder->visibleTiles[i][j].pathImageIndex = indexes;
    }

    painter.end();
    return overlay;
}

QImage ImagePainter::getRiceGrassBundle04(int i, int j, int tileType, int variation, QVector<int> miniTileNumbers){
    QImage &riceGrassBundle04Image = resourceIcons[0];
    QImage overlay = QImage(QSize(gTileSize+128,gTileSize+128),QImage::Format_ARGB32);
    overlay.fill(qRgba(0, 0, 0, 0));
    QPainter painter;
    painter.begin(&overlay);

    for (int i=0;i<miniTileNumbers.length();i++){
        if(miniTileNumbers.value(i)==1){
            painter.drawImage(0,17,riceGrassBundle04Image);
        }else if (miniTileNumbers.value(i)==2){
            painter.drawImage(28,17,riceGrassBundle04Image);
        }else if (miniTileNumbers.value(i)==3){
            painter.drawImage(58,17,riceGrassBundle04Image);
        }else if (miniTileNumbers.value(i)==4){
            painter.drawImage(89,17,riceGrassBundle04Image);
        }else if (miniTileNumbers.value(i)==5){
            painter.drawImage(14,42,riceGrassBundle04Image);
        }else if (miniTileNumbers.value(i)==6){
            painter.drawImage(44,42,riceGrassBundle04Image);
        }else if (miniTileNumbers.value(i)==7){
            painter.drawImage(73,42,riceGrassBundle04Image);
        }
    }




    painter.end();

    return overlay;
}

QImage ImagePainter::getGrassBundle01(int i, int j, int tileType, int variation, QVector<int> miniTileNumbers){
    QImage &grassBundle01Image = resourceIcons[1];
    QImage overlay = QImage(QSize(gTileSize+128,gTileSize+128),QImage::Format_ARGB32);
    overlay.fill(qRgba(0, 0, 0, 0));
    QPainter painter;
    painter.begin(&overlay);

    for (int i=0;i<miniTileNumbers.length();i++){
        if(miniTileNumbers.value(i)==1){
            painter.drawImage(0,17,grassBundle01Image);
        }else if (miniTileNumbers.value(i)==2){
            painter.drawImage(28,17,grassBundle01Image);
        }else if (miniTileNumbers.value(i)==3){
            painter.drawImage(58,17,grassBundle01Image);
        }else if (miniTileNumbers.value(i)==4){
            painter.drawImage(89,17,grassBundle01Image);
        }else if (miniTileNumbers.value(i)==5){
            painter.drawImage(14,42,grassBundle01Image);
        }else if (miniTileNumbers.value(i)==6){
            painter.drawImage(44,42,grassBundle01Image);
        }else if (miniTileNumbers.value(i)==7){
            painter.drawImage(73,42,grassBundle01Image);
        }
    }




    painter.end();

    return overlay;
}

QImage ImagePainter::getSticksBundle01(int i, int j, int tileType, int variation, QVector<int> miniTileNumbers){
    QImage &sticksBundle01Image = resourceIcons[2];
    QImage overlay = QImage(QSize(gTileSize+128,gTileSize+128),QImage::Format_ARGB32);
    overlay.fill(qRgba(0, 0, 0, 0));
    QPainter painter;
    painter.begin(&overlay);

    for (int i=0;i<miniTileNumbers.length();i++){
        if(miniTileNumbers.value(i)==1){
            painter.drawImage(0,17,sticksBundle01Image);
        }else if (miniTileNumbers.value(i)==2){
            painter.drawImage(28,17,sticksBundle01Image);
        }else if (miniTileNumbers.value(i)==3){
            painter.drawImage(58,17,sticksBundle01Image);
        }else if (miniTileNumbers.value(i)==4){
            painter.drawImage(89,17,sticksBundle01Image);
        }else if (miniTileNumbers.value(i)==5){
            painter.drawImage(14,42,sticksBundle01Image);
        }else if (miniTileNumbers.value(i)==6){
            painter.drawImage(44,42,sticksBundle01Image);
        }else if (miniTileNumbers.value(i)==7){
            painter.drawImage(73,42,sticksBundle01Image);
        }
    }




    painter.end();

    return overlay;
}

QImage ImagePainter::mergeImageCorpse(int i, QImage baseImage, QImage overlay, qreal rotation, int variation){ //Is this used?
    //-1 because the arrays start at 0
    QImage overlayImage = overlay;

    //Temp fix, tried to change the rotation point but didn't work...
    //so moving the images around based on rotation instead, ugly but works
    int index;
    if (rotation==90){
        if (i==0){
            index = 7;
        }else if(i==1){
            index = 1;
        }else if(i==2){
            index = 9;
        }else if(i==3){
            index = 3;
        }
    }else if (rotation==180){
        if (i==0){
            index = 9;
        }else if(i==1){
            index = 7;
        }else if(i==2){
            index = 3;
        }else if(i==3){
            index = 1;
        }
    }else if (rotation==270){
        if (i==0){
            index = 3;
        }else if(i==1){
            index = 9;
        }else if(i==2){
            index = 1;
        }else if(i==3){
            index = 7;
        }
    }else{
        if (i==0){
            index = 1;
        }else if(i==1){
            index = 3;
        }else if(i==2){
            index = 7;
        }else if(i==3){
            index = 9;
        }
        rotation = 0;
    }

    overlayImage = getMiniTileImage(Corpse,variation,index);

    //Rotate image so the corpse aligns with it's owner
    QMatrix rm;
    rm.rotate(rotation);
    overlayImage = overlayImage.transformed(rm);
    //

    QImage imageWithOverlay = QImage(baseImage.size(), QImage::Format_ARGB32_Premultiplied);
    QPainter painter(&imageWithOverlay);

    painter.setCompositionMode(QPainter::CompositionMode_Source);
    painter.fillRect(imageWithOverlay.rect(), Qt::transparent);

    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
    painter.drawImage(0, 0, baseImage);

    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
    painter.drawImage(0, 0, overlayImage);

    painter.end();

    return imageWithOverlay;
}

QSize ImagePainter::getBackgroundSize(){
    return backgroundSize;
}
