#include "gui.h"
#include "ui_GUI.h"
#include "opengl.h"
#include "controller.h"
#include "sound.h"
#include "Widgets/Game/console.h"
#include "Widgets/Game/ingamemenu.h"
#include "Widgets/Game/infobox.h"
#include "Widgets/Game/burrowlist.h"
#include "Widgets/Game/deathnotice.h"
#include "Widgets/Game/createburrow.h"
#include "Widgets/Game/burrowmenu.h"
#include "Widgets/Game/storagewidget.h"
#include "Widgets/Game/winventorywidget.h"
#include "Widgets/Game/progresswidget.h"
#include <QLabel>
#include <QtMath>
#include <QPushButton>
#include <QGridLayout>
#include <QKeyEvent>

GUI::GUI(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GUI)
{
    ui->setupUi(this);     
}

GUI::~GUI(){
    delete ui;
}

int GUI::getRandomNumber(const int min, const int max){
    return ((qrand() % ((max + 1) - min)) + min);
}

void GUI::setup(QString levelName){
    qRegisterMetaType<QVector<MoveStruct>>();
    qRegisterMetaType<TileType>();
    qRegisterMetaType<Terrain>();
    qRegisterMetaType<QVector<QString>>();
    qRegisterMetaType<QVector<int>>();
    qRegisterMetaType<SoundFX>();
    qRegisterMetaType<QVector<InventoryItem>>();
    qRegisterMetaType<InventoryItem>();
    qRegisterMetaType<PathPoint>();
    qRegisterMetaType<WombatAITask>();
    qRegisterMetaType<WombatEmoticon>();
    qRegisterMetaType<CauseOfDeath>();
    qRegisterMetaType<QVector<QPoint>>();
    qRegisterMetaType<QVector<bool>>();

    installEventFilter(this);

    qsrand(static_cast<uint>(QTime::currentTime().msec()));
    getRandomNumber(1, 100);

    guiGlobal = new Global(this);
    guiGlobal->setup();

    windowSize = QSize(this->size());
    mainWindowState = -1;
    moveSpeed = 4;    
    updateDistance = 128;   //Having moved more than this distance will trigger an update, i.e. more tiles are loaded and the view is reset

    //Menu
    inGameMenu = new InGameMenu(this);
    inGameMenu->hwGameUI = this;
    inGameMenu->installEventFilter(this);    
    inGameMenu->move((windowSize.width()/2)-105,windowSize.height()-60);
    menuDetect = new QLabel (this);
    menuDetect->installEventFilter(this);
    menuDetect->setGeometry(inGameMenu->geometry());
    inGameMenu->hide();
    //

    screenCenter = QPoint((windowSize.width()/2)-gTileSize, (windowSize.height()/2)-gTileSize);

    locationLabel = new QLabel(this);
    locationLabel->setStyleSheet("QLabel { font-family:'MoolBoran'; font-size: 36pt; color: white;}");
    locationLabel->setGeometry(20,10,windowSize.width(),61);
    locationLabel->setVisible(true);

    console = new Console(this);
    console->gui = this;
    // add again hwGameObject->console = console;
    console->setVisible(false);

    enableEvents = new QLabel(this);
    enableEvents->setGeometry(0,0,windowSize.width(),windowSize.height());
    enableEvents->installEventFilter(this);
    positionInfoBoxOpen = false;
    createBurrowDialogOpen = burrowListOpen = infoBoxOpen = deathNoticeOpen = burrowMenuOpen = storageWidgetOpen = wInventoryWidgetOpen = progressWidgetOpen = false;
    isGod = false;
    keyWActive =  keyAActive = keySActive = keyDActive = keyShiftActive = keySpaceActive = keyBActive = keyCActive = keyTauntActive = keyCollectActive = false;
    timerUpdateScreen = new QTimer(this);
    timerUpdateScreen->setTimerType(Qt::PreciseTimer);
    timerDiggingHole = new QTimer(this);
    timerDiggingUnderground = new QTimer(this);  
    connect(timerDiggingHole, SIGNAL(timeout()), this, SLOT(digHole()));
    connect(timerDiggingUnderground, SIGNAL(timeout()), this, SLOT(digUnderground()));    
    digCount = 0;
    timerSoundFXCoolDown = new QTimer(this);
    connect(timerSoundFXCoolDown, SIGNAL(timeout()), this, SLOT(clearSoundFXCoolDown()));
    timerCheckIfWombatMovementIsDone = new QTimer(this);
    connect(timerCheckIfWombatMovementIsDone, SIGNAL(timeout()), this, SLOT(checkIfWombatMovementIsDone()));
    layersFlipped = false;
    xMoved = yMoved = 0;
    xMoved2 = yMoved2 = 0;
    moving = scrolling = false;
    rotation = 0;
    moveMin = 0;
    moveMax = 127;
    iIndex = jIndex = 0;
    updatingTiles = false;   
    viewPosition = positionsChanged = QPoint(0,0);
    building = false;
    wombatIdScrollTarget = None;

    soundFXCoolDown=false;
    underground = false;
    diggingHole = false;
    diggingUnderground = false;
    xDigDirection=0;
    yDigDirection=0;
    undergroundChanged = false;
    lockInput = lockMovement = false;
    movingWindow = false;
    openGLNotInitilized = true;
    showOwnerBurrowId = None;
    neighbours.resize(9);

    enableEvents->setFocus(); //the fuck I have to do this??

    QSize backgroundSize = calculateBackgroundSize();

    openGL = new OpenGL(this);
    openGL->gui = this;
    openGL->console = console;
    console->openGL = openGL;    
    openGL->setup(backgroundSize);

    QGridLayout *layout = new QGridLayout(this);
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(openGL, 0, 0);
    this->setLayout(layout);

    connect(timerUpdateScreen, SIGNAL(timeout()), this, SLOT(updateOpenGLWidget()));
    timerUpdateScreen->start(gFrameRate);


    locationLabel->raise();
    enableEvents->raise();
   // console->raise();
   // consoleLine->raise();
    menuDetect->raise();
    console->raise();
   // wtf->raise();

    //showPositionInfoBox();

    controller = new Controller(); //No parent, make sure you delete properly when you quit the game
    controller->console = console;
    controller->gui = this;
    controller->openGL = openGL;
    console->controller = controller;
    controller->setupWorker(levelName,backgroundSize);

    sound = new Sound(this);
    sound->gui = this;
    sound->setup();

    honestlyGOFUCKINGDIE.start();
    gofuckyourselfpleasedeargod = 0;    
}

QSize GUI::calculateBackgroundSize(){
    //Calculate how many tiles are necassary to fill the window size
    int w = ((int)(windowSize.width()/gTileSize)+1+4); //+1 round to ceiling, +4 because we want 2 extra tiles on each side
    int h = ((int)(windowSize.height()/gTileSize)+1+4);

    if (!(w % 2 == 0)){
        w += 1;
    }

    if (!(h % 2 == 0)){
        h += 1;
    }

    w *= gTileSize;
    h *= gTileSize;

    if(w>4096){
        w=4096;
    }
    if(h>4096){
        h=4096;
    }

    return QSize (w,h);
}

void GUI::resizeEvent(QResizeEvent* event){
    windowSize = this->size();
    //console->print("wtf: "+QString::number(windowSize.width())+","+QString::number(windowSize.height()));
    int windowWidth = windowSize.width();
    int windowHeight = windowSize.height();

    //qDebug()<<"w: "<<windowSize.width();
    //qDebug()<<"h: "<<windowSize.height();
    if (windowWidth>4096){
        windowWidth = 4096;
    }

    if (windowHeight>4096){
        windowHeight = 4096;
    }

    //openGL->position = QPoint(-1*(2048-(windowWidth/2)),-1*(2048-(windowHeight/2)));;

    inGameMenu->move((windowWidth/2)-105,windowHeight-60);
    menuDetect->setGeometry(inGameMenu->geometry());

    locationLabel->setGeometry(20,10,windowWidth,61);
    console->sizeChanged();
    //console->setGeometry(0,0,windowWidth,windowHeight*0.2);
    //consoleLine->setGeometry(0,(windowHeight*0.2)-2,windowWidth,21);
    enableEvents->setGeometry(0,0,windowWidth,windowHeight);

    if (infoBoxOpen){
        QPoint pos;
        pos.setX(windowWidth-infoBox->width());
        pos.setY(windowHeight-infoBox->height());
        infoBox->move(pos);
        infoBoxOffset = infoBox->pos();
    }
    if (deathNoticeOpen){
        centerWidget(deathNotice);
        deathNoticeOffset = deathNotice->pos();
    }
    if (burrowListOpen){
        centerWidget(burrowList);
        burrowListOffset = burrowList->pos();
    }
    if (positionInfoBoxOpen){
        QPoint pos;
        pos.setX(windowWidth-131-10);
        pos.setY(10);
        positionInfoBox->move(pos);
    }
    if (createBurrowDialogOpen){
        centerWidget(createBurrowDialog);
        createBurrowDialogOffset = createBurrowDialog->pos();
    }
    if (burrowMenuOpen){
        QPoint pos;
        pos.setX(windowWidth-burrowMenu->width()-10);
        pos.setY(windowHeight/4);
        burrowMenu->move(pos);
    }

    //add again positionsChanged = viewPosition-hwGameObject->worldPosition;
    screenCenter = QPoint((windowSize.width()/2), (windowSize.height()/2));

    QSize backgroundSize = calculateBackgroundSize();
    controller->updateBackgroundSize(backgroundSize, underground);    
}

void GUI::setViewPosition(QPoint pos){
    viewPosition = pos;
}

void GUI::setXYMoved(QPoint moved){
    xMoved = xMoved2 = moved.x();
    yMoved = yMoved2 = moved.y();
}

void GUI::setUpdatingTiles(bool b){
    updatingTiles = b;
}

void GUI::startUpdateTeleport(QPoint target){
    //if (updatingTiles==false){   <--- Do I need this!?
    //console->print("start update teleport called");
    updatingTiles = true;
    //openGL->resetTranslate = true;
    viewPosition = target;    
    xMoved = xMoved2 = 0;
    yMoved = yMoved2 = 0;

    //hwHerder->removeAllWombatsFromHerd();
    //hwGameObject->prepareToUpdateTilesForTeleport(viewPosition.x(),viewPosition.y());
    //hwGameObject->updateAllTiles();
    //prepareBackgroundImage();
    //updatingTiles=false;
    //console->print("update teleport complete");
}

void GUI::startUpdate(){
    if (updatingTiles==false){
        updatingTiles = true;
        controller->startUpdate(viewPosition,underground);
    }

    /*
    if (updatingTiles==false){
        //console->print("start update called");
        updatingTiles = true;
        positionsChanged = viewPosition-hwGameObject->worldPosition;
        //hej->append("start "+QString::number(xPositionsChanged)+"yposchanged "+QString::number(yPositionsChanged));
        hwGameObject->prepareToUpdateTiles(viewPosition.x(),viewPosition.y());
        numberOfWombatsOnDisplay = hwHerder->wombatHerd.length();
        hwGameObject->updateAllTiles();
        hwHerder->updateHerd(positionsChanged,numberOfWombatsOnDisplay);
        prepareBackgroundImageInThread();
    }*/
}

void GUI::changeTerrain(QPoint pos, Terrain terrain, int burrowId, bool update){
    controller->changeTerrain(pos, terrain, burrowId, update);

    if(update){
        startUpdate();
    }
}

void GUI::addOverlay(QPoint pos, Overlay overlay, int variation, bool update){
    controller->addOverlay(pos, overlay, variation);

    if(update){
        startUpdate();
    }
}

void GUI::rotateWombat(){

    if(keyWActive&&keyAActive){        
        rotation = 135;
    }else if(keyWActive&&keyDActive){        
        rotation = 225;
    }else if(keySActive&&keyAActive){        
        rotation = 45;
    }else if(keySActive&&keyDActive){        
        rotation = 315;
    }else if (keyWActive){        
        rotation = 180;
    }else if(keyAActive){        
        rotation = 90;
    }else if(keySActive){        
        rotation = 0;
    }else if(keyDActive){        
        rotation = 270;
    }

    if (keyAActive || keyWActive || keySActive || keyDActive){        
        openGL->wombatsOnDisplay[0].rotation = rotation;
    }
}

void GUI::updateNeighboursArray(const QVector<MoveStruct> &neighbours){
    this->neighbours = neighbours;    
}

void GUI::updateNeighbour(QPoint pos, Terrain terrain, bool underground){
    for (int i=0;i<neighbours.length();i++){
        if(pos == neighbours.value(i).pos){
            if(!underground){
                neighbours[i].tileType = terrain;
            }else{
                neighbours[i].tileTypeUnderground = terrain;
            }
            break;
        }
    }
}

Terrain GUI::geTerrainNeighboursArray(QPoint pos, bool underground){
    for (int i=0;i<neighbours.length();i++){
        if(pos == neighbours.value(i).pos){
            if(!underground){
                return neighbours.value(i).tileType;
            }else{
                return neighbours.value(i).tileTypeUnderground;
            }
        }
    }
    return Terrain_None;
}

int GUI::getBurrowIdNeighboursArray(QPoint pos){
    for (int i=0;i<neighbours.length();i++){
        if(pos == neighbours.value(i).pos){
            return neighbours.value(i).burrowId;
        }
    }
    return None;
}

bool GUI::hasOverlayNeighboursArray(QPoint pos, TileType tileType, bool underground){
    for (int i=0;i<neighbours.length();i++){
        if(pos == neighbours.value(i).pos){
            if(!underground){
                for (int j=0;j<neighbours.value(i).overlayTerrainTypes.length();j++){
                    if (neighbours.value(i).overlayTerrainTypes.value(j) == tileType){
                        return true;
                    }
                }
            }else{
                for (int j=0;j<neighbours.value(i).overlayTerrainTypesUnderground.length();j++){
                    if (neighbours.value(i).overlayTerrainTypesUnderground.value(j) == tileType){
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

bool GUI::isBlocked(int x, int y){
    if (!underground){
        //Fix so we can move diagonally between bushes
        if (xMoved-moveSpeed<moveMin && yMoved-moveSpeed<moveMin){
            x = -1+viewPosition.x();
            y = -1+viewPosition.y();
        }else if (xMoved-moveSpeed<moveMin && yMoved+moveSpeed>moveMax){
            x = -1+viewPosition.x();
            y = 1+viewPosition.y();
        }else if (yMoved-moveSpeed<moveMin && xMoved+moveSpeed>moveMax){
            x = 1+viewPosition.x();
            y = -1+viewPosition.y();
        }else if (yMoved+moveSpeed>moveMax && xMoved+moveSpeed>moveMax){
            x = 1+viewPosition.x();
            y = 1+viewPosition.y();
        }else{
            x = x+viewPosition.x();
            y = y+viewPosition.y();
        }

        QPoint pos = QPoint(x,y);        
        bool hasOverlay = hasOverlayNeighboursArray(pos,Bush,false);

        if (hasOverlay){
            if (!soundFXCoolDown){                
                sound->playSoundFXBush();
                soundFXCoolDown = true;
                timerSoundFXCoolDown->start(1000);
            }
            return true;
        }else{
            return false;
        }
    }else{
        if (diggingUnderground==false){            
            xDigDirection = x;
            yDigDirection = y;
            x = x+viewPosition.x();
            y = y+viewPosition.y();            
            QPoint pos = QPoint(x,y);
            Terrain terrain = geTerrainNeighboursArray(pos,true);
            if (terrain==Terrain_UnDug){
                stopDiggingHole();
                startDiggingUnderground();
                return true;
            }else{
                return false;
            }
        }else{
            return true;
        }
    }
}

void GUI::move(){
    bool newTileLeft = false;
    bool newTileUp = false;
    bool newTileDown = false;
    bool newTileRight = false;    

    if (keyAActive){
        if (xMoved-moveSpeed<moveMin) {       //Check if there is a move constraint
            if (isBlocked(-1,0)){
                return;
            }else{
                if (xMoved-moveSpeed<0){      //Check if moved outside of tile
                    newTileLeft=true;
                }
            }
        }
    }

    if (keyWActive){
        if (yMoved-moveSpeed<moveMin) {            
            if (isBlocked(0,-1)){
                return;
            }else{
                if (yMoved-moveSpeed<0){
                    newTileUp=true;
                }
            }
        }
    }

    if (keySActive){
        if (yMoved+moveSpeed>moveMax) {
            if (isBlocked(0,1)){
                return;
            }else{
                if (yMoved+moveSpeed>127){
                    newTileDown=true;
                }
            }
        }
    }

    if (keyDActive){
        if (xMoved+moveSpeed>moveMax) {
            if (isBlocked(1,0)){
                return;
            }else{
                if (xMoved+moveSpeed>127){
                    newTileRight = true;
                }
            }
        }
    }
//If no early return, then update variables
    if (keyAActive){        
        xMoved -= moveSpeed;
        xMoved2 -= moveSpeed;        
        if(newTileLeft){
            xMoved += 128;            
            viewPosition.setX(viewPosition.x()-1);
        }
    }
    if (keyWActive){
        yMoved-=moveSpeed;
        yMoved2-=moveSpeed;       
        if(newTileUp){
            yMoved += 128;
            viewPosition.setY(viewPosition.y()-1);
        }
    }
    if (keySActive){
        yMoved+=moveSpeed;
        yMoved2+=moveSpeed;       
        if(newTileDown){
            yMoved -= 128;
            viewPosition.setY(viewPosition.y()+1);
        }
    }
    if (keyDActive){
        xMoved += moveSpeed;
        xMoved2 += moveSpeed;        
        if(newTileRight){            
            xMoved -= 128;
            viewPosition.setX(viewPosition.x()+1);
        }
    }

    if (newTileLeft || newTileUp || newTileDown || newTileRight){
        setLocationText();
        if (positionInfoBoxOpen){
            updatePositionInfoBox();
        }
    }

    if (!updatingTiles){
        if (xMoved2>=updateDistance || xMoved2<0 || yMoved2>=updateDistance || yMoved2<0){            
            startUpdate();
        }
    }
}

void GUI::stopMove(){
    if (keyAActive==false && keyWActive==false && keySActive==false && keyDActive==false){       
        moving=false;
    }
}

void GUI::setMoveMaxMin(){
    if (underground){
        moveMax = 90; //Constrain movement underground
        moveMin = 38; //Constrain movement underground
    }else{
        moveMax = 127;
        moveMin = 0;
    }
}

void GUI::setUnderground(bool b){
    if (b){
        underground=true;
        moveMax = 90; //Constrain movement underground
        moveMin = 38; //Constrain movement underground
        int tileType = geTerrainNeighboursArray(viewPosition,true);
        if (tileType==Burrow && !burrowMenuOpen){
            showBurrowMenu();
        }
    }else{
        underground=false;
        moveMax = 127;
        moveMin = 0;
        if (burrowMenuOpen){
            closeBurrowMenu();
        }
    }
    /*undergroundChanged=true;
    */
    closeInfoBox();
}

void GUI::setSelectedWombatInRegistryUnderground(bool b){
    controller->setSelectedWombatInRegistryUnderground(b);
}

void GUI::setLockMovement(bool b)
{
    lockMovement = b;
    if(lockMovement){
        moving=false;
    }
}

void GUI::setLockInput(bool b)
{
    lockInput = b;    
}

void GUI::startDiggingHole(){
    timerDiggingHole->start(250);
    diggingHole = true;
}

void GUI::stopDiggingHole(){
    timerDiggingHole->stop();
    digCount= 0;
    diggingHole = false;
}

void GUI::startDiggingUnderground(){
    timerDiggingUnderground->start(250);
    diggingUnderground = true;
}

void GUI::stopDiggingUnderground(){
    timerDiggingUnderground->stop();
    digCount= 0;
    diggingUnderground = false;
}

void GUI::digHole(){
    if (diggingHole){ //Why do I need this
        digCount +=250;

        if (digCount>=2000){
            timerDiggingHole->stop();
            digCount= 0;
            diggingHole=false;            

            int burrowId = getBurrowIdNeighboursArray(viewPosition);
            bool isBlocked = false;
            if(burrowId==None){
                DigNeighbourCheckResultStruct neighbourCheckResultStruct = checkIfNeighbouringTunnels(viewPosition);
                if(neighbourCheckResultStruct.result == DigNeighbourCheckResult_MergeBurrows){
                    isBlocked = true; //Need to merge. Block for now. Implement later...
                }else if(neighbourCheckResultStruct.result == DigNeighbourCheckResult_AddTunnelToBurrow){
                    burrowId = neighbourCheckResultStruct.burrowId;
                }
            }

            if(!isBlocked){
                if (!underground){
                    setUnderground(true);
                    setSelectedWombatInRegistryUnderground(true);
                }else{
                    setUnderground(false);
                    setSelectedWombatInRegistryUnderground(false);
                }

                changeTerrain(viewPosition,Terrain_Hole,burrowId,true);
                //updateNeighbour(viewPosition,Terrain_Hole,true);

                //Ugly fix
                if (underground==true){
                    underground=false;
                    setLocationText();
                    underground=true;
                }

                sound->playSoundFXDigHoleComplete();
            }

        }else{
            if (!soundFXCoolDown){
                sound->playSoundFXDigging();
            }
        }
    }
}

void GUI::digPath(){
    addOverlay(viewPosition,Overlay_Path,1,true);
    sound->playSoundFXBuildPath();
}

void GUI::digUnderground(){
    if (diggingUnderground){   //Why do I need this?
        digCount +=250;

        if (digCount>=1000){            
            timerDiggingUnderground->stop();            
            DigNeighbourCheckResultStruct neighbourCheckResultStruct = checkIfNeighbouringTunnels(QPoint(viewPosition.x()+xDigDirection,viewPosition.y()+yDigDirection));
            if(neighbourCheckResultStruct.result != DigNeighbourCheckResult_MergeBurrows){ //Merge burrow function not implemented yet
                if (!soundFXCoolDown){
                    sound->playSoundFXDigUndergroundComplete();
                }
                Terrain terrain = geTerrainNeighboursArray(viewPosition,true);
                if (terrain==Terrain_Burrow || neighbourCheckResultStruct.result == DigNeighbourCheckResult_AddTunnelToBurrow){
                    controller->addPosToBurrow(viewPosition,QPoint(viewPosition.x()+xDigDirection,viewPosition.y()+yDigDirection));
                }else{
                    terrain = Terrain_Tunnel;
                }
                QPoint pos = QPoint(viewPosition.x()+xDigDirection,viewPosition.y()+yDigDirection);
                changeTerrain(pos,terrain,neighbourCheckResultStruct.burrowId,true); //Updated in back end
                updateNeighbour(pos,terrain,true); //Immediate update in front end, so we don't have to wait from signal from back end
            }
            digCount = 0;
            diggingUnderground=false;
        }else{
            if (!soundFXCoolDown){
                sound->playSoundFXDigging();
            }
        }
    }
}

GUI::DigNeighbourCheckResultStruct GUI::checkIfNeighbouringTunnels(QPoint pos){
    QVector<QPoint> neighbourTilePos;
    neighbourTilePos.append(QPoint(1,0));
    neighbourTilePos.append(QPoint(0,1));
    neighbourTilePos.append(QPoint(-1,0));
    neighbourTilePos.append(QPoint(0,-1));
    QVector<int> neighbourBurrowIds;
    QVector<QPoint> neighbouringTunnels;
    int burrowId = getBurrowIdNeighboursArray(viewPosition);
    DigNeighbourCheckResultStruct returnStruct;
    returnStruct.burrowId = burrowId;

    if(burrowId == None){
        //We are in a tunnel
        //Check if there are neighbouring tunnels and burrows
        for (int i=0;i<neighbourTilePos.length();i++){
            Terrain terrain = geTerrainNeighboursArray(pos+neighbourTilePos.value(i),true);
            if(terrain == Terrain_Tunnel){
                neighbouringTunnels.append(pos+neighbourTilePos.value(i));
            }else if(terrain == Terrain_Burrow){
                int neighbourBurrow = getBurrowIdNeighboursArray(pos+neighbourTilePos.value(i));
                if(neighbourBurrow!=None){
                    int index = neighbourBurrowIds.indexOf(neighbourBurrow);
                    if(index == None){
                        neighbourBurrowIds.append(neighbourBurrow);
                    }
                }
            }
        }

        if(neighbourBurrowIds.length()>1){
            //Several neighbouring burrows. Block!
            returnStruct.result = DigNeighbourCheckResult_MergeBurrows;
        }else if(neighbourBurrowIds.length()==1){            
            //One neighbouring burrow. The current tunnel and other neighbouring tunnels need to be added to that burrow
            controller->addTunnelsToBurrow(neighbourBurrowIds.value(0),neighbouringTunnels);
            //console->print("neighbourBurrowId: "+QString::number(neighbourBurrowIds.value(0)));
            returnStruct.result = DigNeighbourCheckResult_AddTunnelToBurrow;
            returnStruct.burrowId = neighbourBurrowIds.value(0);
        }else{
            //No neighbouring burrows
            returnStruct.result = DigNeighbourCheckResult_NothingFound;
        }
    }else{
        //We are in a burrow
        //Check if there are neighbouring tunnels and burrows
        for (int i=0;i<neighbourTilePos.length();i++){
            Terrain terrain = geTerrainNeighboursArray(pos+neighbourTilePos.value(i),underground);
            if(terrain == Terrain_Tunnel){
                neighbouringTunnels.append(pos+neighbourTilePos.value(i));
            }else if(terrain == Terrain_Burrow){
                int neighbourBurrow = getBurrowIdNeighboursArray(pos+neighbourTilePos.value(i));
                if(neighbourBurrow!=None && neighbourBurrow != burrowId){
                    int index = neighbourBurrowIds.indexOf(neighbourBurrow);
                    if(index == None){
                        neighbourBurrowIds.append(neighbourBurrow);
                    }
                }
            }
        }

        if(neighbourBurrowIds.length()>0){
            //Neighbouring burrow. Need to merge. Block!
            returnStruct.result = DigNeighbourCheckResult_MergeBurrows;
        }else if(neighbouringTunnels.length()>0){
            //All these pos needs to be explored, and check if they belong to same tunnel or different tunnel
            //Then all those pos need to be added to this burrow            
            controller->addTunnelsToBurrow(burrowId,neighbouringTunnels);
            returnStruct.result = DigNeighbourCheckResult_AddTunnelToBurrow;
            returnStruct.burrowId = burrowId;
        }
    }

    return returnStruct;
}

void GUI::centerWidget(QWidget *widget){
    QPoint center;
    center.setX((windowSize.width()/2)-(widget->width()/2));
    center.setY((windowSize.height()/2)-(widget->height()/2));
    widget->move(center);
}

GUI::checkIfResourceReturnStruct GUI::checkIfResource(){
    bool edge = false;
    int x;
    int y;
    checkIfResourceReturnStruct returnData;
    returnData.tileType = None;

    if (xMoved-moveSpeed<0 && yMoved-moveSpeed<0){
        x = -1+viewPosition.x();
        y = -1+viewPosition.y();
        edge = true;
    }else if (xMoved-moveSpeed<0 && yMoved+moveSpeed>127){
        x = -1+viewPosition.x();
        y = 1+viewPosition.y();
        edge = true;
    }else if (yMoved-moveSpeed<0 && xMoved+moveSpeed>127){
        x = 1+viewPosition.x();
        y = -1+viewPosition.y();
        edge = true;
    }else if (yMoved+moveSpeed>127 && xMoved+moveSpeed>127){
        x = 1+viewPosition.x();
        y = 1+viewPosition.y();
        edge = true;
    }else if (xMoved-moveSpeed<0){
        x = -1+viewPosition.x();
        y = viewPosition.y();
        edge = true;
    }else if (yMoved-moveSpeed<0){
        x = viewPosition.x();
        y = -1+viewPosition.y();
        edge = true;
    }else if (xMoved+moveSpeed>127){
        x = 1+viewPosition.x();
        y = viewPosition.y();
        edge = true;
    }else if (yMoved+moveSpeed>127){
        x = viewPosition.x();
        y = 1+viewPosition.y();
        edge = true;
    }

    if (edge == true){
        bool hasBushOverlay = hasOverlayNeighboursArray(QPoint(x,y),Bush,false);
        if (hasBushOverlay){
            returnData.pos = QPoint(x,y);
            returnData.tileType = Bush;
            return returnData;
        }
    }else{
        bool hasRiceGrass = hasOverlayNeighboursArray(viewPosition,RiceGrass,underground);
        if (hasRiceGrass){
            returnData.pos = viewPosition;
            returnData.tileType = RiceGrass;
            return returnData;
        }else{
            bool hasGrass = hasOverlayNeighboursArray(viewPosition,Grass,underground);
            if (hasGrass){
                returnData.pos = viewPosition;
                returnData.tileType = Grass;
                return returnData;
            }
        }
    }

    return returnData;
}

void GUI::claimResource(){
   checkIfResourceReturnStruct tile = checkIfResource();
    if(tile.tileType == Bush || tile.tileType == RiceGrass || tile.tileType == Grass){
        controller->claimResource(tile.pos,tile.tileType);
    }
}

void GUI::collectResource(){
    checkIfResourceReturnStruct tile = checkIfResource();
    if(tile.tileType == Bush || tile.tileType == RiceGrass || tile.tileType == Grass){
        int timeToCollect = 3000;
        showProgressWidget(timeToCollect);
    }
}

void GUI::stopCollectResource(){
    if(progressWidgetOpen){
        closeProgressWidget();
    }
}

void GUI::progressWidgetDone(){
    closeProgressWidget();

    if(keyCollectActive){
        checkIfResourceReturnStruct tile = checkIfResource();
        if(tile.tileType == Bush || tile.tileType == RiceGrass || tile.tileType == Grass){
            controller->collectResource(tile.pos,tile.tileType);
        }

        int timeToCollect = 3000;
        showProgressWidget(timeToCollect);
    }
}


void GUI::setLocationText(){
    if(!underground){
        if (geTerrainNeighboursArray(viewPosition,false)==Hole){
            if(geTerrainNeighboursArray(viewPosition,true)==Burrow){
                controller->getBurrowName(viewPosition); // setLocationText() will be called on return
                return;
            }
        }else if (locationLabel->isVisible()){
            locationLabel->setVisible(false);
        }
    }

}

void GUI::setLocationText(QString name){
    locationLabel->setVisible(true);
    locationLabel->setText(name);
}

void GUI::screenClicked(){

    bool update = false;

    //Hide owner overlay
    if(showOwnerBurrowId != None){
        controller->hideOwner(showOwnerBurrowId);
        showOwnerBurrowId = None;
        update = true;
    }
    //


    int wombatId = checkIfWombatClicked(); // Check if we clicked on a wombat
    if(wombatId!=None){
        controller->prepareWombatInfoForDisplay(wombatId); //displayWombatInfo() will be called on return
    }else{
        QPoint pos = getWorldPositionAtClick();
        //int tileType = getTileTypeNeighboursArray(pos,true);
        //console->print("clickx: "+QString::number(pos.x())+","+QString::number(pos.y()));
        //console->print("tiletype: "+QString::number(tileType));
        controller->prepareTileInfoForDisplay(pos); //displayTileInfo() will be called on return

        //Show owner overlay
        Terrain terrain = geTerrainNeighboursArray(pos,underground);
        if(terrain == Terrain_Hole){
            int burrowId = getBurrowIdNeighboursArray(pos);
            if(burrowId!=None){                
                controller->showOwner(burrowId);
                showOwnerBurrowId = burrowId;
                update = true;
            }
        }
        //
    }

    if(update){
        startUpdate();
    }
}

void GUI::screenDoubleClicked(){    
    QPoint pos = getWorldPositionAtClick();

    int wombatId = checkIfWombatClicked(); // Check if we clicked on a wombat
    if(wombatId!=None){
        showWInventoryWidget(wombatId);
    }else{
        if (underground){
            //You probably need some check here!! Isn't it a bit stupid like it is now?
            //TileType tileType = getTileTypeNeighboursArray(pos,underground);
            //if (tileType == Burrow){
            //bool storageClicked = hasOverlayNeighboursArray(pos,Storage,true);

            //if(storageClicked){
            controller->prepareStorageInfoForShowStorageWidget(pos);
            //}
            //}
        }
    }
}

void GUI::buildBulding(){    
    int tileType = geTerrainNeighboursArray(viewPosition,true);
    bool hasHole = hasOverlayNeighboursArray(viewPosition,HoleLight,true);
    bool hasStorage = hasOverlayNeighboursArray(viewPosition,Storage,true);
    bool hasNest = hasOverlayNeighboursArray(viewPosition,Nest,true);

    if (tileType == Burrow && !hasHole && !hasStorage && !hasNest){
        controller->addBuilding(viewPosition,burrowMenu->buildingType,1);
        startUpdate();
    }
}

int GUI::checkIfWombatClicked(){
    QPoint pos = this->mapFromGlobal(QCursor::pos())-screenCenter; //Click position, as pixeloffset from middle of screen

    // Check if the click position overlaps with any of the wombats currently on display
    int wombatId = None;
    bool wombatClicked = false;
    int wWidth = 64;
    int wLength = 128;
    int boxOffset = 64;
    for (int i=0;i<openGL->wombatsOnDisplay.length();i++){
        if (openGL->wombatsOnDisplay.value(i).underground == underground){
            int age = openGL->wombatsOnDisplay.value(i).age;
            float factor;
            if (age>1){
                factor = (2.0f/128.0f); //Grown wombat size
            }else if (age==1){
                factor = (2.0f/104.0f);
                wWidth = 104/2;
                wLength = 104;
                boxOffset = 104/2;
            }else{
                factor = (2.0f/80.0f); // Joey wombat size
                wWidth = 80/2;
                wLength = 80;
                boxOffset = 80/2;
            }

            QPointF wombatPosPx = openGL->wombatsOnDisplay.value(i).glPos/(factor); //pos in pixels

            float xOffset = xMoved2-openGL->wombatsOnDisplay.value(i).originalOffset.x();
            float yOffset = yMoved2-openGL->wombatsOnDisplay.value(i).originalOffset.y();

            //console->print("xy in paintGL"+QString::number(x)+","+QString::number(y)+"xyMoved"+QString::number(hwGameUI->xMoved2)+","+QString::number(hwGameUI->yMoved2)+"originaloffset"+QString::number(wombatsOnDisplay[i].originalOffset.x())+","+QString::number(wombatsOnDisplay[i].originalOffset.y()));

            if (i!=0){ //Only add offset if it's not the selected wombat
                wombatPosPx = wombatPosPx-QPointF(xOffset,yOffset);
            }

            QRect boundingRect;

            qreal rotation = openGL->wombatsOnDisplay.value(i).rotation;
            if (rotation == 90 || rotation == 270){
                boundingRect = QRect(wombatPosPx.x()-boxOffset,wombatPosPx.y()-(boxOffset/2),wLength,wWidth);
            }else{
                boundingRect = QRect(wombatPosPx.x()-(boxOffset/2),wombatPosPx.y()-boxOffset,wWidth,wLength);
            }

            wombatClicked = boundingRect.contains(pos); //Check if the click position is within the boundingRect of this wombat

            //console->print("pos "+QString::number(pos.x())+","+QString::number(pos.y()));
            //console->print("wombatpos "+QString::number(lucky.x())+","+QString::number(lucky.y()));
            //console->print("match = "+QString::number(match));

            if (wombatClicked){
                /*QLabel *hehe = new QLabel(this);
                hehe->setStyleSheet("QLabel { font-family:'MoolBoran'; font-size: 36pt; color: white; background:red;}");
                hehe->setGeometry(boundingRect);
                hehe->move(QPoint(boundingRect.topLeft()+screenCenter));
                hehe->setVisible(true);
                console->print("clickPos: "+QString::number(this->mapFromGlobal(QCursor::pos()).x())+","+QString::number(this->mapFromGlobal(QCursor::pos()).y()));
                console->print("topLeft: "+QString::number(boundingRect.topLeft().x()+(this->width()/2))+","+QString::number(boundingRect.topLeft().x()+(this->height()/2)));*/

                wombatId = openGL->wombatsOnDisplay.value(i).wombatId;
                break;
            }
        }
    }

    return wombatId;
}

void GUI::displayWombatInfo(QVector<QString> strings, int wombatId, bool setPossessButtonVisible){
    updateInfoBox(strings.value(0),strings.value(1),strings.value(2),strings.value(3),strings.value(4));
    infoBox->wombatId = wombatId;

    if (setPossessButtonVisible){
        updateInfoBoxPossessButton(true);
    }
}

void GUI::displayTileInfo(QVector<QString> strings){    
    updateInfoBox(strings.value(0),strings.value(1),strings.value(2),strings.value(3),strings.value(4));
}

void GUI::updateInfoBox(QString string1, QString string2, QString string3, QString string4, QString string5){
    if (!infoBoxOpen){
        showInfoBox();
    }

    if (infoBoxOpen){
        //If open, set info
        QLabel* info1 = infoBox->findChild<QLabel*>("info1");
        QLabel* info2 = infoBox->findChild<QLabel*>("info2");
        QLabel* info3 = infoBox->findChild<QLabel*>("info3");
        QLabel* info4 = infoBox->findChild<QLabel*>("info4");
        QLabel* info5 = infoBox->findChild<QLabel*>("info5");

        info1->setText(string1);
        info2->setText(string2);
        info3->setText(string3);
        info4->setText(string4);
        info5->setText(string5);

        updateInfoBoxPossessButton(false);
    }
}

void GUI::updateInfoBoxPossessButton(bool b){
    QPushButton* possess = infoBox->findChild<QPushButton*>("infoBoxPossess");
    possess->setVisible(b);
}

void GUI::possess(int wombatId){
    if (deathNoticeOpen){
        closeDeathNotice();
    }

    if (lockInput==false){
        moving=false;
        lockInput=true;
        //updatingTiles = true;
        controller->possess(wombatId,viewPosition,QPoint(xMoved,yMoved),underground,rotation);
    }
}

void GUI::setScrollTarget(int wombatId){
    //This will scroll to a certain world pos and then update the tiles    
    //console->print("target tile pos: "+QString::number(targetTilePos.x())+","+QString::number(targetTilePos.y()));
    //positionsChanged = targetTilePos-viewPosition;
    //console->print("xy changed: "+QString::number(xPositionsChanged)+","+QString::number(yPositionsChanged));
    //viewPosition = targetTilePos;

    //int index = hwHerder->wombatIds.indexOf(hwRegistry->selectedWombat);
    //targetInternalPos = hwHerder->wombatHerd.value(index)->getInternalPosPx(hwHerder->wombatHerd.value(index)->currentPxOffset);
    //console->print("targetInternalPos: "+QString::number(targetInternalPos.x())+","+QString::number(targetInternalPos.y()));

    QPointF start = openGL->wombatsOnDisplay.value(0).glPos;
    int wombatOnDisplayIndex = openGL->getWombatOnDisplayIndex(wombatId);
    QPointF end = openGL->wombatsOnDisplay.value(wombatOnDisplayIndex).glPos;

    //TempFix FUCK!!
    if(openGL->wombatsOnDisplay.value(wombatOnDisplayIndex).positionBuffer.length() != 0){
        QPointF endPos = QPointF(0,0);
        for (int i=0;i<openGL->wombatsOnDisplay.value(wombatOnDisplayIndex).positionBuffer.length();i++){
            wombatPosBufferStruct nextPos = openGL->wombatsOnDisplay.value(wombatOnDisplayIndex).positionBuffer.value(i);
            endPos += nextPos.pxChange;
        }

        wombatPosBufferStruct lastPos = openGL->wombatsOnDisplay.value(wombatOnDisplayIndex).positionBuffer.last();

        float factor;
        if (lastPos.wombatAge>1){
            factor = (2.0f/128.0f); //Grown wombat size
        }else if (lastPos.wombatAge==1){
            factor = (2.0f/104.0f);
        }else{
            factor = (2.0f/80.0f); // Joey wombat size
        }

        end = openGL->wombatsOnDisplay.value(wombatOnDisplayIndex).glPos + QPointF((float)(endPos.x() * factor), (float)(endPos.y()*factor));
    }
    //


    //int age =   openGL->wombatsOnDisplay[hwHerder->wombatHerd[index]->hIndex+1].age;
    //console->print("age target wombat "+QString::number(age));
    QPointF diff = start-end;
    //console->print("openGL pos diff "+QString::number(diff.x())+","+QString::number(diff.y()));
    diff = diff/(2.0f/128.0f);
    //console->print("openGL pos diff pixel "+QString::number(diff.x())+","+QString::number(diff.y()));
    QPointF originalOffset = openGL->wombatsOnDisplay.value(wombatOnDisplayIndex).originalOffset;
    //console->print("openGL pos "+QString::number(diff.x())+","+QString::number(diff.y()));
    int xx = xMoved2-originalOffset.x();
    int yy = yMoved2-originalOffset.y();
    diff = diff + QPointF(xx,yy);
    //console->print("openGL pos orioffset corrected "+QString::number(diff.x())+","+QString::number(diff.y()));

    scrollTarget = QPoint(-diff.x(),-diff.y());
    //scrollTarget = QPoint(x,y);
    //console->print("scroll target: "+QString::number(scrollTarget.x())+","+QString::number(scrollTarget.y()));
    scrollPos = QPoint(0,0);

    wombatIdScrollTarget = wombatId;
    timerCheckIfWombatMovementIsDone->start(200); //Will start scrolling when the target wombat has stopped moving
    /*
    if(openGL->wombatsOnDisplay.value(wombatOnDisplayIndex).moving){
        GUI *gui = this;
        QTimer::singleShot(2000, this, [gui] {
            gui->scrolling = true; //OpenGL will call scrollToTargetPos when scrolling is true
        });
    }else{
        scrolling = true; //OpenGL will call scrollToTargetPos when scrolling is true
    }
*/

}

void GUI::checkIfWombatMovementIsDone(){
    int wombatOnDisplayIndex = openGL->getWombatOnDisplayIndex(wombatIdScrollTarget);
    if(!openGL->wombatsOnDisplay.value(wombatOnDisplayIndex).moving){
        //The wombat has stopped moving
        timerCheckIfWombatMovementIsDone->stop();
        scrolling = true; //OpenGL will call scrollToTargetPos when scrolling is true
        wombatIdScrollTarget = None;
    }
}

void GUI::scrollToTargetPos(){

    if (scrollPos.x()-scrollTarget.x()>=4 || scrollPos.x()-scrollTarget.x()<=-4){
        if (scrollPos.x()<scrollTarget.x()){ // If close to target we have to scroll slower to hit the exact right pos
            scrollPos.setX(scrollPos.x()+moveSpeed);
            xMoved += moveSpeed;
            xMoved2 += moveSpeed;
        }else if (scrollPos.x()>scrollTarget.x()){
            scrollPos.setX(scrollPos.x()-moveSpeed);
            xMoved -= moveSpeed;
            xMoved2 -= moveSpeed;
        }
    }else{
        if (scrollPos.x()<scrollTarget.x()){ // If close to target we have to scroll slower to hit the exact right pos
            scrollPos.setX(scrollPos.x()+1);
            xMoved += 1;
            xMoved2 += 1;
        }else if (scrollPos.x()>scrollTarget.x()){
            scrollPos.setX(scrollPos.x()-1);
            xMoved -= 1;
            xMoved2 -= 1;
        }
    }

    if (scrollPos.y()-scrollTarget.y()>=4 || scrollPos.y()-scrollTarget.y()<=-4){
        if (scrollPos.y()<scrollTarget.y()){
            scrollPos.setY(scrollPos.y()+moveSpeed);
            yMoved += moveSpeed;
            yMoved2 += moveSpeed;
        }else if (scrollPos.y()>scrollTarget.y()){
            scrollPos.setY(scrollPos.y()-moveSpeed);
            yMoved -= moveSpeed;
            yMoved2 -= moveSpeed;
        }

    }else{
        if (scrollPos.y()<scrollTarget.y()){
            scrollPos.setY(scrollPos.y()+1);
            yMoved += 1;
            yMoved2 += 1;
        }else if (scrollPos.y()>scrollTarget.y()){
            scrollPos.setY(scrollPos.y()-1);
            yMoved -= 1;
            yMoved2 -= 1;
        }
    }

    bool newTile = false;
    if (xMoved<0){      //Check if moved outside of tile
        xMoved += 128;
        viewPosition.setX(viewPosition.x()-1);
        newTile = true;
    }

    if (yMoved<0){
        yMoved += 128;
        viewPosition.setY(viewPosition.y()-1);
        newTile = true;
    }

    if (yMoved>127){
        yMoved -= 128;
        viewPosition.setY(viewPosition.y()+1);
        newTile = true;
    }

    if (xMoved>127){
        xMoved -= 128;
        viewPosition.setX(viewPosition.x()+1);
        newTile = true;
    }

    if (newTile){
        if (positionInfoBoxOpen){
            updatePositionInfoBox();
        }
    }

    if (!updatingTiles){
        if (xMoved2>=updateDistance || xMoved2<0 || yMoved2>=updateDistance || yMoved2<0){
            //console->print("update started");
            startUpdate();
        }
    }

    if(scrollPos == scrollTarget){
        controller->scrollToTargetPosDone();
        scrolling = false;
        enableEvents->setFocus();  // Temp       
    }
}

void GUI::updatePositionInfoBox(){
    //positionInfoBox->setText("World:\nx:"+QString::number(viewPosition.x())+"\ny:"+QString::number(viewPosition.y())+"\nChunk:\nx:"+QString::number(hwGameObject->chunkPosition.x())+"\ny:"+QString::number(hwGameObject->chunkPosition.y()));
    positionInfoBox->setText("World:\nx:"+QString::number(viewPosition.x())+"\ny:"+QString::number(viewPosition.y()));
}

void GUI::clearSoundFXCoolDown(){
    soundFXCoolDown=false;
    timerSoundFXCoolDown->stop();
}

void GUI::taunt(){
    if (!soundFXCoolDown){
        sound->playSoundFX(Sound_Taunt);
        soundFXCoolDown = true;
        timerSoundFXCoolDown->start(2000);
        QTimer::singleShot(2000, [=] {
            controller->taunt();
        });
    }
}

QPoint GUI::getWorldPositionAtClick(){
    QPoint tileCenter = QPoint(gTileSize/2,gTileSize/2);
    QPoint diff = this->mapFromGlobal(QCursor::pos())-screenCenter-tileCenter+QPoint(xMoved,yMoved); //Pixeloffset from middle of tile with upperleft corner at middle of screen
    diff = diff/gTileSize; //Convert pixel difference to tile difference
    QPoint p = diff+viewPosition; //Add to viewposition to get world position
    //console->print("click pos: "+QString::number(p.x())+" y: "+QString::number(p.y()));
    return p;
}

QPoint GUI::getMainWindowOffset(){
    emit s_getMainWindowState(); //Such a retarded way of doing it, but I could not figure out another way to access MainWindow

    if (mainWindowState==Qt::WindowMaximized){
        return QPoint(0,0);
    }else{
        return QPoint(160,70); //the fuck?
    }
}

/*
QPoint GUI::getVisibleTileIndexAtClick(){
    //Get world position at click position
    QPoint w = center;  //Screen position of current viewPosition
    QPoint click = this->mapFromGlobal(QCursor::pos());
    click = click-w;           //Difference
    double dx = click.x();
    double dy = click.y();

    emit s_getMainWindowState(); //Such a retarded way of doing it, but I could not figure out another way to access MainWindow

    if (mainWindowState==Qt::WindowMaximized){

    }else{
        dx = dx-160; //the fuck?
        dy = dy-70; //the fuck?
    }

    int tx = xMoved;
    int ty = yMoved;

    dx = dx+tx;
    dy = dy+ty;

    int x = qFloor(dx/128)-1; //Convert pixel difference to tile difference
    int y = qFloor(dy/128)-1;

    x = x + 15; //WorldPos is at array index 15,15, so just add 15,15 to the current offset and you have the visibleTile index
    y = y + 15;

    QPoint p = QPoint (x,y);
    console->print("click pos: "+QString::number(x)+" y: "+QString::number(y));
    return p;
}
*/
bool GUI::eventFilter(QObject *obj, QEvent *event){
    // This function repeatedly call for those QObjects
    // which have installed eventFilter (Step 2)
    if (obj == (QObject*)this){

        if (event->type() == QEvent::KeyPress && lockInput==false){
            QKeyEvent *KeyEvent = (QKeyEvent*)event;

            switch(KeyEvent->key()){
            case Qt::Key_section:
                console->toggleConsole();
                return true;
            case Qt::Key_A:
                if (keyAActive == false && lockMovement==false){
                    if (!moving){
                        moving=true;
                    }
                    keyAActive=true;
                    rotateWombat();
                    if (diggingHole){
                        stopDiggingHole();
                    }else if (diggingUnderground){
                        stopDiggingUnderground();
                    }
                }
                return true;
            case Qt::Key_W:
                if (keyWActive == false && lockMovement==false){
                    if (!moving){
                        moving=true;
                    }
                    keyWActive=true;
                    rotateWombat();
                    if (diggingHole){
                        stopDiggingHole();
                    }else if (diggingUnderground){
                        stopDiggingUnderground();
                    }
                }
                return true;
            case Qt::Key_S:
                if (keySActive == false && lockMovement==false){
                    if (!moving){
                        moving=true;
                    }
                    keySActive=true;
                    rotateWombat();
                    if (diggingHole){
                        stopDiggingHole();
                    }else if (diggingUnderground){
                        stopDiggingUnderground();
                    }
                }
                return true;
            case Qt::Key_D:
                if (keyDActive == false && lockMovement==false){
                    if (!moving){
                        moving=true;
                    }
                    keyDActive=true;
                    rotateWombat();
                    if (diggingHole){
                        stopDiggingHole();
                    }else if (diggingUnderground){
                        stopDiggingUnderground();
                    }
                }
                return true;
            case Qt::Key_Shift:
                if (keyShiftActive==false && lockMovement==false){
                    keyShiftActive=true;
                    if (underground==false && console->isConsoleLineSelected()==false && updatingTiles==false){
                        moving=false;
                        if (geTerrainNeighboursArray(viewPosition,underground)==Hole){
                            //console->print("I am here");
                            setUnderground(true);
                            setSelectedWombatInRegistryUnderground(true);
                            startUpdate();
                        }else{
                            startDiggingHole();
                        }
                    }
                }
                return true;
            case Qt::Key_Space:
                if (keySpaceActive==false && lockMovement==false){
                    keySpaceActive=true;
                    if (underground==true && updatingTiles==false){
                        moving=false;
                        stopDiggingUnderground();

                        Terrain terrain = geTerrainNeighboursArray(viewPosition,false);
                        bool hasOverlayBush = hasOverlayNeighboursArray(viewPosition,Bush,false);

                        //console->print("trying:" + QString::number(tileType));
                        if (terrain==Terrain_Hole){
                            setUnderground(false);
                            setSelectedWombatInRegistryUnderground(false);
                            startUpdate();
                        }else if (!hasOverlayBush) {
                            startDiggingHole();
                        }
                    }
                }
                return true;
            case Qt::Key_B:
                if (keyBActive==false && lockMovement==false){
                    keyBActive=true;
                    if (createBurrowDialogOpen==false){
                        int tileType = geTerrainNeighboursArray(viewPosition,true);
                        int tileType2 = geTerrainNeighboursArray(viewPosition,false);

                        if(building){
                            buildBulding();
                        }else{
                            if ((tileType==Tunnel && underground)||(tileType==Tunnel && tileType2==Hole)){
                                showCreateBurrowDialog();
                            }else if ((tileType==Burrow && underground)||(tileType==Burrow && tileType2==Hole && !underground)){
                                controller->changeHomeBurrow(viewPosition);
                                sound->playSoundFX(Sound_NewHome);
                            }
                        }
                    }else{
                        closeCreateBurrowDialog();
                    }
                }
                return true;
            case Qt::Key_C:
                    if (keyCActive==false && lockMovement==false){
                        keyCActive=true;
                        if (!underground){
                            claimResource();
                        }
                    }
                    return true;
            case Qt::Key_T:
                    if (keyTauntActive==false && lockMovement==false){
                        keyTauntActive=true;
                            taunt();
                    }
                    return true;
            case Qt::Key_F:
                    if (keyCollectActive==false && lockMovement==false){
                        keyCollectActive=true;
                        lockMovement=true;
                        keyAActive = keyWActive = keySActive = keyDActive = false;
                        stopMove();
                        collectResource();
                    }
                    return true;
            default:
                break;
            }
        }else if (event->type() == QEvent::KeyRelease){
            QKeyEvent *KeyEvent = (QKeyEvent*)event;

            switch(KeyEvent->key())
            {
            case Qt::Key_W:
                if (KeyEvent->isAutoRepeat()==false){
                    keyWActive=false;
                    stopMove();
                    rotateWombat();
                    if (diggingUnderground){
                        stopDiggingUnderground();
                    }
                }
                return true;
            case Qt::Key_A:
                if (KeyEvent->isAutoRepeat()==false){
                    keyAActive=false;
                    stopMove();
                    rotateWombat();
                    if (diggingUnderground){
                        stopDiggingUnderground();
                    }
                }
                return true;
            case Qt::Key_S:
                if (KeyEvent->isAutoRepeat()==false){
                    keySActive=false;
                    stopMove();
                    rotateWombat();
                    if (diggingUnderground){
                        stopDiggingUnderground();
                    }
                }
                return true;
            case Qt::Key_D:
                if (KeyEvent->isAutoRepeat()==false){
                    keyDActive=false;
                    stopMove();
                    rotateWombat();
                    if (diggingUnderground){
                        stopDiggingUnderground();
                    }
                }
                return true;
            case Qt::Key_Shift:
                if (KeyEvent->isAutoRepeat()==false){
                    keyShiftActive=false;
                    if (digCount>=750 && digCount<2000){
                        digPath();
                    }
                    stopDiggingHole();
                }
                return true;
            case Qt::Key_Space:
                if (KeyEvent->isAutoRepeat()==false){
                    keySpaceActive=false;
                    stopDiggingHole();
                }
                return true;
            case Qt::Key_B:
                if (KeyEvent->isAutoRepeat()==false){
                    keyBActive=false;
                }
                return true;
            case Qt::Key_C:
                if (KeyEvent->isAutoRepeat()==false){
                    keyCActive=false;
                }
                return true;
            case Qt::Key_T:
                if (KeyEvent->isAutoRepeat()==false){
                    keyTauntActive=false;
                }
                return true;
            case Qt::Key_F:
                if (KeyEvent->isAutoRepeat()==false){
                    keyCollectActive=false;
                    lockMovement=false;
                    if(progressWidgetOpen){
                        closeProgressWidget();
                    }
                }
                return true;
            default:
                break;
            }

        }    return QWidget::eventFilter(obj, event);
    }else if (obj == (QObject*)enableEvents && lockInput==false){

        if(event->type() == QEvent::MouseButtonDblClick){
            screenDoubleClicked();

        }else if (event->type() == QEvent::MouseButtonPress){
            enableEvents->setFocus();
            screenClicked();
            //if (god==true){
            //    QPoint pos = getWorldPositionAtClick();
                //console->print("x: "+QString::number(pos.x())+" y: "+QString::number(pos.y()));
            //    hwGameObject->changeTile(pos.x(),pos.y(),uiTileType,None,true);
            //}else{
            //    screenClicked();
            //}
        }
           return QWidget::eventFilter(obj, event);

    }else if (obj == (QObject*)createBurrowDialog){

        if (event->type() == QEvent::MouseButtonPress){
            const QMouseEvent* const mouseEvent = static_cast<const QMouseEvent*>( event ); //why the fuck can I not check left mouse button in QEvent??
            if(mouseEvent->buttons() == Qt::LeftButton){
                movingWindow = true;
                cursorPos=this->mapFromGlobal(QCursor::pos());
            }
        }else if (event->type() == QEvent::MouseMove){
            moveWidget(createBurrowDialog,createBurrowDialogOffset);

        }else if (event->type() == QEvent::MouseButtonRelease){
            if (movingWindow){
                movingWindow=false;
                createBurrowDialogOffset = createBurrowDialog->pos();
            }

        }
           return QWidget::eventFilter(obj, event);

    }else if (obj == (QObject*)menuDetect){

        if (event->type() == QEvent::Enter && lockInput==false){
            showMenu();
        }

           return QWidget::eventFilter(obj, event);

        }else if (obj == (QObject*)inGameMenu){

        if (event->type() == QEvent::Leave){
            if (!burrowListOpen){
                hideMenu();
            }
        }

           return QWidget::eventFilter(obj, event);

   }else if (obj == (QObject*)burrowList){

        if (event->type() == QEvent::MouseButtonPress){
            const QMouseEvent* const mouseEvent = static_cast<const QMouseEvent*>( event ); //why the fuck can I not check left mouse button in QEvent??
            if(mouseEvent->buttons() == Qt::LeftButton){
                movingWindow = true;
                cursorPos=this->mapFromGlobal(QCursor::pos());
            }
        }else if (event->type() == QEvent::MouseMove){
            moveWidget(burrowList,burrowListOffset);

        }else if (event->type() == QEvent::MouseButtonRelease){
            if (movingWindow){
                movingWindow=false;
                burrowListOffset = burrowList->pos();
            }

        }

           return QWidget::eventFilter(obj, event);

    }else if (obj == (QObject*)infoBox){

    if (event->type() == QEvent::MouseButtonPress){
        const QMouseEvent* const mouseEvent = static_cast<const QMouseEvent*>( event ); //why the fuck can I not check left mouse button in QEvent??
        if(mouseEvent->buttons() == Qt::LeftButton){
            movingWindow = true;
            cursorPos=this->mapFromGlobal(QCursor::pos());
        }
    }else if (event->type() == QEvent::MouseMove){
        moveWidget(infoBox,infoBoxOffset);

    }else if (event->type() == QEvent::MouseButtonRelease){
        if (movingWindow){
            movingWindow=false;
            infoBoxOffset = infoBox->pos();
        }

    }

       return QWidget::eventFilter(obj, event);

   }else if (obj == (QObject*)deathNotice){

    if (event->type() == QEvent::MouseButtonPress){
        const QMouseEvent* const mouseEvent = static_cast<const QMouseEvent*>( event ); //why the fuck can I not check left mouse button in QEvent??
        if(mouseEvent->buttons() == Qt::LeftButton){
            movingWindow = true;
            cursorPos=this->mapFromGlobal(QCursor::pos());
        }
    }else if (event->type() == QEvent::MouseMove){
        moveWidget(deathNotice,deathNoticeOffset);

    }else if (event->type() == QEvent::MouseButtonRelease){
        if (movingWindow){
            movingWindow=false;
            deathNoticeOffset = deathNotice->pos();
        }

    }

       return QWidget::eventFilter(obj, event);

   }else if (obj == (QObject*)storageWidgetBackground){

    if (event->type() == QEvent::MouseButtonPress){
        const QMouseEvent* const mouseEvent = static_cast<const QMouseEvent*>( event ); //why the fuck can I not check left mouse button in QEvent??
        if(mouseEvent->buttons() == Qt::LeftButton){
            movingWindow = true;
            cursorPos=this->mapFromGlobal(QCursor::pos());
        }
    }else if (event->type() == QEvent::MouseMove){
        moveWidget(storageWidget,storageWidgetOffset);

    }else if (event->type() == QEvent::MouseButtonRelease){
        if (movingWindow){
            movingWindow=false;
            storageWidgetOffset = storageWidget->pos();
        }

    }

       return QWidget::eventFilter(obj, event);

    }else if (obj == (QObject*)wInventoryWidgetBackground){

    if (event->type() == QEvent::MouseButtonPress){
        const QMouseEvent* const mouseEvent = static_cast<const QMouseEvent*>( event ); //why the fuck can I not check left mouse button in QEvent??
        if(mouseEvent->buttons() == Qt::LeftButton){
            movingWindow = true;
            cursorPos=this->mapFromGlobal(QCursor::pos());
        }
    }else if (event->type() == QEvent::MouseMove){
        moveWidget(wInventoryWidget,wInventoryWidgetOffset);

    }else if (event->type() == QEvent::MouseButtonRelease){
        if (movingWindow){
            movingWindow=false;
            wInventoryWidgetOffset = wInventoryWidget->pos();
        }

    }

       return QWidget::eventFilter(obj, event);

   }else {
        // pass the event on to the parent class
        return QWidget::eventFilter(obj, event);
    }
}

void GUI::moveWidget(QWidget *widget,QPoint offset){    
    if (movingWindow){
        // Messy, clean it up!
        QPoint temp = this->mapFromGlobal(QCursor::pos())-cursorPos+offset;
        QPoint temp2;
        if (temp.x()  > 0 && temp.x()+widget->width() < windowSize.width()){
            temp2.setX(temp.x());
            temp2.setY(widget->pos().y());
            widget->move(temp2);
        }else if (temp.x()  <= 0){
            temp2.setX(0);
            temp2.setY(widget->pos().y());
            widget->move(temp2);
        }else {
            temp2.setX(windowSize.width()-widget->width());
            temp2.setY(widget->pos().y());
            widget->move(temp2);
        }
        if (temp.y()  > 0 && temp.y()+widget->height() < windowSize.height()){
            temp2.setX(widget->pos().x());
            temp2.setY(temp.y());
            widget->move(temp2);
        }else if (temp.y()  <=0){
            temp2.setX(widget->pos().x());
            temp2.setY(0);
            widget->move(temp2);
        }else{
            temp2.setX(widget->pos().x());
            temp2.setY(windowSize.height()-widget->height());
            widget->move(temp2);
        }
    }
}

void GUI::on_quit_clicked(){ //Quit game
    controller->quitGame(viewPosition);
    controller->deleteLater();
    emit s_backToHWMenu();
}

void GUI::updateOpenGLWidget(){
   /* if(gofuckyourselfpleasedeargod>120){
        gofuckyourselfpleasedeargod = 0;
        int msSinceLast3 = honestlyGOFUCKINGDIE.restart();
        int sSinceLast3 = msSinceLast3 / 1000;
        console->print("time since last 120 update signal was sent to OpenGL from gui: "+QString::number(msSinceLast3)+" ms "+QString::number(sSinceLast3)+" seconds");
    }else{
        gofuckyourselfpleasedeargod++;
    }*/

    openGL->update();    
}

void GUI::showMenu(){
    inGameMenu->show();
    inGameMenu->raise();
}

void GUI::hideMenu(){
    inGameMenu->hide();
    menuDetect->raise();
}

//***************
//*** Widgets ***
//***************

void GUI::showInfoBox(){
    //If not open, create and show
    infoBoxOpen = true;
    controller->setInfoBoxOpen(true);
    infoBox = new InfoBox(this);
    infoBox->gui = this;
    QPoint pos;
    pos.setX(windowSize.width()-infoBox->width());
    pos.setY(windowSize.height()-infoBox->height());    
    infoBox->move(pos);
    infoBox->show();
    infoBox->installEventFilter(this);
    infoBoxOffset = infoBox->pos();
    console->raise();
}

void GUI::closeInfoBox(){
    if (infoBoxOpen){
        infoBoxOpen = false;
        controller->setInfoBoxOpen(false);
        infoBox->deleteLater();
        enableEvents->setFocus();

        //Hide owner overlay
        if(showOwnerBurrowId != None){
            controller->hideOwner(showOwnerBurrowId);
            showOwnerBurrowId = None;
            startUpdate();
        }
        //
    }
}

void GUI::on_showBurrowList_clicked(){
    if (burrowListOpen){
        closeBurrowList();
    }else{
        burrowListOpen = true;
        /*if(hwGameObject->gameOver==false){
            timerDeathNotice->stop(); //Do not show death notice if user has already clicked on burrowlist
        }else{
            setLocationText(QString("GAME OVER!")); //And because of that, we need this
        }*/
        burrowList = new BurrowList(this);
        burrowList->gui = this;
        //connect(hwRegistry,SIGNAL(s_newMonthUpdateDone()),burrowList,SLOT(update()));
        burrowList->setup();
        centerWidget(burrowList);
        burrowList->show();
        burrowListOffset = burrowList->pos();
        burrowList->installEventFilter(this);
        console->raise();
    }
}

void GUI::closeBurrowList(){
    if (burrowListOpen){
        hideMenu();
        burrowListOpen = false;
        burrowList->deleteLater();
        enableEvents->setFocus(); //the fuck I have to do this??
    }
}

void GUI::showDeathNotice(QString wombatName, int wombatAge, int wombatCount, bool gameOver, CauseOfDeath causeOfDeath){
    closeDeathNotice();
    closeCreateBurrowDialog();
    deathNoticeOpen = true;
    deathNotice = new DeathNotice(this);
    deathNotice->gui = this;
    deathNotice->setup(wombatName,wombatAge, wombatCount, causeOfDeath);
    centerWidget(deathNotice);
    deathNotice->show();
    deathNoticeOffset = deathNotice->pos();
    deathNotice->installEventFilter(this);
    if(gameOver){
        setLocationText(QString("GAME OVER!"));
    }
}

void GUI::closeDeathNotice(){
    if (deathNoticeOpen){
        deathNoticeOpen = false;
        deathNotice->deleteLater();
        enableEvents->setFocus(); //the fuck I have to do this??
    }
}

void GUI::showCreateBurrowDialog(){
    int tileType = geTerrainNeighboursArray(viewPosition,true);
    if (tileType==Tunnel){
        createBurrowDialog = new CreateBurrow(this);
        createBurrowDialog->gui = this;
        createBurrowDialog->installEventFilter(this);
        createBurrowDialog->setup();
        centerWidget(createBurrowDialog);
        createBurrowDialog->show();
        createBurrowDialogOffset = createBurrowDialog->pos();
        createBurrowDialogOpen=true;
        lockMovement = true;
        moving = false;
        if (diggingHole){
            stopDiggingHole();
        }else if (diggingUnderground){
            stopDiggingUnderground();
        }
    }
}

void GUI::closeCreateBurrowDialog(){
    if (createBurrowDialogOpen){
        createBurrowDialogOpen = false;
        lockMovement = false;
        createBurrowDialog->deleteLater();
        enableEvents->setFocus();
    }
}

void GUI::showBurrowMenu(){  // Build menu
   if (!burrowMenuOpen){
        burrowMenu = new BurrowMenu(this);
        burrowMenu->gui = this;
        QPoint pos;
        pos.setX(windowSize.width()-burrowMenu->width()-10);
        pos.setY(windowSize.height()/4);
        burrowMenu->move(pos);
        burrowMenu->setVisible(true);
        burrowMenuOpen = true;
    }
}

void GUI::closeBurrowMenu(){
    if (burrowMenuOpen){
        burrowMenuOpen = false;
        burrowMenu->deleteLater();
        enableEvents->setFocus(); //the fuck I have to do this??
        building = false;
    }
}

void GUI::showStorageWidget(int burrowId, int storageId, QVector<bool> acceptedResources){
    if (!storageWidgetOpen){
        storageWidgetOpen = true;
        storageWidget = new StorageWidget(this);
        storageWidget->gui = this;
        storageWidget->burrowId = burrowId;
        storageWidget->storageId = storageId;
        storageWidget->acceptedResources = acceptedResources;
        centerWidget(storageWidget);
        storageWidgetOffset = storageWidget->pos();
        //storageWidget->installEventFilter(this);
        storageWidget->setup();
        storageWidgetBackground->installEventFilter(this);
        storageWidget->setVisible(true);
        controller->setStorageWidgetData(true,storageId);
    }
}

void GUI::closeStorageWidget(){
    if (storageWidgetOpen){
        storageWidgetOpen = false;
        storageWidget->deleteLater();
        enableEvents->setFocus();
        controller->setStorageWidgetData(false,None);
    }
}

void GUI::showPositionInfoBox(){
    positionInfoBox = new QTextEdit (this);
    positionInfoBox->setReadOnly(true);
    positionInfoBox->setStyleSheet("QTextEdit { border: 2px solid black; padding: 5px; color: white; background: rgb(30, 0, 0) }");
    positionInfoBox->setGeometry(windowSize.width()-131-10,10,131,131);
    positionInfoBox->show();
    positionInfoBoxOpen = true;
    updatePositionInfoBox();
    console->raise();
}

void GUI::closePositionInfoBox(){
    positionInfoBoxOpen = false;
    positionInfoBox->deleteLater();
}

void GUI::showWInventoryWidget(int wombatId){
    if(!wInventoryWidgetOpen){
        wInventoryWidgetOpen = true;
        wInventoryWidget = new WInventoryWidget(this);
        wInventoryWidget->gui = this;
        wInventoryWidget->wombatId = wombatId;
        centerWidget(wInventoryWidget);
        wInventoryWidgetOffset = wInventoryWidget->pos();
        //wInventoryWidget->installEventFilter(this);
        wInventoryWidget->setup();
        wInventoryWidgetBackground->installEventFilter(this);
        wInventoryWidget->setVisible(true);
        controller->setWInventoryWidgetData(true,wombatId);
    }
}

void GUI::closeWInventoryWidget(){
    if (wInventoryWidgetOpen){
        wInventoryWidgetOpen = false;
        wInventoryWidget->deleteLater();
        enableEvents->setFocus();
        controller->setWInventoryWidgetData(false,None);
    }
}

void GUI::showProgressWidget(int time){
    if(!progressWidgetOpen){
        progressWidgetOpen = true;
        progressWidget = new ProgressWidget(this);
        progressWidget->gui = this;
        centerWidget(progressWidget);
        progressWidget->setup(time);
        progressWidget->setVisible(true);
    }
}

void GUI::closeProgressWidget(){
    if (progressWidgetOpen){
        progressWidgetOpen = false;
        progressWidget->deleteLater();
        enableEvents->setFocus();
    }
}

void GUI::displayOutOfRangeMessage(QPoint pos){
    QLabel *outOfRange = new QLabel(this);
    outOfRange->setStyleSheet("QLabel { color: white; border: 0;}");
    outOfRange->setText("Too far away!");
    outOfRange->move(pos);
    outOfRange->setVisible(true);

    QTimer::singleShot(2000, [=] {
            outOfRange->deleteLater();
        });
}
