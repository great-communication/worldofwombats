//Game Front End / GUI
#ifndef GUI_H
#define GUI_H

#include <QWidget>
#include <QLabel>
#include "global.h"
#include <QTextEdit>
#include <QLineEdit>
#include <QTime>
#include <QTimer>


class OpenGL;
class Controller;
class Sound;
class Global;
class Console;
class InGameMenu;
class InfoBox;
class BurrowList;
class DeathNotice;
class CreateBurrow;
class BurrowMenu;
class StorageWidget;
class WInventoryWidget;
class ProgressWidget;

namespace Ui {
class GUI;
}

class GUI : public QWidget
{
    Q_OBJECT

public:
    explicit GUI(QWidget *parent = 0);
    ~GUI();
    bool underground;
    bool eventFilter(QObject *obj, QEvent *event);
    void setup(QString levelName);    
    Global *guiGlobal;
    OpenGL *openGL;
    Sound *sound;
    Console *console;
    Controller *controller;
    CreateBurrow *createBurrowDialog;
    BurrowList *burrowList;
    ProgressWidget *progressWidget;
    QPoint viewPosition;   
    QPoint positionsChanged;
    int xPositionsChanged, yPositionsChanged;    
    bool undergroundChanged;
    bool lockInput;
    bool lockMovement;    
    qreal rotation;
    int mainWindowState; //Retarded    
    bool layersFlipped;   
    int xMoved,yMoved;
    int xMoved2,yMoved2;
    int moveSpeed;
    bool moving, scrolling;
    QLabel *enableEvents;    
    bool building;
    bool burrowMenuOpen, storageWidgetOpen, wInventoryWidgetOpen;
    StorageWidget *storageWidget;
    QLabel *storageWidgetBackground;
    WInventoryWidget *wInventoryWidget;
    QLabel *wInventoryWidgetBackground;
    QSize windowSize;
    bool updatingTiles;
    QTimer *timerSoundFXCoolDown;
    bool soundFXCoolDown;
    int getRandomNumber(const int min, const int max);
    void setLocationText();
    void setLocationText(QString name);    
    void possess(int wombatId);
    void setScrollTarget(int wombatId);
    void setMoveMaxMin();    
    void on_quit_clicked();    
    void startUpdate();

    void setUnderground(bool b);
    QPoint getMainWindowOffset();
    void move();
    void setLockMovement(bool b);
    void setLockInput(bool b);
    void displayWombatInfo(QVector<QString> strings, int wombatId, bool setPossessButtonVisible);
    void displayTileInfo(QVector<QString> strings);
    void setViewPosition(QPoint pos);
    void setXYMoved(QPoint moved);
    void setUpdatingTiles(bool b);
    void showDeathNotice(QString wombatName, int wombatAge, int wombatCount, bool gameOver, CauseOfDeath causeOfDeath);
    void displayOutOfRangeMessage(QPoint pos);
    void showStorageWidget(int burrowId, int storageId, QVector<bool> acceptedResources);
    void progressWidgetDone();
    void stopCollectResource();
    void resizeWindow(QSize mainWindowSize);

    void showMenu();
    void hideMenu();

    //Widgets
    void on_showBurrowList_clicked();
    void closeBurrowList();
    void showBurrowMenu();
    void closeDeathNotice();
    void closeCreateBurrowDialog();
    void closeStorageWidget();
    void closeInfoBox();
    void closeWInventoryWidget();
    void closeProgressWidget();
    //

    //Called from controller
    void updateNeighboursArray(const QVector<MoveStruct> &neighbours);    



signals:
    void s_backToHWMenu();
    void s_getMainWindowState(); //Retarded

public slots:    
    void scrollToTargetPos();    
    void updateOpenGLWidget();    

private slots:
    void clearSoundFXCoolDown();
    void digHole();
    void digUnderground();
    void checkIfWombatMovementIsDone();

private:
    Ui::GUI *ui;
    friend class Console;

    //Widgets
    InGameMenu *inGameMenu;
    InfoBox *infoBox;    
    DeathNotice *deathNotice;
    BurrowMenu *burrowMenu;
    bool infoBoxOpen, burrowListOpen, deathNoticeOpen, createBurrowDialogOpen, positionInfoBoxOpen, progressWidgetOpen;
    QPoint infoBoxOffset, burrowListOffset, deathNoticeOffset, createBurrowDialogOffset, storageWidgetOffset, wInventoryWidgetOffset;
    //
    QTextEdit *positionInfoBox;
    QLabel *menuDetect;
    QLabel *locationLabel;    
    bool isGod;
    QTimer *timerUpdateScreen,*timerDiggingHole,*timerDiggingUnderground,*timerCheckIfWombatMovementIsDone;
    bool keyWActive,keyAActive,keySActive,keyDActive,keyShiftActive,keySpaceActive,keyBActive,keyCActive,keyTauntActive, keyCollectActive;
    int moveMax,moveMin;
    int iIndex,jIndex;
    int updateDistance;    
    bool diggingHole;
    bool diggingUnderground;
    int xDigDirection;
    int yDigDirection;    
    QPoint cursorPos;    
    bool movingWindow;
    int digCount;
    int numberOfWombatsOnDisplay;
    //QString deadWombatName;
    //int deadWombatAge;
    QPoint scrollTarget;
    QPoint scrollPos;
    bool openGLNotInitilized;    
    int showOwnerBurrowId;
    //QPointF targetInternalPos;
    QVector<MoveStruct> neighbours;
    QPoint screenCenter;
    enum DigNeighbourCheckResult {
        DigNeighbourCheckResult_NothingFound = 1,
        DigNeighbourCheckResult_AddTunnelToBurrow = 2,
        DigNeighbourCheckResult_MergeBurrows = 3,
    };
    struct DigNeighbourCheckResultStruct {
        DigNeighbourCheckResult result;
        int burrowId;
    };
    struct checkIfResourceReturnStruct {
        QPoint pos;
        TileType tileType;
    };
    int wombatIdScrollTarget;   
    QPoint getWorldPositionAtClick();
    //QPoint getVisibleTileIndexAtClick();
    void resizeEvent(QResizeEvent* event);
    QSize calculateBackgroundSize();    
    void updatePositionInfoBox();
    void startUpdateTeleport(QPoint target);
    void changeTerrain(QPoint pos, Terrain terrain, int burrowId, bool update);
    void addOverlay(QPoint pos, Overlay overlay, int variation, bool update);
    void rotateWombat();
    void updateNeighbour(QPoint pos, Terrain terrain, bool underground);
    Terrain geTerrainNeighboursArray(QPoint pos, bool underground);
    int getBurrowIdNeighboursArray(QPoint pos);
    bool hasOverlayNeighboursArray(QPoint pos, TileType tileType, bool underground);
    bool isBlocked(int x, int y);
    DigNeighbourCheckResultStruct checkIfNeighbouringTunnels(QPoint pos);
    void startDiggingHole();
    void stopDiggingHole();
    void startDiggingUnderground();
    void stopDiggingUnderground();    
    void claimResource();
    void collectResource();    
    void centerWidget(QWidget *widget);
    void moveWidget(QWidget *widget, QPoint offset);
    void stopMove();    
    void screenClicked();
    void screenDoubleClicked();
    void buildBulding();
    int checkIfWombatClicked();
    void updateInfoBox(QString string1, QString string2, QString string3, QString string4, QString string5);
    void updateInfoBoxPossessButton(bool b);
    void taunt();    
    checkIfResourceReturnStruct checkIfResource();
    void digPath();
    //Widgets
    void showInfoBox();
    void showCreateBurrowDialog();
    void closeBurrowMenu();    
    void showPositionInfoBox();
    void closePositionInfoBox();
    void showWInventoryWidget(int wombatId);
    void setSelectedWombatInRegistryUnderground(bool b);
    void showProgressWidget(int time);
    //
    QTime honestlyGOFUCKINGDIE;
    int gofuckyourselfpleasedeargod;


};

#endif // GUI_H
