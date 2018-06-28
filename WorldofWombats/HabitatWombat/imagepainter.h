//Creates the backgroundImage that is sent to openGL in the frontend
#ifndef IMAGEPAINTER_H
#define IMAGEPAINTER_H

#include <QObject>
#include "global.h"
#include <QSize>

class Global;
class World;
class WorldBuilder;

class ImagePainter : public QObject
{
    Q_OBJECT
public:
    explicit ImagePainter(QObject *parent = nullptr);
    Global *global;
    World *world;
    WorldBuilder *worldBuilder;
    void setup(Global *global, World *world, WorldBuilder *worldBuilder, QSize backgroundSize);
    void updateBackgroundSize(QSize backgroundSize, bool isUnderground);
    QImage prepareBackgroundImage(bool isUnderground);
    QSize getBackgroundSize();

signals:

public slots:

private:
    QSize backgroundSize;
    QVector<TerrainImagesStruct> terrainImages;
    QVector<OverlayImagesStruct> overlayImages;
    QVector<QImage> RiceGrassImages;
    QVector<QImage> GrassImages;
    QVector<QImage> PathImages;
    QVector<QImage> resourceIcons;
    QImage mergeImages(QImage baseImage, QImage overlay);
    QImage getMiniTileImage(int tileType, int variation, int miniTileNumber);
    QImage getRiceGrassImage(int i, int j, int tileType, int variation, QVector<int> miniTileNumbers);
    QImage getGrassImage(int i, int j, int tileType, int variation, QVector<int> miniTileNumbers);
    QImage getPathImage(int i, int j, int tileType, int variation, QVector<int> miniTileNumbers);
    QImage mergeImageCorpse(int i, QImage basePixmap, QImage overlay, qreal rotation, int variation);
    QImage getRiceGrassBundle04(int i, int j, int tileType, int variation, QVector<int> miniTileNumbers);
    QImage getGrassBundle01(int i, int j, int tileType, int variation, QVector<int> miniTileNumbers);
    QImage getSticksBundle01(int i, int j, int tileType, int variation, QVector<int> miniTileNumbers);
};

#endif // IMAGEPAINTER_H
