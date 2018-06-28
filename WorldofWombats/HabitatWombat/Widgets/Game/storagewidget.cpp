#include "storagewidget.h"
#include "ui_storagewidget.h"
#include "../../controller.h"
#include "../../opengl.h"
#include <QKeyEvent>

StorageWidget::StorageWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StorageWidget)
{
    ui->setupUi(this);
}

StorageWidget::~StorageWidget()
{
    closeAmountWidget();

    if(movingLabel){
        draggedLabel->deleteLater();
        movingLabel = false;
    }
    delete ui;
}

void StorageWidget::setup(){
    gui->storageWidgetBackground = ui->background;

    layout = new QGridLayout(this);
    layout->setAlignment(Qt::AlignTop);
    layout->setMargin(0);
    layout->setSpacing(0);
    ui->frame->setLayout(layout);

    movingLabel = false;
    splitStack = false;
    amountWidget = false;
    amountWidgetOpen = false;
    deleteItemWidgetOpen = false;

    for(int i=0;i<gui->guiGlobal->gResources.length();i++){
        QString label = gui->guiGlobal->getResourceName(gui->guiGlobal->gResources.value(i).resource);
        resourceList.append(new QListWidgetItem(tr(label.toLatin1().constData()), ui->listWidget));
    }

    for(int i=0;i<resourceList.length();i++){
        resourceList[i]->setFlags(Qt::ItemIsUserCheckable);
        if(acceptedResources.value(i) == false){
            resourceList[i]->setCheckState(Qt::Unchecked);
        }else{
            resourceList[i]->setCheckState(Qt::Checked);
        }
    }

    ui->listWidget->setVisible(false);

    ui->listWidget->setStyleSheet("QListWidget::indicator { width: 15px; height:15px; } QListWidget::indicator:unchecked { image: url(:/HabitatWombat/Images/Icons/HabitatWombat/Images/Icons/unchecked.png) } QListWidget::indicator:checked { image: url(:/HabitatWombat/Images/Icons/HabitatWombat/Images/Icons/checked.png) } QListWidget { border: 1px solid black; color: white; background: rgb(30, 0, 0); }");


    prepareUpdate();
    checkAcceptedResources();
}

void StorageWidget::prepareUpdate(){
    gui->controller->getStorageInventory(burrowId, storageId);
}

void StorageWidget::update(const QVector<InventoryItem>& inventory, int percentageFull){
    this->inventory = inventory;
    if(!movingLabel){
        //Delete all old labels
        for (int i=0;i<labels.length();i++){
            labels[i]->deleteLater();
        }
        labels.clear();

        //Create new labels
        for (int i=0;i<inventory.length();i++){
            QLabel *label = new QLabel (this);
            label->setStyleSheet("QLabel { color: white; border: 0;}");
            QString resourceName = gui->guiGlobal->getResourceName(inventory.value(i).resource);
            QString amount = QString::number(inventory.value(i).amount);
            label->setText(resourceName+" "+amount);            
            layout->addWidget(label);
            label->installEventFilter(this);
            labels.append(label);
        }

        if(inventory.length()==0){
            QLabel *label = new QLabel (this);
            label->setText("Empty");
            label->setStyleSheet("QLabel { color: white; border: 0;}");            
            layout->addWidget(label);
            labels.append(label);
        }

        ui->labelFull->setText(QString::number(percentageFull)+"%");

    }
}

void StorageWidget::on_storageClose_clicked(){
    gui->closeStorageWidget();
}

QRect StorageWidget::getStorageWidgetDropAreaPos(){
    QPoint pos = this->mapToGlobal(ui->frame->pos());
    QRect rect = QRect(pos.x(),pos.y(),ui->frame->width(),ui->frame->height());
    return rect;
}

bool StorageWidget::eventFilter(QObject *obj, QEvent *event)
{
    // This function repeatedly call for those QObjects
    // which have installed eventFilter (Step 2)
    for (int i=0;i<labels.length();i++){
        if (obj == (QObject*)labels[i]){
            if (event->type() == QEvent::MouseButtonPress){
                const QMouseEvent* const mouseEvent = static_cast<const QMouseEvent*>( event ); //why the fuck can I not check left mouse button in QEvent??
                if(mouseEvent->buttons() == Qt::LeftButton || mouseEvent->buttons() == Qt::RightButton){
                    if(!movingLabel && !amountWidgetOpen){
                        closeAllDialogsInWInventoryWidget();

                        if(mouseEvent->buttons() == Qt::LeftButton){
                            splitStack = false;
                        }else if(mouseEvent->buttons() == Qt::RightButton){
                            splitStack = true;
                        }

                        movingLabel = true;
                        index = i;
                        /*QPoint mainWindowOffset = gui->getMainWindowOffset();
                        QPoint fix;
                        if (mainWindowOffset == QPoint(0,0)){
                            fix = QPoint(20,35);
                        }else{
                            fix = QPoint(20,15);
                        }
                        cursorPos=QCursor::pos()-mainWindowOffset-fix;*/
                        cursorPos = gui->mapFromGlobal(QCursor::pos());
                        //labels[i]->setText("hej");
                        labels[i]->setStyleSheet("QLabel { color: grey; border: 0;}");
                        dragSource = labels[i];
                        draggedLabel = new QLabel (gui);
                        draggedLabel->setStyleSheet("QLabel { color: white; border: 0;}");
                        draggedLabel->setText(labels[i]->text());
                        draggedLabel->setAlignment(Qt::AlignCenter);
                        //draggedLabel->setText(QString::number(cursorPos.x())+","+QString::number(cursorPos.y()));
                        offset = QPoint(draggedLabel->width()/2,draggedLabel->height()/2);
                        draggedLabel->move(cursorPos-offset);
                        draggedLabel->setVisible(true);                        
                        //labels[i]->setText(QString::number(offset.x())+","+QString::number(offset.y()));
                    }
                }
            }else if (event->type() == QEvent::MouseMove){
                if (movingLabel && !amountWidgetOpen){
                    closeAllDialogsInWInventoryWidget();
                    closeDeleteItemWidget();
                    moveWidget(draggedLabel);
                }

            }else if (event->type() == QEvent::MouseButtonRelease){
                if (movingLabel && !amountWidgetOpen){
                    if(gui->wInventoryWidgetOpen && gui->wInventoryWidget->wombatId == gui->openGL->wombatsOnDisplay.value(0).wombatId){
                        QRect dropArea = gui->wInventoryWidget->getWInventoryWidgetDropAreaPos();
                        bool inDropArea = dropArea.contains(QCursor::pos());                        
                        if(inDropArea){
                            if(splitStack){
                                showAmountWidget();
                            }else{
                                moveFinished(0); //The parameter will not be used in this case
                            }                            
                        }else{
                            showDeleteItemWidget();
                        }
                    }else{
                        showDeleteItemWidget();
                    }
                }
            }
            break;
        }
    }

    // pass the event on to the parent class
    return QWidget::eventFilter(obj, event);

}

void StorageWidget::moveWidget(QWidget *widget){

    if (movingLabel){
        // Messy, clean it up!
        QPoint temp = gui->mapFromGlobal(QCursor::pos())-offset;
        //draggedLabel->setText(QString::number(QCursor::pos().x())+","+QString::number(QCursor::pos().y()));
        QPoint temp2;
        if (temp.x()  > 0 && temp.x()+widget->width() < gui->windowSize.width()){
            temp2.setX(temp.x());
            temp2.setY(widget->pos().y());
            widget->move(temp2);
        }else if (temp.x()  <= 0){
            temp2.setX(0);
            temp2.setY(widget->pos().y());
            widget->move(temp2);
        }else {
            temp2.setX(gui->windowSize.width()-widget->width());
            temp2.setY(widget->pos().y());
            widget->move(temp2);
        }
        if (temp.y()  > 0 && temp.y()+widget->height() < gui->windowSize.height()){
            temp2.setX(widget->pos().x());
            temp2.setY(temp.y());
            widget->move(temp2);
        }else if (temp.y()  <=0){
            temp2.setX(widget->pos().x());
            temp2.setY(0);
            widget->move(temp2);
        }else{
            temp2.setX(widget->pos().x());
            temp2.setY(gui->windowSize.height()-widget->height());
            widget->move(temp2);
        }
    }
}

void StorageWidget::showDeleteItemWidget(){
    if(!deleteItemWidgetOpen){
        deleteItemWidgetOpen = true;
        deleteItemWidget = new DeleteItemWidget(gui);
        QPoint mainWindowOffset = gui->getMainWindowOffset();
        QPoint fix;
        if (mainWindowOffset == QPoint(0,0)){
            fix = QPoint(20,35);
        }else{
            fix = QPoint(20,15);
        }
        QPoint widgetOffset = QPoint((-(deleteItemWidget->width()/2))+25,draggedLabel->height());
        QPoint pos = QCursor::pos()-mainWindowOffset-fix+widgetOffset;
        deleteItemWidget->move(pos);
        deleteItemWidget->storageWidget = this;
        deleteItemWidget->setup(false);
        deleteItemWidget->setVisible(true);
    }
}

void StorageWidget::closeDeleteItemWidget(){
    if(deleteItemWidgetOpen){
        deleteItemWidget->deleteLater();
        deleteItemWidgetOpen=false;
        gui->enableEvents->setFocus();
    }
}

void StorageWidget::deleteDragLabel(){
    if(movingLabel){
        dragSource->setStyleSheet("QLabel { color: white; border: 0;}");
        draggedLabel->deleteLater();
        movingLabel = false;
    }
}

void StorageWidget::deleteItem(int amount){
    InventoryItem item = inventory.value(index);
    //if (splitStack){
    //    item.amount = amount;
    //}
    gui->controller->removeItemFromStorage(burrowId,storageId,item);
    deleteDragLabel();
    closeDeleteItemWidget();
    gui->enableEvents->setFocus();
}

void StorageWidget::showAmountWidget(){
    amountWidget = new AmountWidget(gui);
   /* QPoint mainWindowOffset = gui->getMainWindowOffset();
    QPoint fix;
    if (mainWindowOffset == QPoint(0,0)){
        fix = QPoint(20,35);
    }else{
        fix = QPoint(20,15);
    }
    QPoint widgetOffset = QPoint((-(amountWidget->width()/2))+25,draggedLabel->height());*/
    QPoint pos = gui->mapFromGlobal(QCursor::pos());
    amountWidget->move(pos);
    amountWidget->storageWidget = this;    
    int amount = inventory.value(index).amount;
    amountWidget->setup(amount,false);
    amountWidget->setVisible(true);
    amountWidgetOpen = true;
}

void StorageWidget::closeAmountWidget(){
    if(amountWidgetOpen){
        amountWidget->deleteLater();
        amountWidgetOpen=false;
    }
}

void StorageWidget::moveCancelled(){
    if(movingLabel){
        dragSource->setStyleSheet("QLabel { color: white; border: 0;}");
        draggedLabel->deleteLater();
        movingLabel = false;
    }
}

void StorageWidget::moveFinished(int amount){
    dragSource->setStyleSheet("QLabel { color: white; border: 0;}");
    if(gui->wInventoryWidgetOpen){
        InventoryItem item = inventory.value(index);  // Index is not guaranteed to be same (actually I think it is ok)
        if (splitStack){
            item.amount = amount;
        }
        gui->controller->moveItemFromStorage(item,gui->wInventoryWidget->wombatId,burrowId,storageId,draggedLabel->pos());
    }
    draggedLabel->deleteLater();
    movingLabel = false;
}

void StorageWidget::closeAllDialogsInWInventoryWidget(){
    if(gui->wInventoryWidgetOpen){
        if(gui->wInventoryWidget->amountWidgetOpen){
            gui->wInventoryWidget->deleteDragLabel();
            gui->wInventoryWidget->closeAmountWidget();
        }else if(gui->wInventoryWidget->deleteItemWidgetOpen){
            gui->wInventoryWidget->deleteDragLabel();
            gui->wInventoryWidget->closeDeleteItemWidget();
        }
    }
}

void StorageWidget::on_acceptedResourcesButton_clicked(){
    if(ui->listWidget->isVisible()){
        ui->listWidget->setVisible(false);
    }else{
        ui->listWidget->setVisible(true);
    }
}

void StorageWidget::on_listWidget_itemClicked(QListWidgetItem *item){
    if(item->checkState()==Qt::Unchecked){
        item->setCheckState(Qt::Checked);
    }else{
        item->setCheckState(Qt::Unchecked);
    }

    checkAcceptedResources();
    setAcceptedResourcesInRegistry();
}

void StorageWidget::checkAcceptedResources(){
    int resourceAcceptedCount = 0;
    int index = 0;

    for(int i=0;i<resourceList.length();i++){
        if(resourceList.value(i)->checkState() == Qt::Unchecked){
            if(i>=0 && i<acceptedResources.length()){
                acceptedResources[i] = false;
            }
        }else{
            if(i>=0 && i<acceptedResources.length()){
                acceptedResources[i] = true;
            }
            index = i;
            resourceAcceptedCount++;
        }
    }

    if(resourceAcceptedCount == 0){
        setAcceptedResourcesButtonText("No Resources Accepted");
    }else if(resourceAcceptedCount == 1){
        QString resourceName = gui->guiGlobal->getResourceName(gui->guiGlobal->gResources.value(index).resource);
        setAcceptedResourcesButtonText("Only "+resourceName+" accepted");
    }else if (resourceAcceptedCount == resourceList.length()){
        setAcceptedResourcesButtonText("All Resources Accepted");
    }else{
        setAcceptedResourcesButtonText("Multiple Resources Accepted");
    }
}

void StorageWidget::setAcceptedResourcesInRegistry(){
    gui->controller->setAcceptedResourcesInStorage(burrowId,storageId,acceptedResources);
}

void StorageWidget::setAcceptedResourcesButtonText(QString text){
    ui->acceptedResourcesButton->setText(text);
}
