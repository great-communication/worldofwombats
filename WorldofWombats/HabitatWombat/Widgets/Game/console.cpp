#include "console.h"
#include "ui_console.h"
#include <QTextBlock>
#include <QtMath>
#include <QScrollBar>

Console::Console(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Console)
{
    ui->setupUi(this);       
    ui->consoleLine->installEventFilter(this);
    ui->pushButton->installEventFilter(this);
    ui->consoleLine->setFixedHeight(20);
    ui->pushButton->setFixedHeight(2);
    ui->console->setStyleSheet("QTextEdit { font-family:'MS Shell Dlg 2'; font-size: 7pt; color: white; border: 2px solid black; padding: 5px; color: white; background: rgba(0, 0, 0, 180)}");
    ui->consoleLine->setStyleSheet("QLineEdit { font-family:'MS Shell Dlg 2'; font-size: 7pt; color: white; border: 2px solid black; color: white; background: rgba(0, 0, 0, 180)}");
    ui->pushButton->setStyleSheet("QPushButton { border: 2px solid black; color: white; background: rgba(0, 0, 0, 180)}");
    ui->console->verticalScrollBar()->setStyleSheet(styleSheet());
    stopped = false;
    blockLimit = 500;
    pct = 0.2; //Default height 20% of MainWindow height
    oldCursorPos = QPoint(0,0);
    //qDebug()<<"From main thread: "<<QThread::currentThreadId();
    //qDebug()<<"width: "<<w;
}

Console::~Console()
{
    delete ui;
}

void Console::toggleConsole(){
    if (ui->console->isVisible()==false){
        this->setVisible(true);
        ui->consoleLine->setFocus();
    }else{
        this->setVisible(false);
        gui->enableEvents->setFocus();
    }
}

void Console::sizeChanged(){
    this->setGeometry(0,0,gui->width(),(gui->height()*pct));
}

void Console::dragging(){
    QPoint newCursorPos = QCursor::pos();
    QPoint p = newCursorPos-oldCursorPos;
    double temp = (double)(this->height()+p.y())/(double)gui->height();

    if(temp>0.1 && temp<0.9){
        pct = temp;
        this->setGeometry(0,0,gui->width(),(gui->height()*pct));
        oldCursorPos = newCursorPos;
    }
}

void Console::print(QString string){

    if(ui->console->document()->blockCount() > blockLimit){
        //Remove oldest block if more blocks than blockLimit
        QTextBlock block = ui->console->document()->begin();
        QTextCursor cursor(block);
        block = block.next();
        cursor.select(QTextCursor::BlockUnderCursor);
        //move forward one character to select the return-char,
        //in order to avoid the empty line after the selected text is removed.
        cursor.movePosition(QTextCursor::NextCharacter,QTextCursor::KeepAnchor);
        cursor.removeSelectedText();
    }

    ui->console->append(string);
}

void Console::on_consoleLine_returnPressed()
{
    QString input = ui->consoleLine->text();
    QStringList query = input.split(",");

    if (input == "clear") {
        ui->console->setText("");
    }else if (input == "setupWorker"){
        //controller->setupWorker();
    }else if (input == "getxymoved"){
        print("xMoved2:"+ QString::number(gui->xMoved2)+" yMoved2: "+ QString::number(gui->yMoved2));
    }else if (input == "savetextofile"){
        QFile file("yourFile.png");
        file.open(QIODevice::WriteOnly);
        openGL->grabFramebuffer().save(&file, "PNG");
    }else if (input == "getneighbours"){
        for (int i=0;i<gui->neighbours.length();i++){
            print("pos:"+ QString::number(gui->neighbours.value(i).pos.x())+","+QString::number(gui->neighbours.value(i).pos.y())+" tiletype:"+QString::number(gui->neighbours.value(i).tileType));
        }

    }else if (input == "getwombatsOnDisplaylength"){
        int i = openGL->wombatsOnDisplay.length();
        print("number of womabts:"+ QString::number(i));
    }else if (query.value(0) == "debug") {
        //print("wombatsondisplay in opengl: "+QString::number(gui->openGL->wombatsOnDisplay.length()));
        //for (int i=0;i<gui->openGL->wombatsOnDisplay.length();i++){
        //    print("wombatId: "+QString::number(gui->openGL->wombatsOnDisplay.value(i).wombatId));
        //}

        controller->toWorker();
        print("debugdone");
    }else if (query.value(0) == "debug2") {
        int x = query.value(1).toInt();
        int y = query.value(2).toInt();

        controller->toWorkerWithParameters(QPoint(x,y));
        print("debugdone");
    }else if (query.value(0) == "findpath") {
        int x = query.value(1).toInt();
        int y = query.value(2).toInt();
        PathPoint startPos;
        startPos.pos = QPoint(x,y);
        startPos.underground = query.value(3).toInt();
        x = query.value(4).toInt();
        y = query.value(5).toInt();
        PathPoint endPos;
        endPos.pos = QPoint(x,y);
        endPos.underground = query.value(6).toInt();

        controller->findPathDebug(startPos,endPos);
        print("done");
    }else if (input == "sleep") {
        openGL->wombatsOnDisplay[1].emoticon = WombatEmoticon_Exclamation;
        print("sleepdisplayed");
    }else if (query.value(0) == "getnamewombatid") {
        int wombatId = query.value(1).toInt();
        controller->getNameWombatId(wombatId);

        print("sleepdisplayed");
    }else if (query.value(0) == "printallwombatstoconsole") {
        controller->printAllWombatsToConsole();
    }else if (query.value(0) == "getpositionbufferlength") {
        int wombatId = query.value(1).toInt();
        int wombatIndex = -1;
        for (int i=0;i<gui->openGL->wombatsOnDisplay.length();i++){
            if (gui->openGL->wombatsOnDisplay.value(i).wombatId == wombatId){
                wombatIndex = i;
            }
        }

        if(wombatIndex !=1){
            print("wombat id: "+QString::number(wombatId)+" position buffer length :"+QString::number(gui->openGL->wombatsOnDisplay.value(wombatIndex).positionBuffer.length()));
        }



        print("sleepdisplayed");
    }else if (query.value(0) == "getbesttime") {
        print("longest update: "+QString::number(gui->openGL->bestFuckSoFar));
    }




/*
    if (input == "god = 1" || input == "god=1"){
        hwGameUI->god = true;
        print("You are God.");
    }else if (input == "god = 0" || input == "god=0") {
        hwGameUI->god = false;
        print("God is dead.");
    }else if (input == "clear") {
        ui->console->setText("");
    }else if (input == "terraintype = 1" || input == "terraintype=1") {
        hwGameUI->uiTileType = Dirt;
        print("TerrainType set to Dirt.");
    }else if (input == "terraintype = 2" || input == "terraintype=2") {
        hwGameUI->uiTileType = Bush;
        print("TerrainType set to Bush.");
    }else if (input == "position = 1" || input == "position=1") {
        hwGameUI->showPositionInfoBox();
        print("Show position enabled.");
    }else if (input == "position = 0" || input == "position=0") {
        hwGameUI->closePositionInfoBox();
        print("Show position disabled.");
    }else if (input == "gettiletype") {
        int terrainTypeUnderground = hwGameObject->getTileType(hwGameUI->viewPosition,true);
        print(QString::number(terrainTypeUnderground));
    }else if (input == "noise") {
        //QPoint p = QPoint(5,5);
        //hwGameObject->getNoiseData(p);
        print("done");
    }else if (input == "howmanywombats") {
           print("nr wombats "+QString::number(hwRegistry->wombats.length()));
    }else if (input == "selectedwombat") {
        int selectedWombat = hwRegistry->selectedWombat;
        print(QString::number(selectedWombat));
    }else if (input == "entrances") {
        int burrowId = hwGameObject->getBurrowId(hwGameUI->viewPosition.x(),hwGameUI->viewPosition.y());
        print("entrances: "+QString::number(hwRegistry->burrows[burrowId-1].entrances));
        for (int i=0;i< hwRegistry->burrows[burrowId-1].entrancePos.length();i++){
            QPoint p = hwRegistry->burrows[burrowId-1].entrancePos[i];
            print("pos: "+QString::number(p.x())+" "+QString::number(p.y()));
        }
    }else if (input == "burrowid") {
        int burrowId = hwGameObject->getBurrowId(hwGameUI->viewPosition.x(),hwGameUI->viewPosition.y());
        print(QString::number(burrowId));
    }else if (input == "howmanywombats") {
        int n = hwRegistry->wombats.length();
        print("wombats on display:"+QString::number(n));
    }else if (input == "howmanywombatsondisplay") {
        int n = hwHerder->wombatHerd.length();
        print("wombats on display:"+QString::number(n));
    }else if (input == "showdeathnotice") {
        hwGameUI->selectedWombatDied("fake death",10);
    }else if (input == "getselectedwombat") {
        print("dflj:"+ QString::number(hwRegistry->selectedWombat));
    }else if (input == "getselectedwombatpos") {
        int index = hwRegistry->wombatIds.indexOf(hwRegistry->selectedWombat);
        QPoint pos = hwRegistry->wombats.value(index).pos;
        print("selected wombat pos:"+ QString::number(pos.x())+","+QString::number(pos.y()));
    }else if (input == "fuckyou1") {
        print("dflj:"+ QString::number(hwRegistry->burrows[0].populationWombatIds[0]));
    }else if (input == "fuckyou2") {
        print("dflj:"+ QString::number(hwRegistry->burrows[0].populationWombatIds[0]));
        print("dflj2:"+ QString::number(hwRegistry->burrows[1].populationWombatIds[0]));
    //}else if (query.value(0) == "pos") {
        //int x = query.value(1).toInt();
        //int y = query.value(2).toInt();
    }else if (input == "burrow0pop") {
        print("pop:"+ QString::number(hwRegistry->burrows[0].population));
        print("poplength:"+ QString::number(hwRegistry->burrows[0].populationWombatIds.length()));
    }else if (input == "entireburrow0pop"){
        QVector<int> burrowWombats = hwRegistry->burrows[0].populationWombatIds;
        for (int i=0;i<hwRegistry->burrows[0].populationWombatIds.length();i++){
            int index = hwRegistry->wombatIds.indexOf(burrowWombats[i]);
            print(hwRegistry->wombats[index].name);
        }
    }else if (input == "saveburrowstotextfile") {
        hwRegistry->saveBurrowsToTextFile();
        print("save done");
    }else if (input == "savebackgroundtofile") {
        QFile file("yourFile.png");
        file.open(QIODevice::WriteOnly);
        //openGL->backgroundImage.mirrored().rgbSwapped().save(&file, "PNG");
        print("save done");
    }else if (input == "ffs") {
        hwGameUI->prepareBackgroundImage();
    }else if (query.value(0) == "pos") {
        int x = query.value(1).toInt();
        int y = query.value(2).toInt();
        print("xy"+QString::number(x)+","+QString::number(y));
        //hwHerder->addWombatToHerd(1, QPoint(1,-1), 454545, 454545, QPoint (0,0));
        openGL->addWombatToDisplay(QPoint(x,y),QPoint(64,64),0,1,0);
    }else if (query.value(0) == "showcorpse") {
        QPoint eh = QPoint(query.value(1).toInt(),query.value(2).toInt());
        hwGameUI->showCorpse(eh,0,false);
        print("showed");
    }else if (input == "check") {
        print("checked");
    }else if (input == "stoptime") {
        hwGameObject->timerNewMonth->stop();
        print("stopped");
    }else if (input == "starttime") {
        hwGameObject->timerNewMonth->start(hwGameObject->timeSpeed);
        print("started");
    }else if (input == "newyear") {
        hwGameObject->newMonth();
        hwGameObject->newMonth();
        hwGameObject->newMonth();
        hwGameObject->newMonth();
        hwGameObject->newMonth();
        hwGameObject->newMonth();
        hwGameObject->newMonth();
        hwGameObject->newMonth();
        hwGameObject->newMonth();
        hwGameObject->newMonth();
        hwGameObject->newMonth();
        hwGameObject->newMonth();
        print("new year");
    }else if (input == "wherearewombats") {
        QString pos;
        print("number of herdmembers:"+QString::number(hwHerder->wombatHerd.length()));
        for (int i=0;i<hwHerder->wombatHerd.length();i++){
            int wombatId = hwHerder->wombatHerd[i]->wombatId;
            int index = hwRegistry->wombatIds.indexOf(wombatId);
            pos = "herdpos: "+QString::number(hwHerder->wombatHerd[i]->oldTilePos.x())+","+QString::number(hwHerder->wombatHerd[i]->oldTilePos.y())+" name: "+ hwGameObject->hwRegistry->wombats.value(index).name;
            print(pos);

        }


        pos = "";
        print("number of wombatsondisplay:"+QString::number(openGL->wombatsOnDisplay.length()));
        for (int i=0;i<openGL->wombatsOnDisplay.length();i++){
            QPointF kk = openGL->wombatsOnDisplay[i].glPos;
            int age = openGL->wombatsOnDisplay[i].age;
            QPoint tilepos;
            QPoint remainingPixels;

            if (age>1){
                kk = kk/(2.0f/128.0f);


            }else if (age==1){
                kk = kk/(2.0f/104.0f);
            }else{
                kk = kk/(2.0f/80.0f);
            }

            int ix = qFloor(kk.x()/128); //Using int gives us the integer part only
            int iy = qFloor(kk.y()/128);
            double dx = kk.x()/128; //Using double gives us the full number
            double dy = kk.y()/128;
            dx = dx-ix; //  Full number - integer part gives us the remainder only.
            dy = dy-iy;
            dx = dx*128; // Remainder*128 gives us the internal pos within the tile (in pixels)
            dy = dy*128;

            tilepos = QPoint(ix,iy);
            remainingPixels = QPoint(dx,dy);

            int wombatId = hwHerder->wombatHerd[i]->wombatId;
            int index = hwRegistry->wombatIds.indexOf(wombatId);
            pos = "openGL tile offset: "+QString::number(tilepos.x())+","+QString::number(tilepos.y())+" pixeloffset: "+QString::number(remainingPixels.x())+","+QString::number(remainingPixels.y())+" name: "+ hwGameObject->hwRegistry->wombats.value(index).name;
            print(pos);
        }
    }else if (input == "allwombatnames") {
        QString name = "";
        for (int i=0;i<hwRegistry->wombats.length();i++){
            name += hwRegistry->wombats[i].name+", ";
        }
        print(name);
    }else if (input == "herdindexes") {
        QString index = "";
        for (int i=0;i<hwHerder->wombatHerd.length();i++){
            index += QString::number(hwHerder->wombatHerd[i]->hIndex) +", ";
        }
        print(index);
    }else if (input == "stopallmovement") {
        stopped=true;
        for (int i=0;i<hwHerder->wombatHerd.length();i++){
            hwHerder->wombatHerd[i]->timerMoveToTarget->stop();
            hwHerder->wombatHerd[i]->timerNewTarget->stop();
            print("all movement stopped");
        }
    }else if (input == "checkmoved") {
        print("xMoved,yMoved: "+QString::number(hwGameUI->xMoved)+","+QString::number(hwGameUI->yMoved));
        print("xMoved2,yMoved2: "+QString::number(hwGameUI->xMoved2)+","+QString::number(hwGameUI->yMoved2));
    }else if (input == "showwombat") {
        hwGameUI->wombat->setVisible(true);
    }else if (input == "hidewwombat") {
        hwGameUI->wombat->setVisible(false);
    }else if (input == "killselectedwombat"){

        QVector<int> deathList;

        int wombatIdd = hwRegistry->selectedWombat;
        int indexx = hwRegistry->getWombatIndex(wombatIdd);
        deathList.append(indexx);

        // Kill wombats
        int wombatId;
        int herdIndex;
        int index;
        QPoint pos;
        QString wombatNameTemp;
        int wombatAgeTemp;

        if (hwRegistry->wombats.length()>0){
            wombatNameTemp = hwRegistry->wombats.value(0).name;
            wombatAgeTemp = hwRegistry->wombats.value(0).age;
        }
        bool selectedWombatDied = false;
        for (int i=deathList.length()-1;i>-1;i--){ //Need to remove from back of array or indexes are not valid anymore
            index = deathList.value(i);
            //index = wombatIds.indexOf(wombatId);
            wombatId = hwRegistry->wombats.value(index).id;
            pos = hwRegistry->wombats.value(index).pos;
            herdIndex = hwHerder->wombatIds.indexOf(wombatId);
            if (herdIndex!=-1){
                hwGameUI->showCorpse(pos,hwHerder->wombatHerd[herdIndex]->rotation,hwHerder->wombatHerd[herdIndex]->underground);
                hwHerder->removeWombatFromHerd(herdIndex);
            }

            //console->print(wombats.value(index).name+" died");
            if (wombatId==hwRegistry->selectedWombat){
                selectedWombatDied = true;
                hwRegistry->selectedWombat = -1;
                wombatNameTemp = hwRegistry->wombats.value(index).name;
                wombatAgeTemp = hwRegistry->wombats.value(index).age;
            }else{
                hwGameObject->removeWombatIdFromTile(pos,wombatId);
            }
            hwRegistry->removeWombatFromRegistry(wombatId);

        }



        if (selectedWombatDied){
            hwGameUI->selectedWombatDied(wombatNameTemp,wombatAgeTemp);
            hwGameUI->showCorpse(hwGameUI->viewPosition,hwGameUI->rotation,hwGameUI->underground);
            hwGameObject->playSoundFX(Sound_Death);
        }

        if (hwRegistry->wombatIds.length()<1){ // BirthList needs to be proccessed  before this check
            hwGameUI->closeDeathNotice();
            hwGameObject->setGameOver();
            hwGameUI->selectedWombatDied(wombatNameTemp,wombatAgeTemp);
        }
    }else if (input == "savesectortofile") {
        hwGameObject->saveSectorToFile();
    }else if (input == "lengthofburrow") {
        print("length "+QString::number(hwRegistry->burrows.length()));
    }else if (input == "prepareBackgroundImage") {
        hwGameUI->prepareBackgroundImage();
        print("preparing BackgroundImage");
    }else if (input == "startupdate") {
        hwGameUI->startUpdate();
        print("update started");
    }else if (input == "getgranaries") {

    }else if (input == "howmuchfood") {
        int food = 0;
        for (int j=0;j<hwRegistry->burrows[0].storages[0].allTiles.length();j++){
            for (int k=0;k<hwRegistry->burrows[0].storages[0].allTiles[j].inventory.length();k++){
                if(hwRegistry->burrows[0].storages[0].allTiles[j].inventory[k].resource == Resource_RiceGrass){
                    food += hwRegistry->burrows[0].storages[0].allTiles[j].inventory[k].amount;
                }
            }
        }

        print("food in granary "+QString::number(food));

    }else if (input == "getworldpositionatclick") {
        QPoint pos = hwGameUI->getWorldPositionAtClick();
        print("pos "+QString::number(pos.x())+","+QString::number(pos.y()));
    }else if (query.value(0) == "addricebundle01") {
        int x = query.value(1).toInt();
        int y = query.value(2).toInt();
        TileType heh = RiceGrass_Bundle01;
        hwGameObject->addOverlayToTileUnderground(QPoint(x,y),heh,1);
        print("ricebundleadded");
    }



*/

    ui->consoleLine->setText("");

}

bool Console::eventFilter(QObject *obj, QEvent *event)
{
    // This function repeatedly call for those QObjects
    // which have installed eventFilter (Step 2)
    if (obj == (QObject*)ui->consoleLine){

        if (event->type() == QEvent::KeyPress)
        {
          QKeyEvent *KeyEvent = (QKeyEvent*)event;

            switch(KeyEvent->key())
            {
            case Qt::Key_section:
                toggleConsole();
                return true;
            case Qt::Key_Escape:
                toggleConsole();
                return true;
            default:
              break;
            }
        }
           return QWidget::eventFilter(obj, event);

    }else if (obj == (QObject*)ui->pushButton){

        if (event->type() == QEvent::MouseMove){
            dragging();
        }else if (event->type() == QEvent::MouseButtonPress){
            oldCursorPos = QCursor::pos();
        }
        return QWidget::eventFilter(obj, event);

    }else {
            // pass the event on to the parent class
            return QWidget::eventFilter(obj, event);
        }
}

bool Console::isConsoleLineSelected(){
    return ui->consoleLine->hasFocus();
}
