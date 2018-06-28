#include "burrowlist.h"
#include "ui_burrowlist.h"
#include <QScrollBar>
#include <QKeyEvent>
#include "../../controller.h"

BurrowList::BurrowList(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BurrowList)
{
    ui->setupUi(this);    

    ui->burrowListBackground->setStyleSheet("QWidget { border: 2px solid black; padding: 5px; background: rgb(30, 0, 0) }");
    ui->burrowListHeadline->setStyleSheet("QLabel { border: 0; color: white; background: rgb(30, 0, 0)}");
    ui->burrowListLabelName->setStyleSheet("QLabel { border: 0; color: white; background: rgb(30, 0, 0)}");
    ui->burrowListLabelPop->setStyleSheet("QLabel { border: 0; color: white; background: rgb(30, 0, 0)}");
    ui->burrowListLabelFood->setStyleSheet("QLabel { border: 0; color: white; background: rgb(30, 0, 0)}");
    ui->burrowListLabelSize->setStyleSheet("QLabel { border: 0; color: white; background: rgb(30, 0, 0)}");
    ui->burrowListNameList->setStyleSheet("QTextEdit { border: 0; color: white; background: rgb(30, 0, 0)}");
    ui->burrowListPopList->setStyleSheet("QTextEdit { border: 0; color: white; background: rgb(30, 0, 0)}");
    ui->burrowListFoodList->setStyleSheet("QTextEdit { border: 0; color: white; background: rgb(30, 0, 0)}");
    ui->burrowListSizeList->setStyleSheet("QTextEdit { border: 0; color: white; background: rgb(30, 0, 0)}");
    ui->burrowTotalPopLabel->setStyleSheet("QLabel { border: 0; color: white; background: rgb(30, 0, 0)}");
    ui->burrowTotalPopNumber->setStyleSheet("QLabel { border: 0; color: white; background: rgb(30, 0, 0)}");
    ui->burrowListExit->setStyleSheet("QPushButton { border: 0; color: white; background: rgb(30, 0, 0)}");
    //ui->burrowListSizeList->verticalScrollBar()->setStyleSheet(styleSheet());
    //Connect all QTextEdits so that they scroll together
    connect(ui->burrowListSizeList->verticalScrollBar(), SIGNAL(valueChanged(int)), ui->burrowListNameList->verticalScrollBar(), SLOT(setValue(int)));
    connect(ui->burrowListSizeList->verticalScrollBar(), SIGNAL(valueChanged(int)), ui->burrowListPopList->verticalScrollBar(), SLOT(setValue(int)));
    connect(ui->burrowListSizeList->verticalScrollBar(), SIGNAL(valueChanged(int)), ui->burrowListFoodList->verticalScrollBar(), SLOT(setValue(int)));
    connect(ui->burrowListNameList->verticalScrollBar(), SIGNAL(valueChanged(int)), ui->burrowListSizeList->verticalScrollBar(), SLOT(setValue(int)));
    connect(ui->burrowListPopList->verticalScrollBar(), SIGNAL(valueChanged(int)), ui->burrowListSizeList->verticalScrollBar(), SLOT(setValue(int)));
    connect(ui->burrowListFoodList->verticalScrollBar(), SIGNAL(valueChanged(int)), ui->burrowListSizeList->verticalScrollBar(), SLOT(setValue(int)));

    //ui->burrowListScrollArea->setStyleSheet("QScrollArea { border: 1px solid black;}");

    indexSelectedBurrow=-1;
    selfBlock = true;
    updateBlock = updateBlock2 = false;

}

BurrowList::~BurrowList(){
    delete ui;
}

void BurrowList::setup(){
    prepareBurrowListForDisplay();
}

void BurrowList::on_burrowListExit_clicked(){
    gui->hideMenu();
    gui->closeBurrowList();
}

void BurrowList::update(){ //Slot connected to newMonthUpdateDone signal in Registry
    updateBlock = updateBlock2 = true;  // To block the cursorPositionChanged() signal.
    prepareBurrowListForDisplay();
}

void BurrowList::prepareBurrowListForDisplay(){ //displayBurrowList() will be called on return
    int selectedBurrowId = burrowIds.value(indexSelectedBurrow);
    gui->controller->prepareBurrowListForDisplay(selectedBurrowId);
}

void BurrowList::displayBurrowList(QVector<QString> strings, QVector<int> burrowIds, QVector<int> selectedBurrowWombatIds){    
    this->burrowIds = burrowIds;
    this->wombatIds = selectedBurrowWombatIds;

    int scrollBarValue = ui->burrowListSizeList->verticalScrollBar()->value();

    ui->burrowListNameList->setText(strings[0]);
    ui->burrowListPopList->setText(strings[1]);
    ui->burrowListFoodList->setText(strings[2]);
    ui->burrowListSizeList->setText(strings[3]);
    ui->burrowTotalPopNumber->setText(strings[4]);

    ui->burrowListSizeList->verticalScrollBar()->setValue(scrollBarValue);
}


void BurrowList::on_burrowListNameList_cursorPositionChanged()
{
    //Show wombat list
    //Why does QTextEdit not have clicked() or selected() or similar?
    //Have to block the cursorPositionChanged() signal on every update now
    //Tried setting up an event filter to catch the click event but didn't work...
    //Stupid... can't figure out another way to get the line pos... nothing else worked because it is html etc...

    if (updateBlock){
        updateBlock=false;
    }else{
        QString s = ui->burrowListNameList->toPlainText();
        int cursorPos = ui->burrowListNameList->textCursor().position();

        if (selfBlock){
            selfBlock=false;
        }else{
            selfBlock=true;
            int length = s.length();
            s = s.remove(cursorPos,length);
            QStringList query = s.split("\n");
            int line = query.length();
            if (indexSelectedBurrow==line-1){ //If clicked same burrow again, hide the wombat info
                indexSelectedBurrow=-1;
                setup(); //Refill all lists, but without wombat info
            }else{ // If not, show wombat info
                indexSelectedBurrow = line-1;
                //hwGameUI->sendToConsole("line: "+QString::number(line));
                setup(); //Refill all lists, now with wombat info as well
            }
        }
    }
}

void BurrowList::on_burrowListSizeList_cursorPositionChanged(){

    if (updateBlock2){
        updateBlock2=false;
    }else{
        //Possess button clicked
        //Stupid... but can't figure out another way... nothing else worked because it is html etc...
        QString s = ui->burrowListSizeList->toPlainText();
        int cursorPos = ui->burrowListSizeList->textCursor().position();
        int length = s.length();
        int endOfLine = s.indexOf("\n",cursorPos); //Pos at end of line from cursor pos
        s = s.remove(endOfLine,length);             //Remove all lines after this pos
        int possess = s.indexOf("Possess");
        if (possess!=-1){
            s = s.remove(0,possess);                //Remove all lines before possess, now we have all wombats up to the wombat we clicked
            QStringList query = s.split("\n");      // Split each line.
            int line = query.length();
            line = line-1;                          //Dont' know why -1, but it is correct

            if (line!=-1 && line<=wombatIds.length() && query.value(line)=="    ok"){   //Make sure line index is valid and that it has the text "    ok"
                int wombatId = wombatIds.value(line-1); // -1 to get the array index, because it starts at 0
                gui->possess(wombatId);  //Change wombat
                on_burrowListExit_clicked(); //Close
            }
        }
    }
}
