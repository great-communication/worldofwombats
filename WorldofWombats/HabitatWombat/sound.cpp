#include "sound.h"
#include "gui.h"

Sound::Sound(QObject *parent) : QObject(parent){

}

void Sound::setup(){
    playlistsfx = new QMediaPlaylist(this);
    playlistsfx->addMedia(QUrl("qrc:/HabitatWombat/Sound/HabitatWombat/Sounds/fst_grass_sneak_01.wav"));
    playlistsfx->addMedia(QUrl("qrc:/HabitatWombat/Sound/HabitatWombat/Sounds/fst_grass_sneak_02.wav"));
    playlistsfx->addMedia(QUrl("qrc:/HabitatWombat/Sound/HabitatWombat/Sounds/fst_grass_sneak_03.wav"));
    playlistsfx->addMedia(QUrl("qrc:/HabitatWombat/Sound/HabitatWombat/Sounds/fst_grass_sneak_04.wav"));
    playlistsfx->addMedia(QUrl("qrc:/HabitatWombat/Sound/HabitatWombat/Sounds/fst_grass_sneak_05.wav"));
    playlistsfx->addMedia(QUrl("qrc:/HabitatWombat/Sound/HabitatWombat/Sounds/fst_grass_sneak_06.wav"));
    playlistsfx->addMedia(QUrl("qrc:/HabitatWombat/Sound/HabitatWombat/Sounds/fst_npc_dirt_walk_03.wav"));
    playlistsfx->addMedia(QUrl("qrc:/HabitatWombat/Sound/HabitatWombat/Sounds/fst_npc_dirt_walk_04.wav"));
    playlistsfx->addMedia(QUrl("qrc:/HabitatWombat/Sound/HabitatWombat/Sounds/wpn_impact_crgiantclub_dirt_01.wav"));
    playlistsfx->addMedia(QUrl("qrc:/HabitatWombat/Sound/HabitatWombat/Sounds/wpn_impact_crgiantclub_dirt_02.wav"));
    playlistsfx->addMedia(QUrl("qrc:/HabitatWombat/Sound/HabitatWombat/Sounds/wpn_impact_crgiantclub_dirt_03.wav")); //arrayindex 10
    playlistsfx->addMedia(QUrl("qrc:/HabitatWombat/Sound/HabitatWombat/Sounds/ui_levelup.wav"));
    playlistsfx->addMedia(QUrl("qrc:/HabitatWombat/Sound/HabitatWombat/Sounds/ui_skills_stop_01.wav"));
    playlistsfx->addMedia(QUrl("qrc:/HabitatWombat/Sound/HabitatWombat/Sounds/ui_skills_stop_02.wav"));
    playlistsfx->addMedia(QUrl("qrc:/HabitatWombat/Sound/HabitatWombat/Sounds/ui_skills_stop_03.wav")); //arrayindex 14
    playlistsfx->addMedia(QUrl("qrc:/HabitatWombat/Sound/HabitatWombat/Sounds/ui_quest_update.wav"));
    playlistsfx->addMedia(QUrl("qrc:/HabitatWombat/Sound/HabitatWombat/Sounds/npc_human_eat_01.wav"));
    playlistsfx->addMedia(QUrl("qrc:/HabitatWombat/Sound/HabitatWombat/Sounds/npc_human_eat_02.wav"));
    playlistsfx->addMedia(QUrl("qrc:/HabitatWombat/Sound/HabitatWombat/Sounds/npc_human_eat_03.wav")); // arrayindex 18
    playlistsfx->addMedia(QUrl("qrc:/Sounds/Sounds/mag_vampire_sunlight_02.wav"));  // Fix path and put it in habitat wombat
    playlistsfx->addMedia(QUrl("qrc:/HabitatWombat/Sound/HabitatWombat/Sounds/npc_goat_death_02vollowered.wav"));
    playlistsfx->addMedia(QUrl("qrc:/HabitatWombat/Sound/HabitatWombat/Sounds/ns2_gorge_taunt.mp3")); // arrayindex 21
    playlistsfx->addMedia(QUrl("qrc:/HabitatWombat/Sound/HabitatWombat/Sounds/phy_generic_dirt_medium_h_01.wav"));
    playlistsfx->addMedia(QUrl("qrc:/HabitatWombat/Sound/HabitatWombat/Sounds/phy_generic_dirt_medium_h_03.wav"));
    playlistsfx->addMedia(QUrl("qrc:/HabitatWombat/Sound/HabitatWombat/Sounds/phy_generic_dirt_medium_l_01.wav"));
    playlistsfx->addMedia(QUrl("qrc:/HabitatWombat/Sound/HabitatWombat/Sounds/phy_generic_dirt_medium_l_02.wav"));

    playlistsfx->setPlaybackMode(QMediaPlaylist::CurrentItemOnce);

    soundfx = new QMediaPlayer(this);
    soundfx->setPlaylist(playlistsfx);
    soundfx->setVolume(50);
    digSoundFlip=false;

}

void Sound::playSoundFX(int index){
    if(index == Sound_ClaimRiceGrass){
        playSoundFXClaimRiceGrass();
    }else{
        playlistsfx->setCurrentIndex(index);
        soundfx->play();
    }
}

void Sound::playSoundFXBush(){
    int temp = gui->getRandomNumber(0,5);
    playSoundFX(temp);
}

void Sound::playSoundFXDigging(){
    if (digSoundFlip){
        playSoundFX(6);
        digSoundFlip=false;
    }else{
        playSoundFX(7);
        digSoundFlip=true;
    }
}

void Sound::playSoundFXDigHoleComplete(){
    int temp = gui->getRandomNumber(8,10);
    playSoundFX(temp);
}

void Sound::playSoundFXDigUndergroundComplete(){
    int temp = gui->getRandomNumber(12,14);
    playSoundFX(temp);
}

void Sound::playSoundFXBurrowCreated(){
    playSoundFX(Sound_BurrowCreated);
}

void Sound::playSoundFXClaimRiceGrass(){
    int temp = gui->getRandomNumber(16,18);
    playlistsfx->setCurrentIndex(temp);
    soundfx->play();
}

void Sound::playSoundFXBuildPath(){
    int temp = gui->getRandomNumber(22,25);
    playlistsfx->setCurrentIndex(temp);
    soundfx->play();
}
