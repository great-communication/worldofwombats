#include "herdmember.h"
#include <QtMath>
#include "worldbuilder.h"
#include "worker.h"
#include "herder.h"
#include "registry.h"

HerdMember::HerdMember(QObject *parent) : QObject(parent){

}

void HerdMember::setup()
{
    timerNewRandomTarget = new QTimer(this);
    connect(timerNewRandomTarget, SIGNAL(timeout()), this, SLOT(newRandomTarget()));
    timerMoveToTarget = new QTimer(this);
    connect(timerMoveToTarget, SIGNAL(timeout()), this, SLOT(moveToTarget()));
    timerTEMPFIXFUCK = new QTimer(this);
    connect(timerTEMPFIXFUCK, SIGNAL(timeout()), this, SLOT(timerGOFUCKYOURSELF()));
    pxChange = QPoint(0,0);
    internalPos = currentPxOffset;
    oldTilePos = originalTilePos;
    newTilePos = originalTilePos;
    int indexx = registry->wombatIds.indexOf(wombatId);
    age = registry->wombats.value(indexx).age;
    birthMonth = registry->wombats.value(indexx).birthMonth;
    underground = registry->wombats.value(indexx).underground;
    burrowId = registry->wombats.value(indexx).burrowId;
    currentState = WombatAIState_Idle;
    currentTask = WombatAITask_Idling;
    idleCount = 0;
    idleTime = 0;    
    oldTaskString = "Idling";
    taskString = "Idling";
    myWorkShare = 0;
    workCount = 0;
    amountToEat = 0;
    noMoreRiceGrassCount = noMoreGrassCount = noMoreSticksCount = 0;
    currentPath.clear();
    tempFixFUCKCheckTask = tempFixFUCKEat = tempFixFUCKStartSleep = tempFixFUCKHarvestRiceGrass = tempFixFUCKHarvestGrass = tempFixFUCKHarvestSticks = tempFixFUCKWakeUpExclamation = false;
    movingInOpenGl = false;

    checkTask();

    //worker->consolePrint("Adding wombat to herder: "+QString::number(wombatId));
   // worker->consolePrint("Herdlength: "+QString::number(herder->wombatHerd.length()));
    worker->addWombatToDisplay(wombatId, originalTilePos,currentPxOffset,rotation,underground,age,currentTask);

    //worker->updateWombatPosition(hIndex+1,move,rotation,underground,age); is this needed? why? because of visible?
}

void HerdMember::checkTask(){
    /*if(wombatId == 3){
        int wombatIndex = registry->getWombatIndex(wombatId);
        worker->consolePrint("wombat "+QString::number(wombatId)+" "+registry->wombats.value(wombatIndex).name+" task: "+QString::number(currentTask));
    }*/

    switch(currentState){
    case WombatAIState_Idle:
        switch(currentTask){
        case WombatAITask_Idling:
            taskIdling();
            break;
        case WombatAITask_FindingSpotToSleep:
            taskFindingSpotToSleep();
            break;
        case WombatAITask_Sleeping:
            taskSleeping();
            break;
        default :
            ;
        }
        break;

    case WombatAIState_Working:
        switch(currentTask){
        case WombatAITask_FetchingRiceGrass:
            taskFetchingRiceGrass();
            break;
        case WombatAITask_FetchingGrass:
            taskFetchingGrass();
            break;
        case WombatAITask_FetchingSticks:
            taskFetchingSticks();
            break;
        case WombatAITask_Moving:
            break;
        case WombatAITask_HarvestingRiceGrass:
            taskHarvestingRiceGrass();
            break;
        case WombatAITask_HarvestingGrass:
            taskHarvestingGrass();
            break;
        case WombatAITask_HarvestingSticks:
            taskHarvestingSticks();
            break;

        case WombatAITask_ReturningRiceGrass:
            taskReturningRiceGrass();
            break;
        case WombatAITask_ReturningGrass:
            taskReturningGrass();
            break;

        case WombatAITask_ReturningSticks:
            taskReturningSticks();
            break;

        case WombatAITask_GoingToEat:
            taskGoingToEat();
            break;
        case WombatAITask_Eating:
            taskEating();
            break;
        default :
            ;
        }
        break;

    default :
        ;
    }
}

//Idle Tasks
void HerdMember::taskIdling(){
    int wombatIndex = registry->getWombatIndex(wombatId);
    if(registry->wombats.value(wombatIndex).plump<50){
        setTaskString("Looking for something to eat");
        currentState = WombatAIState_Working;
        currentTask = WombatAITask_GoingToEat;
        goToEat();
    }else{
        if(idleCount<idleTime){
            if(!currentPath.isEmpty()){
                newTargetFromPath();
            }else{
                if(!movingInOpenGl){
                    timerNewRandomTarget->start(worldBuilder->getRandomNumber(2000,8000));
                    idleCount++;
                }
            }
        }else{
            if(burrowId==None){
                if(registry->wombats.value(wombatIndex).rested < 30){
                    setTaskString("Sleeping");
                    currentTask = WombatAITask_Sleeping;
                    startSleep();
                }else{
                    continueIdle();
                }
            }else{
                if(registry->wombats.value(wombatIndex).rested < 30){
                    setTaskString("Finding somewhere to sleep");
                    currentTask = WombatAITask_FindingSpotToSleep;
                    findSpotToSleep();
                }else{
                    if(workCount>5){
                        workCount = 0;
                        continueIdle();
                    }else{
                        if(age>1){
                            int burrowIndex = registry->getBurrowIndex(burrowId);
                            Resource resource = checkIfFetchTaskIsAvailable(burrowIndex);                            

                            if(resource != Resource_None){
                                currentState = WombatAIState_Working;                                
                                if(resource == Resource_RiceGrass){
                                    currentTask = WombatAITask_FetchingRiceGrass;
                                    setTaskString("Fetching ricegrass");
                                    fetchRiceGrass(burrowIndex);
                                }else if(resource == Resource_Grass){
                                    currentTask = WombatAITask_FetchingGrass;
                                    setTaskString("Fetching grass");
                                    fetchGrass(burrowIndex);
                                }else if(resource == Resource_Sticks){
                                    currentTask = WombatAITask_FetchingSticks;
                                    setTaskString("Fetching sticks");
                                    fetchSticks(burrowIndex);
                                }
                            }else{
                                if(registry->wombats.value(wombatIndex).plump<80){
                                    setTaskString("Looking for something to eat");
                                    currentState = WombatAIState_Working;
                                    currentTask = WombatAITask_GoingToEat;
                                    goToEat();
                                }else{
                                    continueIdle();
                                }
                            }
                        }else{
                            if(registry->wombats.value(wombatIndex).plump<80){
                                setTaskString("Looking for something to eat");
                                currentState = WombatAIState_Working;
                                currentTask = WombatAITask_GoingToEat;
                                goToEat();
                            }else{
                                continueIdle(); //Wombats <2 do not work, they only eat!
                            }

                        }
                    }
                }
            }
        }
    }
}

void HerdMember::taskFindingSpotToSleep(){
    if(currentPath.isEmpty()){
        if(!movingInOpenGl){
            currentTask = WombatAITask_Sleeping;
            setTaskString("Sleeping");
            tempFixFUCKStartSleep = true;
            timerTEMPFIXFUCK->start(4000);
        }
    }else{
        newTargetFromPath();
    }
}

void HerdMember::taskSleeping(){
    int wombatIndex = registry->getWombatIndex(wombatId);
    if(registry->wombats.value(wombatIndex).rested ==100){
        currentTask = WombatAITask_Idling;
        setTaskString("Idling");
        worker->setWombatTask(hIndex+1,WombatAITask_Idling);
        worker->setWombatEmoticon(hIndex+1,WombatEmoticon_None);
        int random = worldBuilder->getRandomNumber(2000,8000);
        tempFixFUCKCheckTask = true;
        timerTEMPFIXFUCK->start(random);
    }else{
        if(registry->wombats.value(wombatIndex).rested+10 > 100){
            registry->wombats[wombatIndex].rested = 100;
        }else{
            registry->wombats[wombatIndex].rested += 10;
        }
        worker->setWombatEmoticon(hIndex+1,WombatEmoticon_Sleeping);
        tempFixFUCKCheckTask = true;
        timerTEMPFIXFUCK->start(5000);
    }
}


//Work Tasks
void HerdMember::taskFetchingRiceGrass(){
    if(currentPath.isEmpty()){
        if(!movingInOpenGl){
            int wombatIndex = registry->getWombatIndex(wombatId);
            int random = 1;
            if(registry->wombats.value(wombatIndex).plump<80){
                random = worldBuilder->getRandomNumber(1,4);
            }
            if(random==4){
                currentTask = WombatAITask_Eating;
                setTaskString("Eating");
                tempFixFUCKEat = true;
                timerTEMPFIXFUCK->start(3000);
            }else{
                currentTask = WombatAITask_HarvestingRiceGrass;
                setTaskString("Harvesting rice grass");
                tempFixFUCKHarvestRiceGrass = true;
                timerTEMPFIXFUCK->start(3000);
            }
        }
    }else{
        newTargetFromPath();
    }
}

void HerdMember::taskFetchingGrass(){
    if(currentPath.isEmpty()){
        if(!movingInOpenGl){
            currentTask = WombatAITask_HarvestingGrass;
            setTaskString("Harvesting grass");
            tempFixFUCKHarvestGrass = true;
            timerTEMPFIXFUCK->start(3000);
        }
    }else{
        newTargetFromPath();
    }
}

void HerdMember::taskFetchingSticks(){    
    if(currentPath.isEmpty()){
        if(!movingInOpenGl){
            currentTask = WombatAITask_HarvestingSticks;
            setTaskString("Harvesting sticks");
            tempFixFUCKHarvestSticks = true;
            timerTEMPFIXFUCK->start(3000);
        }
    }else{
        newTargetFromPath();
    }
}

void HerdMember::taskHarvestingRiceGrass(){
    if(myWorkShare==0 || noMoreRiceGrassCount>4){
        noMoreRiceGrassCount = 0;
        currentTask = WombatAITask_ReturningRiceGrass;
        setTaskString("Returning rice grass");
        returnToRiceGrassStorage();
    }else{
        tempFixFUCKHarvestRiceGrass = true;
        timerTEMPFIXFUCK->start(3000);
    }
}

void HerdMember::taskHarvestingGrass(){
    //worker->consolePrint("myWorkShare: "+QString::number(myWorkShare));

    if(myWorkShare==0 || noMoreRiceGrassCount>4){
        noMoreRiceGrassCount = 0;
        currentTask = WombatAITask_ReturningGrass;
        setTaskString("Returning grass");
        returnToGrassStorage();
    }else{
        tempFixFUCKHarvestGrass = true;
        timerTEMPFIXFUCK->start(3000);
    }
}

void HerdMember::taskHarvestingSticks(){
    if(myWorkShare==0 || noMoreSticksCount>4){
        noMoreSticksCount = 0;
        currentTask = WombatAITask_ReturningSticks;
        setTaskString("Returning sticks");
        returnToSticksStorage();
    }else{
        tempFixFUCKHarvestSticks = true;
        timerTEMPFIXFUCK->start(3000);
    }
}

void HerdMember::taskReturningRiceGrass(){
    if(currentPath.isEmpty()){
        if(!movingInOpenGl){
            returnRiceGrassToStorage();
            currentState = WombatAIState_Idle;
            currentTask = WombatAITask_Idling;
            setTaskString("Idling");
            workCount += 1;
            //Make wombat more tired
            int wombatIndex = registry->getWombatIndex(wombatId);
            if(wombatIndex!=None){
                registry->wombats[wombatIndex].rested -= 10;
                if (registry->wombats.value(wombatIndex).rested < 0){
                    registry->wombats[wombatIndex].rested = 0;
                }
            }

            int random = worldBuilder->getRandomNumber(100,4000);
            tempFixFUCKCheckTask = true;
            timerTEMPFIXFUCK->start(random);
        }
    }else{
        newTargetFromPath();
    }
}

void HerdMember::taskReturningGrass(){
    if(currentPath.isEmpty()){
        if(!movingInOpenGl){
            returnGrassToStorage();
            currentState = WombatAIState_Idle;
            currentTask = WombatAITask_Idling;
            setTaskString("Idling");
            workCount += 1;
            //Make wombat more tired
            int wombatIndex = registry->getWombatIndex(wombatId);
            if(wombatIndex!=None){
                registry->wombats[wombatIndex].rested -= 10;
                if (registry->wombats.value(wombatIndex).rested < 0){
                    registry->wombats[wombatIndex].rested = 0;
                }
            }

            int random = worldBuilder->getRandomNumber(100,4000);
            tempFixFUCKCheckTask = true;
            timerTEMPFIXFUCK->start(random);
        }
    }else{
        newTargetFromPath();
    }
}

void HerdMember::taskReturningSticks(){
    if(currentPath.isEmpty()){
        if(!movingInOpenGl){
            returnSticksToStorage();
            currentState = WombatAIState_Idle;
            currentTask = WombatAITask_Idling;
            setTaskString("Idling");
            workCount += 1;
            //Make wombat more tired
            int wombatIndex = registry->getWombatIndex(wombatId);
            if(wombatIndex!=None){
                registry->wombats[wombatIndex].rested -= 10;
                if (registry->wombats.value(wombatIndex).rested < 0){
                    registry->wombats[wombatIndex].rested = 0;
                }
            }

            int random = worldBuilder->getRandomNumber(100,4000);
            tempFixFUCKCheckTask = true;
            timerTEMPFIXFUCK->start(random);
        }
    }else{
        newTargetFromPath();
    }
}

void HerdMember::taskGoingToEat(){    
    if(currentPath.isEmpty()){
        if(!movingInOpenGl){
            bool hasOverlayRiceGrass = worldBuilder->hasOverlay(oldTilePos,Overlay_RiceGrass,underground);
            bool hasOverlayStorage = worldBuilder->hasOverlay(oldTilePos,Overlay_Storage,underground);
            bool storageWithRiceGrassPresent = false;
            if(hasOverlayStorage){
                int storageId = registry->getStorageId(oldTilePos,burrowId);
                int burrowIndex = registry->getBurrowIndex(burrowId);
                int storageIndex = registry->getStorageIndex(burrowIndex,storageId);
                QVector<InventoryItem> items = registry->getStorageInventory(burrowIndex,storageIndex);

                for (int j=0;j<items.length();j++){
                    if(items.value(j).resource == Resource_RiceGrass && items.value(j).amount>0){
                        storageWithRiceGrassPresent = true;
                        break;
                    }
                }
            }

            if(hasOverlayRiceGrass || storageWithRiceGrassPresent){
                currentTask = WombatAITask_Eating;
                setTaskString("Eating");
                tempFixFUCKEat = true;
                int random = worldBuilder->getRandomNumber(2000,4000);
                timerTEMPFIXFUCK->start(random);
            }else{
                currentState = WombatAIState_Idle;
                currentTask = WombatAITask_Idling;
                setTaskString("Idling");
                tempFixFUCKCheckTask = true;
                timerTEMPFIXFUCK->start(5000);
            }
        }
    }else{        
        newTargetFromPath();
    }
}

void HerdMember::taskEating(){
    if(amountToEat==0 || noMoreRiceGrassCount>4){
        noMoreRiceGrassCount = 0;
        currentState = WombatAIState_Idle;
        currentTask = WombatAITask_Idling;
        setTaskString("Idling");
        tempFixFUCKCheckTask = true;
        timerTEMPFIXFUCK->start(2000);
    }else{
        tempFixFUCKEat = true;
        timerTEMPFIXFUCK->start(3000);
    }
}

void HerdMember::timerGOFUCKYOURSELF(){
    timerTEMPFIXFUCK->stop();

    bool tempFixFUCKCheckTask2 = tempFixFUCKCheckTask;
    bool tempFixFUCKEat2 = tempFixFUCKEat;
    bool tempFixFUCKStartSleep2 = tempFixFUCKStartSleep;
    bool tempFixFUCKHarvestRiceGrass2 = tempFixFUCKHarvestRiceGrass;
    bool tempFixFUCKHarvestGrass2 = tempFixFUCKHarvestGrass;
    bool tempFixFUCKHarvestSticks2 = tempFixFUCKHarvestSticks;
    bool tempFixFUCKWakeUpExclamation2 = tempFixFUCKWakeUpExclamation;

    tempFixFUCKCheckTask = tempFixFUCKEat = tempFixFUCKStartSleep = tempFixFUCKHarvestRiceGrass = tempFixFUCKHarvestGrass = tempFixFUCKHarvestSticks = tempFixFUCKWakeUpExclamation = false;


    if(tempFixFUCKCheckTask2){
        checkTask();
    }else if(tempFixFUCKEat2){
        eat();
    }else if(tempFixFUCKStartSleep2){
        startSleep();
    }else if(tempFixFUCKHarvestRiceGrass2){
        harvestRiceGrass();
    }else if(tempFixFUCKHarvestGrass2){
        harvestGrass();
    }else if(tempFixFUCKHarvestSticks2){
        harvestSticks();
    }

    if(tempFixFUCKWakeUpExclamation2){
        worker->setWombatEmoticon(hIndex+1,WombatEmoticon_None);
    }
}

void HerdMember::eat(){
    //worker->consolePrint("eat start wombat: "+QString::number(wombatId));
    int burrowIndex = registry->getBurrowIndex(burrowId);
    int wombatIndex = registry->getWombatIndex(wombatId);
    bool noMoreRiceGrassesAtThisLocation = false;

    if(wombatIndex>=0 && wombatIndex<registry->wombats.length() && burrowIndex>=0 && burrowIndex<registry->burrows.length()){
        if(amountToEat==0){
            int plumpnessMissing = 100-registry->wombats.value(wombatIndex).plump;
            amountToEat = plumpnessMissing/8;
        }

        bool hasOverlayStorage = worldBuilder->hasOverlay(oldTilePos,Overlay_Storage,underground);

        if(underground && hasOverlayStorage){
            // Eating from storage
            int storageId = registry->getStorageId(oldTilePos,burrowId);
            int storageIndex = registry->getStorageIndex(burrowIndex,storageId);
            QVector<InventoryItem> items = registry->getStorageInventory(burrowIndex,storageIndex);
            for (int j=0;j<items.length();j++){
                if(items.value(j).resource == Resource_RiceGrass){
                    if(amountToEat>items.value(j).amount){
                        InventoryItem eaten;
                        eaten.resource = Resource_RiceGrass;
                        eaten.amount = items.value(j).amount;
                        registry->removeItemFromStorage(burrowIndex,storageIndex,eaten);
                        registry->wombats[wombatIndex].plump += 8*eaten.amount;
                        registry->wombats[wombatIndex].health += 4*eaten.amount;
                        if(registry->wombats.value(wombatIndex).health>100){
                            registry->wombats[wombatIndex].health = 100;
                        }

                    }else{
                        InventoryItem eaten;
                        eaten.resource = Resource_RiceGrass;
                        eaten.amount = amountToEat;
                        registry->removeItemFromStorage(burrowIndex,storageIndex,eaten);
                        registry->wombats[wombatIndex].plump += 8*eaten.amount;
                        registry->wombats[wombatIndex].health += 8*eaten.amount;
                        if(registry->wombats.value(wombatIndex).health>100){
                            registry->wombats[wombatIndex].health = 100;
                        }
                    }
                    break;
                }

            }
            amountToEat = 0; // Does not matter if resource was found or not, or how much was eaten, we want to choose a new task regardless, and that's why we neet to set amountToEat to 0
        }else{
            //Eating from fresh ricegrass
    //worker->consolePrint("eating from fresh rice grass");
            //Check how much ricegrass is available at this ricegrass location
            //and eat 1 ricegrass if available
            bool riceGrassFound = false;
            for (int i=0;i<registry->burrows.value(burrowIndex).riceGrasses.length();i++){
                if(oldTilePos == registry->burrows.value(burrowIndex).riceGrasses.value(i).pos){
                     //worker->consolePrint("rice grass found");
                    riceGrassFound = true;
                    if (registry->burrows.value(burrowIndex).riceGrasses.value(i).quantityAvailable>0){
                         //worker->consolePrint("rice grass available yes");
                        if(amountToEat>0){
                             //worker->consolePrint("amount to eat: "+QString::number(amountToEat));
                            amountToEat -= 1;
                            registry->wombats[wombatIndex].plump += 8;
                            registry->wombats[wombatIndex].health += 8;
                            if(registry->wombats.value(wombatIndex).health>100){
                                registry->wombats[wombatIndex].health = 100;
                            }
                            registry->burrows[burrowIndex].riceGrasses[i].quantityAvailable -= 1;
                        }
                    }

                    if ((int)registry->burrows.value(burrowIndex).riceGrasses.value(i).quantityAvailable==0){
                        noMoreRiceGrassesAtThisLocation = true;
                        noMoreRiceGrassCount += 1;
                    }
                    break;
                }
            }

            if(!riceGrassFound){
                noMoreRiceGrassesAtThisLocation = true;
                noMoreRiceGrassCount += 1;
            }

            if(noMoreRiceGrassesAtThisLocation && amountToEat>0){
                QPoint targetTile = checkSurroundingTilesForRiceGrass();
                if(targetTile!=QPoint(0,0)){ //If new tile found
                    setTargetTile(targetTile);
                    startTimerMoveToTarget();
                    return; //Early return!
                }
            }
        }

        if(amountToEat==0 || noMoreRiceGrassesAtThisLocation){
            //Done with eating
            checkTask();
        }else{
            targetMiddleOfTile();
        }


    }else{
        worker->consolePrint("Error eat");
    }

//worker->consolePrint("eat end wombat: "+QString::number(wombatId));
}

void HerdMember::goToEat(){
    //worker->consolePrint("go to eat start wombat: "+QString::number(wombatId));

    int burrowIndex = registry->getBurrowIndex(burrowId);
    int riceGrassCount = registry->burrows.value(burrowIndex).riceGrasses.length();
    int wombatIndex = registry->getWombatIndex(wombatId);

    if(riceGrassCount>0){
        bool hasOverlayRiceGrass = worldBuilder->hasOverlay(oldTilePos,Overlay_RiceGrass,underground);
        int amount = 0;
        for (int i=0;i<registry->burrows.value(burrowIndex).riceGrasses.length();i++){
            if(registry->burrows.value(burrowIndex).riceGrasses.value(i).pos==oldTilePos){
                amount = registry->burrows.value(burrowIndex).riceGrasses.value(i).quantityAvailable;
                break;
            }
        }

        if(hasOverlayRiceGrass && amount>0){    //If already standing on ricegrass
            targetMiddleOfTile();
        }else{
            bool riceGrassFound = false;
            int count = 0;

            //If starving, check storages first
            bool alreadyWentToStorage = false;
            if(registry->wombats.value(wombatIndex).plump<=0 && registry->wombats.value(wombatIndex).health<80){
                for (int i=0;i<registry->burrows.value(burrowIndex).storages.length();i++){
                    int storageIndex = registry->getStorageIndex(burrowIndex,registry->burrows.value(burrowIndex).storages.value(i).id);
                    QVector<InventoryItem> items = registry->getStorageInventory(burrowIndex,storageIndex);
                    for (int j=0;j<items.length();j++){
                        if(items.value(j).resource == Resource_RiceGrass && items.value(j).amount>0){

                            QPoint storagePos = registry->burrows.value(burrowIndex).storages.value(i).allTiles.value(0).pos;

                            PathPoint start;
                            start.pos = oldTilePos;
                            start.underground = underground;
                            PathPoint end;
                            end.pos = storagePos;
                            end.underground = true;

                            getAndSetNewPath(start,end);
                            currentPath.takeLast();
                            targetMiddleOfTile();
                            alreadyWentToStorage = true;
                            break;
                        }
                    }
                }
            }

            if(!alreadyWentToStorage){
                int random = worldBuilder->getRandomNumber(1,5);
                if(random == 1){
                    //Go to the ricegrass with the most ricegrass available
                    int riceGrassIndex = None;
                    int mostAvailable = 0;
                    for (int i=0;i<registry->burrows.value(burrowIndex).riceGrasses.length();i++){
                        if((int)registry->burrows.value(burrowIndex).riceGrasses.value(i).quantityAvailable>mostAvailable){
                            mostAvailable = (int)registry->burrows.value(burrowIndex).riceGrasses.value(i).quantityAvailable;
                            riceGrassIndex = i;
                        }
                    }

                    if(mostAvailable>0){
                        riceGrassFound = true;
                        QPoint riceGrassPos = registry->burrows.value(burrowIndex).riceGrasses.value(riceGrassIndex).pos;

                        PathPoint start;
                        start.pos = oldTilePos;
                        start.underground = underground;
                        PathPoint end;
                        end.pos = riceGrassPos;
                        end.underground = false;

                        getAndSetNewPath(start,end);
                        currentPath.takeLast();
                        targetMiddleOfTile();
                    }


                }else{
                    //Go to random ricegrass
                    while(!riceGrassFound && count<5){
                        int randomRiceGrass = worldBuilder->getRandomNumber(0,riceGrassCount-1);

                        if((int)registry->burrows.value(burrowIndex).riceGrasses.value(randomRiceGrass).quantityAvailable>0){
                            riceGrassFound = true;
                            QPoint riceGrassPos = registry->burrows.value(burrowIndex).riceGrasses.value(randomRiceGrass).pos;

                            PathPoint start;
                            start.pos = oldTilePos;
                            start.underground = underground;
                            PathPoint end;
                            end.pos = riceGrassPos;
                            end.underground = false;

                            getAndSetNewPath(start,end);
                            currentPath.takeLast();
                            targetMiddleOfTile();
                        }

                        count++;
                    }

                    //If no random ricegrass had ricegrass available, then loop through all ricegrasses and go to first one that has ricegrass
                    if(!riceGrassFound){
                        for (int i=0;i<registry->burrows.value(burrowIndex).riceGrasses.length();i++){
                            if((int)registry->burrows.value(burrowIndex).riceGrasses.value(i).quantityAvailable>0){
                                riceGrassFound = true;

                                QPoint riceGrassPos = registry->burrows.value(burrowIndex).riceGrasses.value(i).pos;

                                PathPoint start;
                                start.pos = oldTilePos;
                                start.underground = underground;
                                PathPoint end;
                                end.pos = riceGrassPos;
                                end.underground = false;

                                getAndSetNewPath(start,end);
                                currentPath.takeLast();
                                targetMiddleOfTile();
                                break;
                            }
                        }
                    }
                }

                //If all ricegrasses are empty, then look in the storages
                if(!riceGrassFound){

                    for (int i=0;i<registry->burrows.value(burrowIndex).storages.length();i++){
                        int storageIndex = registry->getStorageIndex(burrowIndex,registry->burrows.value(burrowIndex).storages.value(i).id);
                        QVector<InventoryItem> items = registry->getStorageInventory(burrowIndex,storageIndex);
                        for (int j=0;j<items.length();j++){
                            if(items.value(j).resource == Resource_RiceGrass && items.value(j).amount>0){

                                QPoint storagePos = registry->burrows.value(burrowIndex).storages.value(i).allTiles.value(0).pos;

                                PathPoint start;
                                start.pos = oldTilePos;
                                start.underground = underground;
                                PathPoint end;
                                end.pos = storagePos;
                                end.underground = true;

                                getAndSetNewPath(start,end);
                                currentPath.takeLast();
                                targetMiddleOfTile();
                                break;
                            }
                        }
                    }
                }

                //If still nothing found, start again
                if(!riceGrassFound){
                    tempFixFUCKCheckTask = true;
                    timerTEMPFIXFUCK->start(5000);
                }
            }
        }
    }else{
        tempFixFUCKCheckTask = true;
        timerTEMPFIXFUCK->start(5000);
    }

     //worker->consolePrint("go to eat end wombat: "+QString::number(wombatId));
}

void HerdMember::wakeUp(){ //Called when taunt nearby
    currentTask = WombatAITask_Idling;
    setTaskString("Idling");
    worker->setWombatTask(hIndex+1,WombatAITask_Idling);
    worker->setWombatEmoticon(hIndex+1,WombatEmoticon_Exclamation);
    tempFixFUCKWakeUpExclamation = true;
}


void HerdMember::continueIdle(){
    idleCount = 0;
    idleTime = worldBuilder->getRandomNumber(2,7);

    if(burrowId==None){
        timerNewRandomTarget->start(worldBuilder->getRandomNumber(2000,8000));
    }else{
        int burrowIndex = registry->getBurrowIndex(burrowId);
        int posIndex = worldBuilder->getRandomNumber(0,registry->burrows.value(burrowIndex).allNonEntrancePos.length()-1);
        QPoint pos = registry->burrows.value(burrowIndex).allNonEntrancePos.value(posIndex);
        PathPoint start;
        start.pos = oldTilePos;
        start.underground = underground;
        PathPoint end;
        end.pos = pos;
        end.underground = true;
        getAndSetNewPath(start,end);
        newTargetFromPath();
    }
}

Resource HerdMember::checkIfFetchTaskIsAvailable(int burrowIndex){
    bool riceGrassAvailable = false;
    bool grassAvailable = false;
    bool sticksAvailable = false;

    for (int j=0;j<registry->burrows.value(burrowIndex).riceGrasses.length();j++){
        if(registry->burrows.value(burrowIndex).riceGrasses.value(j).quantityAvailable >0){
            riceGrassAvailable = true;
            break;
        }
    }
    for (int j=0;j<registry->burrows.value(burrowIndex).grasses.length();j++){
        if(registry->burrows.value(burrowIndex).grasses.value(j).quantityAvailable>0){
            grassAvailable = true;
            break;
        }
    }
    for (int j=0;j<registry->burrows.value(burrowIndex).bushes.length();j++){
        if(registry->burrows.value(burrowIndex).bushes.value(j).quantityAvailable){
            sticksAvailable = true;
            break;
        }
    }

    for (int k=0;k<registry->burrows.value(burrowIndex).storages.length();k++){
        int storageIndex = registry->getStorageIndex(burrowIndex,registry->burrows.value(burrowIndex).storages.value(k).id);
        QVector<InventoryItem> items = registry->getStorageInventory(burrowIndex,storageIndex);
        int totalStorageSize = gStorageSize*registry->burrows.value(burrowIndex).storages.value(k).allTiles.length();
        int sizeUsed = 0;
        for (int l=0;l<items.length();l++){
            sizeUsed += worldBuilder->global->getItemSize(items.value(l));
        }

        int storageSizeLeftToUse = totalStorageSize-sizeUsed;

        if(storageSizeLeftToUse>0){
            bool riceGrassAccepted = registry->burrows.value(burrowIndex).storages.value(k).acceptedResources.value(2);
            bool grassAccepted = registry->burrows.value(burrowIndex).storages.value(k).acceptedResources.value(1);
            bool sticksAccepted = registry->burrows.value(burrowIndex).storages.value(k).acceptedResources.value(0);

            if(riceGrassAccepted){
                int riceGrassSize = registry->global->getResourceSize(Resource_RiceGrass);
                int riceGrassAmount = storageSizeLeftToUse/riceGrassSize;
                if(riceGrassAmount>0 && riceGrassAvailable){
                    storageIdFUCK = registry->burrows.value(burrowIndex).storages.value(k).id;
                    return Resource_RiceGrass;
                }
            }

            if(grassAccepted){
                int grassSize = registry->global->getResourceSize(Resource_Grass);
                int grassAmount = storageSizeLeftToUse/grassSize;
                if(grassAmount>0 && grassAvailable){
                    storageIdFUCK = registry->burrows.value(burrowIndex).storages.value(k).id;
                    return Resource_Grass;
                }
            }

            if(sticksAccepted){
                int sticksSize = registry->global->getResourceSize(Resource_Sticks);
                int sticksAmount = storageSizeLeftToUse/sticksSize;
                if(sticksAmount>0 && sticksAvailable){
                    storageIdFUCK = registry->burrows.value(burrowIndex).storages.value(k).id;
                    return Resource_Sticks;
                }
            }
        }
    }

    return Resource_None;
}

void HerdMember::fetchRiceGrass(int burrowIndex){
    //Check if already standing on riceGrass and quantity available >0
    bool hasOverlayRiceGrass = worldBuilder->hasOverlay(oldTilePos,Overlay_RiceGrass,underground);
    int amount = 0;
    for (int i=0;i<registry->burrows.value(burrowIndex).riceGrasses.length();i++){
        if(registry->burrows.value(burrowIndex).riceGrasses.value(i).pos==oldTilePos){
            amount = registry->burrows.value(burrowIndex).riceGrasses.value(i).quantityAvailable;
            break;
        }
    }

    if(!(hasOverlayRiceGrass && amount>0)){
        //If not then take random riceGrass and calculate path
        int randomRiceGrass = worldBuilder->getRandomNumber(0,registry->burrows.value(burrowIndex).riceGrasses.length()-1);
        QPoint riceGrassPos = registry->burrows.value(burrowIndex).riceGrasses.value(randomRiceGrass).pos;

        PathPoint start;
        start.pos = oldTilePos;
        start.underground = underground;
        PathPoint end;
        end.pos = riceGrassPos;
        end.underground = false;

        getAndSetNewPath(start,end);
        currentPath.takeLast();
    }

    targetMiddleOfTile();
}

void HerdMember::fetchGrass(int burrowIndex){
    //Check if already standing on grass and quantity available >0
    bool hasOverlayGrass = worldBuilder->hasOverlay(oldTilePos,Overlay_Grass,underground);
    int amount = 0;
    for (int i=0;i<registry->burrows.value(burrowIndex).grasses.length();i++){
        if(registry->burrows.value(burrowIndex).grasses.value(i).pos==oldTilePos){
            amount = registry->burrows.value(burrowIndex).grasses.value(i).quantityAvailable;
            break;
        }
    }

    if(!(hasOverlayGrass && amount>0)){
        //If not then take random grass and calculate path
        int randomGrass = worldBuilder->getRandomNumber(0,registry->burrows.value(burrowIndex).grasses.length()-1);
        QPoint grassPos = registry->burrows.value(burrowIndex).grasses.value(randomGrass).pos;

        PathPoint start;
        start.pos = oldTilePos;
        start.underground = underground;
        PathPoint end;
        end.pos = grassPos;
        end.underground = false;

        getAndSetNewPath(start,end);
        currentPath.takeLast();
    }

    targetMiddleOfTile();
}

void HerdMember::fetchSticks(int burrowIndex){
    //Take random bush and calculate path
    int randomBush = worldBuilder->getRandomNumber(0,registry->burrows.value(burrowIndex).bushes.length()-1);
    QPoint bushPos = registry->burrows.value(burrowIndex).bushes.value(randomBush).pos;
    QVector<QPoint> targetTiles;

    //Check for open terrain around bush
    for (int i=-1;i<2;i++){
        for (int j=-1;j<2;j++){
            if(!(i==0 && j==0)){
                bool hasOverlayBush = worldBuilder->hasOverlay(bushPos+QPoint(i,j),Overlay_Bush,underground);
                if(!hasOverlayBush){
                    QPoint targetTile = bushPos+QPoint(i,j);
                    targetTiles.append(targetTile);
                }
            }
        }
    }

    //Check which target tile is closest to current pos
    int shortestDistance = 2147483647;
    int indexOfShortest = None;
    for (int i=0;i<targetTiles.length();i++){
        QPoint diff = oldTilePos-targetTiles.value(i);
        int distance = diff.manhattanLength();
        if (distance<shortestDistance){
            shortestDistance = distance;
            indexOfShortest = i;
        }
    }

    if(indexOfShortest!=None){
        PathPoint start;
        start.pos = oldTilePos;
        start.underground = underground;
        PathPoint end;
        end.pos = targetTiles.value(indexOfShortest);
        end.underground = false;

        getAndSetNewPath(start,end);
        currentPath.takeLast();
    }

    targetMiddleOfTile();
}

void HerdMember::startSleep(){
    worker->setWombatTask(hIndex+1,WombatAITask_Sleeping);

    tempFixFUCKCheckTask = true;
    timerTEMPFIXFUCK->start(2000);
}

void HerdMember::findSpotToSleep(){
    QPoint pos;
    int burrowIndex = registry->getBurrowIndex(burrowId);
    if(registry->burrows.value(burrowIndex).nests.length()>0){
        int posIndex = worldBuilder->getRandomNumber(0,registry->burrows.value(burrowIndex).nests.length()-1);
        pos = registry->burrows.value(burrowIndex).nests.value(posIndex);
    }else{
        int posIndex = worldBuilder->getRandomNumber(0,registry->burrows.value(burrowIndex).allNonEntrancePos.length()-1);
        pos = registry->burrows.value(burrowIndex).allNonEntrancePos.value(posIndex);
    }

    PathPoint start;
    start.pos = oldTilePos;
    start.underground = underground;
    PathPoint end;
    end.pos = pos;
    end.underground = true;
    getAndSetNewPath(start,end);
    newTargetFromPath();
}

void HerdMember::harvestRiceGrass(){
    int burrowIndex = registry->getBurrowIndex(burrowId);
    int riceGrassSize = worldBuilder->global->getResourceSize(Resource_RiceGrass);

    if(myWorkShare==0){
        //Number of working wombats
        int wombatCount = 0;

        for (int j=0;j<registry->burrows.value(burrowIndex).populationWombatIds.length();j++){
            if(registry->burrows.value(burrowIndex).populationWombatIds.value(j) != registry->selectedWombat){
                int wombatIndex = registry->getWombatIndex(registry->burrows.value(burrowIndex).populationWombatIds.value(j));
                if(registry->wombats.value(wombatIndex).age>1){
                    if(registry->wombats.value(wombatIndex).plump>50){
                        wombatCount += 1;
                    }
                }
            }
        }

        //This wombat's share of the work available
        int workAvailable = registry->burrows.value(burrowIndex).workBuffer;
        if(wombatCount==0){
            myWorkShare = workAvailable;
        }else{
            myWorkShare = workAvailable/wombatCount;
        }

        riceGrassesLeftToTake = myWorkShare / riceGrassSize;
    }

    bool noMoreRiceGrassesAtThisLocation = false;
    bool riceGrassFound = false;

    //Check how much ricegrass is available at this ricegrass location
    //and take 1 ricegrass if available
    for (int i=0;i<registry->burrows.value(burrowIndex).riceGrasses.length();i++){
        if(oldTilePos == registry->burrows.value(burrowIndex).riceGrasses.value(i).pos){
            riceGrassFound = true;
            if ((int)registry->burrows.value(burrowIndex).riceGrasses.value(i).quantityAvailable>0){
                riceGrassesLeftToTake -= 1;
                registry->addItemToWombatInventory(wombatId,Resource_RiceGrass,1);
                registry->burrows[burrowIndex].riceGrasses[i].quantityAvailable -= 1;
            }

            if ((int)registry->burrows.value(burrowIndex).riceGrasses.value(i).quantityAvailable==0){
                noMoreRiceGrassesAtThisLocation = true;
                noMoreRiceGrassCount += 1;
            }
            break;
        }
    }

    if(!riceGrassFound){
        noMoreRiceGrassesAtThisLocation = true;
        noMoreRiceGrassCount += 1;
    }

    if(noMoreRiceGrassesAtThisLocation && riceGrassesLeftToTake>0){
        QPoint targetTile = checkSurroundingTilesForRiceGrass();
        if(targetTile!=QPoint(0,0)){ //If new tile found
            setTargetTile(targetTile);
            startTimerMoveToTarget();
            return; //Early return!
        }
    }

    if(riceGrassesLeftToTake==0){
        //Done with harvesting
        //Update workBuffer
        int workNotUsed = riceGrassesLeftToTake*riceGrassSize;
        int workUsed = myWorkShare-workNotUsed;
        registry->burrows[burrowIndex].workBuffer -= workUsed;
        myWorkShare = 0;
        checkTask();
    }else{
        targetMiddleOfTile();
    }
}

void HerdMember::harvestGrass(){
    int burrowIndex = registry->getBurrowIndex(burrowId);
    int grassSize = worldBuilder->global->getResourceSize(Resource_Grass);

    if(myWorkShare<=0){
        //Number of working wombats
        int wombatCount = 0;

        for (int j=0;j<registry->burrows.value(burrowIndex).populationWombatIds.length();j++){
            if(registry->burrows.value(burrowIndex).populationWombatIds.value(j) != registry->selectedWombat){
                int wombatIndex = registry->getWombatIndex(registry->burrows.value(burrowIndex).populationWombatIds.value(j));
                if(registry->wombats.value(wombatIndex).age>1){
                    if(registry->wombats.value(wombatIndex).plump>50){
                        wombatCount += 1;
                    }
                }
            }
        }

        //This wombat's share of the work available
        int workAvailable = registry->burrows.value(burrowIndex).workBuffer;
        if(wombatCount==0){
            myWorkShare = workAvailable;
        }else{
            myWorkShare = workAvailable/wombatCount;
        }

        grassesLeftToTake = myWorkShare / grassSize;
    }

    if(myWorkShare>0){ // Temp fix for some strange bug...

        bool noMoreGrassesAtThisLocation = false;
        bool grassFound = false;

        //Check how much ricegrass is available at this ricegrass location
        //and take 1 ricegrass if available
        for (int i=0;i<registry->burrows.value(burrowIndex).grasses.length();i++){
            if(oldTilePos == registry->burrows.value(burrowIndex).grasses.value(i).pos){
                grassFound = true;
                if ((int)registry->burrows.value(burrowIndex).grasses.value(i).quantityAvailable>0){
                    grassesLeftToTake -= 1;
                    registry->addItemToWombatInventory(wombatId,Resource_Grass,1);
                    registry->burrows[burrowIndex].grasses[i].quantityAvailable -= 1;
                }

                if ((int)registry->burrows.value(burrowIndex).grasses.value(i).quantityAvailable==0){
                    noMoreGrassesAtThisLocation = true;
                    noMoreGrassCount += 1;
                }
                break;
            }
        }

        if(!grassFound){
            noMoreGrassesAtThisLocation = true;
            noMoreGrassCount += 1;
        }

        if(noMoreGrassesAtThisLocation && grassesLeftToTake>0){
            QPoint targetTile = checkSurroundingTilesForRiceGrass();
            if(targetTile!=QPoint(0,0)){ //If new tile found
                setTargetTile(targetTile);
                startTimerMoveToTarget();
                return; //Early return!
            }
        }

    }else{
        grassesLeftToTake = 0;
        myWorkShare = 0;
    }

    if(grassesLeftToTake==0){
        //Done with harvesting
        //Update workBuffer
        int workNotUsed = grassesLeftToTake*grassSize;
        int workUsed = myWorkShare-workNotUsed;
        registry->burrows[burrowIndex].workBuffer -= workUsed;
        myWorkShare = 0;
        checkTask();
    }else{
        targetMiddleOfTile();
    }
}

void HerdMember::harvestSticks(){    
    int burrowIndex = registry->getBurrowIndex(burrowId);
    int sticksSize = worldBuilder->global->getResourceSize(Resource_Sticks);

    if(myWorkShare==0){

        //Number of working wombats
        int wombatCount = 0;

        for (int j=0;j<registry->burrows.value(burrowIndex).populationWombatIds.length();j++){
            if(registry->burrows.value(burrowIndex).populationWombatIds.value(j) != registry->selectedWombat){
                int wombatIndex = registry->getWombatIndex(registry->burrows.value(burrowIndex).populationWombatIds.value(j));
                if(registry->wombats.value(wombatIndex).age>1){
                    if(registry->wombats.value(wombatIndex).plump>50){
                        wombatCount += 1;
                    }
                }
            }
        }

        //This wombat's share of the work available
        int workAvailable = registry->burrows.value(burrowIndex).workBuffer;
        if(wombatCount==0){
            myWorkShare = workAvailable;
        }else{
            myWorkShare = workAvailable/wombatCount;
        }

        sticksLeftToTake = myWorkShare / sticksSize;

        QPoint tileOffset = QPoint(0,0);
        bool done = false;
        //Check for bush with sticks available around this location
        for (int i=-1;i<2 && !done;i++){
            for (int j=-1;j<2 && !done;j++){
                if(!(i==0 && j==0)){
                    bool hasOverlayBush = worldBuilder->hasOverlay(oldTilePos+QPoint(i,j),Overlay_Bush,underground);
                    if(hasOverlayBush){
                        for (int k=0;k<registry->burrows.value(burrowIndex).bushes.length() && !done;k++){
                            if(oldTilePos+QPoint(i,j) == registry->burrows.value(burrowIndex).bushes.value(k).pos){
                                if(registry->burrows.value(burrowIndex).bushes.value(k).quantityAvailable>0){
                                    tileOffset = QPoint(i,j);
                                    done = true;
                                }
                            }
                        }
                    }
                }
            }
        }
        sticksLocationFUCK = oldTilePos + tileOffset;
        targetEdgeOfTile();
    }else{

        //worker->consolePrint("sticks location: "+QString::number(sticksLocationFUCK.x())+","+QString::number(sticksLocationFUCK.y()));

        bool noMoreSticksAtThisLocation = false;
        bool sticksFound = false;

        if(sticksLocationFUCK == QPoint(0,0)){
            myWorkShare = 0; //Abort
            checkTask();
        }else{
            //Check how many sticks are available at this stick location
            //and take 1 stick if available
            for (int i=0;i<registry->burrows.value(burrowIndex).bushes.length();i++){
                if(sticksLocationFUCK == registry->burrows.value(burrowIndex).bushes.value(i).pos){
                    sticksFound = true;
                    if ((int)registry->burrows.value(burrowIndex).bushes.value(i).quantityAvailable>0){
                        sticksLeftToTake -= 1;
                        registry->addItemToWombatInventory(wombatId,Resource_Sticks,1);
                        registry->burrows[burrowIndex].bushes[i].quantityAvailable -= 1;
                    }

                    if ((int)registry->burrows.value(burrowIndex).bushes.value(i).quantityAvailable==0){
                        noMoreSticksAtThisLocation = true;
                        noMoreSticksCount += 1;
                    }
                    break;
                }
            }

            if(!sticksFound){
                noMoreSticksAtThisLocation = true;
                noMoreSticksCount += 1;
            }

            if(noMoreSticksAtThisLocation && sticksLeftToTake>0){
                QPoint targetTile = checkSurroundingTilesForSticks();
                if(targetTile!=QPoint(0,0)){ //If new tile found
                    targetEdgeOfTile();
                    return; //Early return!
                }
            }

            if(sticksLeftToTake==0){
                //Done with harvesting
                //Update workBuffer
                int workNotUsed = sticksLeftToTake*sticksSize;
                int workUsed = myWorkShare-workNotUsed;
                registry->burrows[burrowIndex].workBuffer -= workUsed;
                myWorkShare = 0;
                checkTask();
            }else{
                targetEdgeOfTile();
            }
        }
    }
}

QPoint HerdMember::checkSurroundingTilesForRiceGrass(){
    for (int i=-1;i<2;i++){
        for (int j=-1;j<2;j++){
            if(!(i==0 && j==0)){
                QPoint pos = oldTilePos+QPoint(i,j);
                if(worldBuilder->hasOverlay(pos,Overlay_RiceGrass,false)){
                    int ownerId = worldBuilder->getOwner(pos.x(),pos.y());
                    if(ownerId == burrowId){
                        int burrowIndex = registry->getBurrowIndex(burrowId);
                        for (int k=0;k<registry->burrows.value(burrowIndex).riceGrasses.length();k++){
                            if(pos == registry->burrows.value(burrowIndex).riceGrasses.value(k).pos){
                                if((int)registry->burrows.value(burrowIndex).riceGrasses.value(k).quantityAvailable>0){
                                    return pos;
                                }
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
    return QPoint(0,0);
}

QPoint HerdMember::checkSurroundingTilesForGrass(){
    for (int i=-1;i<2;i++){
        for (int j=-1;j<2;j++){
            if(!(i==0 && j==0)){
                QPoint pos = oldTilePos+QPoint(i,j);
                if(worldBuilder->hasOverlay(pos,Overlay_Grass,false)){
                    int ownerId = worldBuilder->getOwner(pos.x(),pos.y());
                    if(ownerId == burrowId){
                        int burrowIndex = registry->getBurrowIndex(burrowId);
                        for (int k=0;k<registry->burrows.value(burrowIndex).grasses.length();k++){
                            if(pos == registry->burrows.value(burrowIndex).grasses.value(k).pos){
                                if((int)registry->burrows.value(burrowIndex).grasses.value(k).quantityAvailable>0){
                                    return pos;
                                }
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
    return QPoint(0,0);
}

QPoint HerdMember::checkSurroundingTilesForSticks(){
    for (int i=-1;i<2;i++){
        for (int j=-1;j<2;j++){
            if(!(i==0 && j==0)){
                QPoint pos = oldTilePos+QPoint(i,j);
                if(worldBuilder->hasOverlay(pos,Overlay_Bush,false)){
                    int ownerId = worldBuilder->getOwner(pos.x(),pos.y());
                    if(ownerId == burrowId){
                        int burrowIndex = registry->getBurrowIndex(burrowId);
                        for (int k=0;k<registry->burrows.value(burrowIndex).bushes.length();k++){
                            if(pos == registry->burrows.value(burrowIndex).bushes.value(k).pos){
                                if((int)registry->burrows.value(burrowIndex).bushes.value(k).quantityAvailable>0){
                                    sticksLocationFUCK = pos;
                                    return pos;
                                }
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
    return QPoint(0,0);
}

void HerdMember::returnToRiceGrassStorage(){


    //Find closest Storage and StorageTile
    int burrowIndex = registry->getBurrowIndex(burrowId);
    int shortestDistance = 2147483647;
    int iIndex = 0;
    int jIndex = 0;
    //for (int i=0;i<registry->burrows.value(burrowIndex).storages.length();i++){
    //    if(registry->burrows.value(burrowIndex).storages.value(i).acceptedResources.value(2)){
    int storageIndex = registry->getStorageIndex(burrowIndex,storageIdFUCK);
            for (int j=0;j<registry->burrows.value(burrowIndex).storages.value(storageIndex).allTiles.length();j++){
                storagePos = registry->burrows.value(burrowIndex).storages.value(storageIndex).allTiles.value(j).pos;
                QPoint diff = oldTilePos-storagePos;
                int distance = diff.manhattanLength();
                if (distance<shortestDistance){
                    shortestDistance = distance;
                    iIndex = storageIndex;
                    jIndex = j;
                }
            }
        //}
    //}

    storagePos = registry->burrows.value(burrowIndex).storages.value(iIndex).allTiles.value(jIndex).pos;



    PathPoint start;
    start.pos = oldTilePos;
    //worker->consolePrint("oldTilePos: "+QString::number(oldTilePos.x())+","+QString::number(oldTilePos.y()));
    start.underground = underground;
    PathPoint end;
    end.pos = storagePos;
    end.underground = true;

    getAndSetNewPath(start,end);
    //currentPath.takeLast();
    newTargetFromPath();
}

void HerdMember::returnToGrassStorage(){
    //Find closest Storage and StorageTile
    int burrowIndex = registry->getBurrowIndex(burrowId);
    int shortestDistance = 2147483647;
    int iIndex = 0;
    int jIndex = 0;
    //for (int i=0;i<registry->burrows.value(burrowIndex).storages.length();i++){
    //    if(registry->burrows.value(burrowIndex).storages.value(i).acceptedResources.value(1)){
    int storageIndex = registry->getStorageIndex(burrowIndex,storageIdFUCK);
            for (int j=0;j<registry->burrows.value(burrowIndex).storages.value(storageIndex).allTiles.length();j++){
                storagePos = registry->burrows.value(burrowIndex).storages.value(storageIndex).allTiles.value(j).pos;
                QPoint diff = oldTilePos-storagePos;
                int distance = diff.manhattanLength();
                if (distance<shortestDistance){
                    shortestDistance = distance;
                    iIndex = storageIndex;
                    jIndex = j;
                }
            }
        //}
    //}

    storagePos = registry->burrows.value(burrowIndex).storages.value(iIndex).allTiles.value(jIndex).pos;

    PathPoint start;
    start.pos = oldTilePos;
    //worker->consolePrint("oldTilePos: "+QString::number(oldTilePos.x())+","+QString::number(oldTilePos.y()));
    start.underground = underground;
    PathPoint end;
    end.pos = storagePos;
    end.underground = true;

    getAndSetNewPath(start,end);
    //currentPath.takeLast();
    newTargetFromPath();
}

void HerdMember::returnToSticksStorage(){
    //Find closest Storage and StorageTile
    int burrowIndex = registry->getBurrowIndex(burrowId);
    int shortestDistance = 2147483647;
    int iIndex = 0;
    int jIndex = 0;
    //for (int i=0;i<registry->burrows.value(burrowIndex).storages.length();i++){
    //    if(registry->burrows.value(burrowIndex).storages.value(i).acceptedResources.value(1)){
    int storageIndex = registry->getStorageIndex(burrowIndex,storageIdFUCK);
            for (int j=0;j<registry->burrows.value(burrowIndex).storages.value(storageIndex).allTiles.length();j++){
                storagePos = registry->burrows.value(burrowIndex).storages.value(storageIndex).allTiles.value(j).pos;
                QPoint diff = oldTilePos-storagePos;
                int distance = diff.manhattanLength();
                if (distance<shortestDistance){
                    shortestDistance = distance;
                    iIndex = storageIndex;
                    jIndex = j;
                }
            }
        //}
    //}

    storagePos = registry->burrows.value(burrowIndex).storages.value(iIndex).allTiles.value(jIndex).pos;

    PathPoint start;
    start.pos = oldTilePos;
    //worker->consolePrint("oldTilePos: "+QString::number(oldTilePos.x())+","+QString::number(oldTilePos.y()));
    start.underground = underground;
    PathPoint end;
    end.pos = storagePos;
    end.underground = true;

    getAndSetNewPath(start,end);
    //currentPath.takeLast();
    newTargetFromPath();
}

void HerdMember::returnRiceGrassToStorage(){
    int burrowIndex = registry->getBurrowIndex(burrowId);
    int storageId = registry->getStorageId(oldTilePos,burrowId);
    int storageIndex = registry->getStorageIndex(burrowIndex,storageId);
    if(registry->burrows.value(burrowIndex).storages.value(storageIndex).acceptedResources.value(2)){
        InventoryItem item;
        item.resource = Resource_RiceGrass;
        item.amount = 0;
        item = registry->getItemFromWombatInventory(wombatId,item);
        registry->addItemToStorage(burrowIndex,storageIndex,item);
    }
}

void HerdMember::returnGrassToStorage(){
    int burrowIndex = registry->getBurrowIndex(burrowId);
    int storageId = registry->getStorageId(oldTilePos,burrowId);
    int storageIndex = registry->getStorageIndex(burrowIndex,storageId);
    if(registry->burrows.value(burrowIndex).storages.value(storageIndex).acceptedResources.value(1)){
        //worker->consolePrint("no fuck");
        InventoryItem item;
        item.resource = Resource_Grass;
        item.amount = 0;
        item = registry->getItemFromWombatInventory(wombatId,item);
        registry->addItemToStorage(burrowIndex,storageIndex,item);
    }else{
        //worker->consolePrint("fuck");
    }
}

void HerdMember::returnSticksToStorage(){
    int burrowIndex = registry->getBurrowIndex(burrowId);
    int storageId = registry->getStorageId(oldTilePos,burrowId);
    int storageIndex = registry->getStorageIndex(burrowIndex,storageId);
    if(registry->burrows.value(burrowIndex).storages.value(storageIndex).acceptedResources.value(0)){
        InventoryItem item;
        item.resource = Resource_Sticks;
        item.amount = 0;
        item = registry->getItemFromWombatInventory(wombatId,item);
        registry->addItemToStorage(burrowIndex,storageIndex,item);
    }else{

    }
}


void HerdMember::setTargetTile(QPoint pos){
    currentPxOffset = internalPos;
    oldPosPx = currentPxOffset;
    QPoint offset = pos-oldTilePos;
    int x = offset.x()*gTileSize;
    int y = offset.y()*gTileSize;
    targetPosPx = QPoint(x,y)+oldPosPx;

    if (targetPosPx.x() > oldPosPx.x()){
        moveXDirection = 1;
    }else{
        moveXDirection = -1;
    }
    if (targetPosPx.y() > oldPosPx.y()){
        moveYDirection = 1;
    }else{
        moveYDirection = -1;
    }
}

void HerdMember::targetMiddleOfTile(){    
        currentPxOffset = internalPos;
        oldPosPx = currentPxOffset;
        int x = worldBuilder->getRandomNumber(-32,+32);
        int y = worldBuilder->getRandomNumber(-32,+32);
        targetPosPx = QPoint(64,64)-QPoint(x,y);
        //worker->consolePrint("intenralpos: "+QString::number(currentPxOffset.x())+","+QString::number(currentPxOffset.y()));
        //worker->consolePrint("current px offset: "+QString::number(internalPos.x())+","+QString::number(internalPos.y()));
        //worker->consolePrint("targetpos in px: "+QString::number(targetPosPx.x())+","+QString::number(targetPosPx.y()));

        if (targetPosPx.x() > oldPosPx.x()){
            moveXDirection = 1;
        }else{
            moveXDirection = -1;
        }
        if (targetPosPx.y() > oldPosPx.y()){
            moveYDirection = 1;
        }else{
            moveYDirection = -1;
        }

        startTimerMoveToTarget();
}

void HerdMember::targetEdgeOfTile(){
        currentPxOffset = internalPos;
        oldPosPx = currentPxOffset;

        QPoint hm = sticksLocationFUCK - oldTilePos;

        int x = worldBuilder->getRandomNumber(32,96);
        int y = worldBuilder->getRandomNumber(32,96);
        if(hm.x()>0){
            x=126;
        }else{
            x=1;
        }

        if(hm.y()>0){
            y=126;
        }else{
            y=1;
        }

        targetPosPx = QPoint(x,y);
        //worker->consolePrint("targetpos in px: "+QString::number(targetPosPx.x())+","+QString::number(targetPosPx.y()));

        if (targetPosPx.x() > oldPosPx.x()){
            moveXDirection = 1;
        }else{
            moveXDirection = -1;
        }
        if (targetPosPx.y() > oldPosPx.y()){
            moveYDirection = 1;
        }else{
            moveYDirection = -1;
        }

        startTimerMoveToTarget();
}

void HerdMember::startTimerMoveToTarget(){
    if(!timerMoveToTarget->isActive()){
        timerMoveToTarget->start(gFrameRate);
    }
}

void HerdMember::getAndSetNewPath(PathPoint start, PathPoint end){
    //worker->consolePrint(QString::number(wombatId)+ " requesting new Path. Curent path length is: "+QString::number(currentPath.length()));
    currentPath = registry->findPath(start,end);
    //worker->consolePrint(QString::number(wombatId)+ " got new Path. Curent path length is: "+QString::number(currentPath.length()));
}



void HerdMember::newTargetFromPath(){
    //worker->consolePrint("newTargetFromPath currentPxOffset: "+QString::number(currentPxOffset.x())+","+QString::number(currentPxOffset.y()));
    //worker->consolePrint("newTargetFromPath internalpos: "+QString::number(internalPos.x())+","+QString::number(internalPos.y()));

    if(!currentPath.isEmpty()){     // remove?
        currentPxOffset = internalPos;
        //worker->consolePrint("newtargetfrompath currentPxOffset: "+QString::number(currentPxOffset.x())+","+QString::number(currentPxOffset.y()));
        oldPosPx = currentPxOffset;
        PathPoint nextTile = currentPath.takeLast();
        QPoint offset = nextTile.pos-oldTilePos;
        //worker->consolePrint("oldTilePos: "+QString::number(oldTilePos.x())+","+QString::number(oldTilePos.y()));
        //worker->consolePrint("targettile: "+QString::number(nextTile.pos.x())+","+QString::number(nextTile.pos.y()));
        int x = offset.x()*gTileSize;
        int y = offset.y()*gTileSize;
        //worker->consolePrint("xy: "+QString::number(x)+","+QString::number(y));
        /*int rand = worldBuilder->getRandomNumber(-32,+32);
        QPoint randomOffset = QPoint(rand,rand);
        QPoint newPos = oldPosPx+randomOffset;
        if(newPos.x()>127 || newPos.x()< 0 || newPos.y()>127 || newPos.y()< 0){
            randomOffset = QPoint(0,0);
        }*/

        //worker->consolePrint("oldtile: "+QString::number(oldTilePos.x())+","+QString::number(oldTilePos.y()));
        targetPosPx = QPoint(x,y)+oldPosPx;
        //worker->consolePrint("targetpos in px: "+QString::number(targetPosPx.x())+","+QString::number(targetPosPx.y()));
        //targetPosPx = QPoint(x,y)+oldPosPx+randomOffset;
       // worker->consolePrint("targetpos in px+rand: "+QString::number(targetPosPx.x())+","+QString::number(targetPosPx.y()));

        if(nextTile.underground != underground){
            underground = nextTile.underground;
            registry->setWombatUnderground(wombatId,underground);
        }


        if (targetPosPx.x() > oldPosPx.x()){
            moveXDirection = 1;
        }else{
            moveXDirection = -1;
        }
        if (targetPosPx.y() > oldPosPx.y()){
            moveYDirection = 1;
        }else{
            moveYDirection = -1;
        }

        startTimerMoveToTarget();
    }else{ // remove?
        if (currentPxOffset==targetPosPx){
            checkTask();
        }
    }
}

void HerdMember::newRandomTarget(){
    currentPxOffset = internalPos;
    oldPosPx = currentPxOffset;
    int x = worldBuilder->getRandomNumber(-64,+64);
    int y = worldBuilder->getRandomNumber(-64,+64);
    targetPosPx = QPoint(x,y)+oldPosPx;

    QPoint newTilePos = oldTilePos;

    if(targetPosPx.x()<0){
        newTilePos = oldTilePos+QPoint(-1,0);
    }else if(targetPosPx.x()>127){
        newTilePos = oldTilePos+QPoint(1,0);
    }

    if(targetPosPx.y()<0){
        newTilePos = oldTilePos+QPoint(0,-1);
    }else if(targetPosPx.y()>127){
        newTilePos = oldTilePos+QPoint(0,1);
    }

    //worker->consolePrint("oldTile: "+QString::number(oldTilePos.x())+","+QString::number(oldTilePos.y()));
    //worker->consolePrint("newTiled: "+QString::number(newTilePos.x())+","+QString::number(newTilePos.y()));

    if (oldTilePos!=newTilePos){
        bool isUnDug = worldBuilder->hasTerrain(Terrain_UnDug,underground,newTilePos);
        bool hasBushOverlay = worldBuilder->hasOverlay(newTilePos,Overlay_Bush,underground);
        //worker->consolePrint("terrainType: "+QString::number(terrainType)+" pos: "+QString::number(newTilePos.x())+","+QString::number(newTilePos.y()));
        if (hasBushOverlay || isUnDug){
            //worker->consolePrint("wrong terrain");
            return; //Wrong terrain
        }else{
            QVector<int> tileWombatIds;
            tileWombatIds = worldBuilder->getTileWombatIds(newTilePos);
            if (tileWombatIds.value(0)!=None){
                //worker->consolePrint("tile occupied");
                return; //Tile occupied
            }
        }
    }

    if (x > 0){
        moveXDirection = 1;
    }else{
        moveXDirection = -1;
    }
    if (y > 0){
        moveYDirection = 1;
    }else{
        moveYDirection = -1;
    }

    if (underground){ //Check that the underground move constraints are not broken
        QPointF targetInternalPos = getInternalPosPx(targetPosPx);
        bool isUnDug = worldBuilder->hasTerrain(Terrain_UnDug,underground,oldTilePos+QPoint(moveXDirection,0)); //IS there a bug here!?
        if (isUnDug){
            if (moveXDirection == 1){
                if (targetInternalPos.x()>90){ //These are the underground move constraints
                    return; //Breaking the constraint
                }
            }else{
                if (targetInternalPos.x()<38){
                    return;
                }
            }

        }

        isUnDug = worldBuilder->hasTerrain(Terrain_UnDug,underground,oldTilePos+QPoint(0,moveYDirection)); //IS there a bug here!?
        if (isUnDug){
            if (moveYDirection == 1){
                if (targetInternalPos.y()>90){
                    return;
                }
            }else{
                if (targetInternalPos.y()<38){
                    return;
                }
            }
        }
    }
    timerNewRandomTarget->stop();
    startTimerMoveToTarget();
}

void HerdMember::moveToTarget(){    
    pxChange = QPoint(0,0);
    left = up = down = right = false; //Is used in rotateWombat()

    if (currentPxOffset.x()!=targetPosPx.x()){
       currentPxOffset.setX(currentPxOffset.x()+(1*moveXDirection));
       pxChange += QPoint(1*moveXDirection,0);
       if (moveXDirection>0){
           right = true;
       }else{
            left = true;
       }
    }
    if (currentPxOffset.y()!=targetPosPx.y()){
       currentPxOffset.setY(currentPxOffset.y()+(1*moveYDirection));
       pxChange += QPoint(0,1*moveYDirection);
       if (moveYDirection>0){
           down = true;
       }else{
            up = true;
       }
    }


    internalPos += pxChange;
    bool newTile = false;
    if (internalPos.x()<0) {
        internalPos.rx() += gTileSize;
        oldTilePos.rx() -= 1;
        newTile = true;
    }else if (internalPos.x()>127) {
        internalPos.rx() -= gTileSize;
        oldTilePos.rx() += 1;
        newTile = true;
    }

    if (internalPos.y()<0) {
        internalPos.ry() += gTileSize;
        oldTilePos.ry() -= 1;
        newTile = true;
    }else if (internalPos.y()>127) {
        internalPos.ry() -= gTileSize;
        oldTilePos.ry() += 1;
        newTile = true;
    }

    if (currentPxOffset==targetPosPx){
        timerMoveToTarget->stop();
    }

    rotateWombat();
    worker->addPosToWombatPosBuffer(wombatId,pxChange,rotation,underground,age); // Send info to the openGl widget in main thread

    if(newTile){        
        herder->registry->changeWombatPos(wombatId,oldTilePos);
    }

//worker->consolePrint("internal posFUCK: "+QString::number(internalPos.x())+","+QString::number(internalPos.y()));

    if (currentPxOffset==targetPosPx){        
        //worker->consolePrint("currentpx offset: "+QString::number(currentPxOffset.x())+","+QString::number(currentPxOffset.y()));
        //worker->consolePrint("internal pos: "+QString::number(internalPos.x())+","+QString::number(internalPos.y()));
        checkTask();
    }



}

QPoint HerdMember::getWorldPosFromPxOffsetPos(QPoint posPx){
    double dx = posPx.x();
    double dy = posPx.y();
    int x = qFloor(dx/gTileSize);
    int y = qFloor(dy/gTileSize);
    QPoint pos = QPoint (x,y);
    QPoint worldPos = pos+originalTilePos;    
    return worldPos;
}

QPointF HerdMember::getInternalPosPx(QPointF pos){
    double xTarget = (int)pos.x();
    double yTarget = (int)pos.y();   
    int ix = qFloor(xTarget/gTileSize); //Using int gives us the integer part only
    int iy = qFloor(yTarget/gTileSize);
    double dx = xTarget/gTileSize; //Using double gives us the full number
    double dy = yTarget/gTileSize;
    dx = dx-ix; //  Full number - integer part gives us the remainder only.
    dy = dy-iy;
    dx = dx*gTileSize; // Remainder*gTileSize gives us the internal pos within the tile (in pixels)
    dy = dy*gTileSize;
    QPoint p = QPoint((int)dx,(int)dy);
    return p;
}



void HerdMember::rotateWombat(){
    if(up&&left){
        rotation = 135;
    }else if(up&&right){
        rotation = 225;
    }else if(down&&left){
        rotation = 45;
    }else if(down&&right){
        rotation = 315;
    }else if (up){
        rotation = 180;
    }else if(left){
        rotation = 90;
    }else if(down){
        rotation = 0;
    }else if(right){
        rotation = 270;
    }    
}

void HerdMember::goBackToOldTarget(){
    int x = oldPosPx.x()-targetPosPx.x();
    int y = oldPosPx.y()-targetPosPx.y();

    targetPosPx = oldPosPx;

    if (x > 0){
        moveXDirection = 1;
    }else{
        moveXDirection = -1;
    }
    if (y > 0){
        moveYDirection = 1;
    }else{
        moveYDirection = -1;
    }
    timerNewRandomTarget->stop();
    startTimerMoveToTarget();
}

void HerdMember::growWombat(){
    //This is used for making the small joey wombats grow into full-sized wombats
    if (birthMonth==worldBuilder->month){
        age += 1;

        //worker->updateWombatSize(hIndex+1,age);

        if (age == 2){
            disconnect(worldBuilder,SIGNAL(s_newMonth()),this,SLOT(growWombat()));
        }
    }
}

void HerdMember::setTaskString(QString s){
    oldTaskString = taskString;
    taskString = s;

    if(s == "Idling" && age<2){        // Temp fix move to somewhere else... right?
        int random = worldBuilder->getRandomNumber(1,2);
        if(random==2){
            taskString = "Playing";
        }
    }


    if(registry->guiInfoBoxOpen){
        if(registry->wombatIdGuiInfoBox == wombatId){
            registry->prepareWombatInfoForDisplay(wombatId);
        }
    }
}

void HerdMember::setMovingInOpenGl(bool b){ // Need this because wombats in opengl are moved with a delay to avoid lag... and then the herdmember is not in sync with opengl anymore
    if(b){
        oldTaskString = taskString;
        movingInOpenGl = true;
    }else{
        movingInOpenGl = false;
        if(!timerMoveToTarget->isActive()){
            checkTask();
        }
    }
}
