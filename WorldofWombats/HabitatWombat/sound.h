//Plays sound
#ifndef SOUND_H
#define SOUND_H

#include <QObject>
#include <QMediaPlayer>
#include <QMediaPlaylist>

class GUI;

class Sound : public QObject
{
    Q_OBJECT
public:
    explicit Sound(QObject *parent = nullptr);
    GUI *gui;
    void setup();
    void playSoundFX(int index);
    void playSoundFXBush();
    void playSoundFXDigging();
    void playSoundFXDigHoleComplete();
    void playSoundFXDigUndergroundComplete();
    void playSoundFXBurrowCreated();
    void playSoundFXClaimRiceGrass();
    void playSoundFXBuildPath();

signals:

public slots:


private:
    QMediaPlaylist *playlistsfx;
    QMediaPlayer *soundfx;
    bool digSoundFlip;
};

#endif // SOUND_H
