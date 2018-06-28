//Each level has a level file that stores seed number and position etc.
#ifndef LEVEL_H
#define LEVEL_H

#include <QObject>

class WorldBuilder;

class Level : public QObject
{
    Q_OBJECT
public:
    explicit Level(QObject *parent = nullptr);
    WorldBuilder *worldBuilder;
    void setup(WorldBuilder *worldBuilder, QString levelName);
    void create();
    void save();
    void load();
    QString getName();
    int getSeed();

signals:

public slots:

private:
    QString name;
    int seed;
};

#endif // LEVEL_H
