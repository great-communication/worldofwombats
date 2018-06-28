//Loads and crops the sources images
#ifndef IMAGELOADER_H
#define IMAGELOADER_H

#include <QObject>
#include "global.h"

class Global;

class ImageLoader : public QObject
{
    Q_OBJECT
public:
    explicit ImageLoader(QObject *parent = nullptr);
    Global *global;
    void setup(Global *global);   

    QVector<TerrainImagesStruct> loadTerrains();
    QVector<OverlayImagesStruct> loadOverlays();
    QVector<QImage> loadRiceGrass();
    QVector<QImage> loadGrass();
    QVector<QImage> loadPath();
    QVector<QImage> loadResourceIcons();

signals:

public slots:

private:

};

#endif // IMAGELOADER_H
