#include "winventorywidget.h"
#include "ui_winventorywidget.h"
#include "../../controller.h"
#include <QKeyEvent>

WInventoryWidget::WInventoryWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WInventoryWidget)
{
    ui->setupUi(this);
}

WInventoryWidget::~WInventoryWidget()
{
    closeAmountWidget();

    if(movingLabel){
        draggedLabel->deleteLater();
        movingLabel = false;
    }
    delete ui;
}

void WInventoryWidget::setup(){
    gui->wInventoryWidgetBackground = ui->background; //Detection box for dragging?

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

    prepareUpdate();
}

void WInventoryWidget::prepareUpdate(){
    gui->controller->getWombatInventory(wombatId);
}

void WInventoryWidget::update(const QVector<InventoryItem>& inventory, int selectedWombat){
    this->inventory = inventory;
    //Delete all old labels
    for (int i=0;i<labels.length();i++){
        labels[i]->deleteLater();
    }
    labels.clear();

    //Create new labels
    int usedSize = 0;
    for (int i=0;i<inventory.length();i++){
        QLabel *label = new QLabel (this);
        label->setStyleSheet("QLabel { color: white; border: 0;}");
        QString resourceName = gui->guiGlobal->getResourceName(inventory.value(i).resource);
        QString amount = QString::number(inventory.value(i).amount);
        label->setText(resourceName+" "+amount);
        layout->addWidget(label);
        if(wombatId == selectedWombat){
            label->installEventFilter(this);
        }
        labels.append(label);
        usedSize += gui->guiGlobal->getItemSize(inventory.value(i));
    }

    if(inventory.length()==0){
        QLabel *label = new QLabel (this);
        label->setText("Empty");
        label->setStyleSheet("QLabel { color: white; border: 0;}");
        layout->addWidget(label);
        labels.append(label);
    }

    //Calculate how full the storage is
    int totalSize = gWombatInventorySize;

    int percentage = ((double)usedSize/(double)totalSize)*100;

    ui->labelFull->setText(QString::number(percentage)+"%");
}

void WInventoryWidget::on_inventoryClose_clicked(){
    closeDeleteItemWidget();
    gui->closeWInventoryWidget();
}

QRect WInventoryWidget::getWInventoryWidgetDropAreaPos(){
    QPoint pos = this->mapToGlobal(ui->frame->pos());
    QRect rect = QRect(pos.x(),pos.y(),ui->frame->width(),ui->frame->height());
    return rect;
}

bool WInventoryWidget::eventFilter(QObject *obj, QEvent *event)
{
    // This function repeatedly call for those QObjects
    // which have installed eventFilter (Step 2)
    for (int i=0;i<labels.length();i++){
        if (obj == (QObject*)labels[i]){
            if (event->type() == QEvent::MouseButtonPress){
                const QMouseEvent* const mouseEvent = static_cast<const QMouseEvent*>( event ); //why the fuck can I not check left mouse button in QEvent??
                if(mouseEvent->buttons() == Qt::LeftButton || mouseEvent->buttons() == Qt::RightButton){                    
                    if(!movingLabel && !amountWidgetOpen){
                        closeAllDialogsInStorageWidget();

                        if(mouseEvent->buttons() == Qt::LeftButton){
                            splitStack = false;
                        }else if(mouseEvent->buttons() == Qt::RightButton){
                            splitStack = true;
                        }

                        movingLabel = true;
                        index = i;
                       /* QPoint mainWindowOffset = gui->getMainWindowOffset();
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
                    closeAllDialogsInStorageWidget();
                    closeDeleteItemWidget();
                    moveWidget(draggedLabel);
                }

            }else if (event->type() == QEvent::MouseButtonRelease){                 
                if (movingLabel && !amountWidgetOpen){
                    if(gui->storageWidgetOpen){
                        QRect dropArea = gui->storageWidget->getStorageWidgetDropAreaPos();
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

void WInventoryWidget::moveWidget(QWidget *widget){

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

void WInventoryWidget::showDeleteItemWidget(){
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
        deleteItemWidget->wInventoryWidget = this;
        deleteItemWidget->setup(true);
        deleteItemWidget->setVisible(true);
    }
}

void WInventoryWidget::closeDeleteItemWidget(){
    if(deleteItemWidgetOpen){
        deleteItemWidget->deleteLater();
        deleteItemWidgetOpen=false;
        gui->enableEvents->setFocus();
    }
}

void WInventoryWidget::deleteDragLabel(){
    if(movingLabel){
        dragSource->setStyleSheet("QLabel { color: white; border: 0;}");
        draggedLabel->deleteLater();
        movingLabel = false;
    }
}

void WInventoryWidget::deleteItem(int amount){
    InventoryItem item = inventory.value(index);
    //if (splitStack){
    //    item.amount = amount;
    //}
    gui->controller->removeItemFromWombatInventory(wombatId,item);
    deleteDragLabel();
    closeDeleteItemWidget();
    gui->enableEvents->setFocus();
}

void WInventoryWidget::showAmountWidget(){
    amountWidget = new AmountWidget(gui);
    QPoint mainWindowOffset = gui->getMainWindowOffset();
    QPoint fix;
    if (mainWindowOffset == QPoint(0,0)){
        fix = QPoint(20,35);
    }else{
        fix = QPoint(20,15);
    }
    QPoint widgetOffset = QPoint((-(amountWidget->width()/2))+25,draggedLabel->height());
    QPoint pos = QCursor::pos()-mainWindowOffset-fix+widgetOffset;
    amountWidget->move(pos);
    amountWidget->wInventoryWidget = this;
    //int wombatIndex = registry->getWombatIndex(wombatId);
    int amount = inventory.value(index).amount;
    amountWidget->setup(amount,true);
    amountWidget->setVisible(true);
    amountWidgetOpen = true;
}

void WInventoryWidget::closeAmountWidget(){
    if(amountWidgetOpen){
        amountWidget->deleteLater();
        amountWidgetOpen=false;
    }
}

void WInventoryWidget::moveCancelled(){
    if(movingLabel){
        dragSource->setStyleSheet("QLabel { color: white; border: 0;}");
        draggedLabel->deleteLater();
        movingLabel = false;
    }
}

void WInventoryWidget::moveFinished(int amount){
    dragSource->setStyleSheet("QLabel { color: white; border: 0;}");
    if(gui->storageWidgetOpen){
        //int wombatIndex = hwRegistry->getWombatIndex(wombatId);
        InventoryItem item = inventory.value(index);
        if (splitStack){
            item.amount = amount;
        }
        gui->controller->moveItemFromWombat(item,wombatId,gui->storageWidget->burrowId,gui->storageWidget->storageId,draggedLabel->pos());
    }
    draggedLabel->deleteLater();
    movingLabel = false;
}

void WInventoryWidget::closeAllDialogsInStorageWidget(){
    if(gui->storageWidgetOpen){
        if(gui->storageWidget->amountWidgetOpen){
            gui->storageWidget->deleteDragLabel();
            gui->storageWidget->closeAmountWidget();
        }else if(gui->storageWidget->deleteItemWidgetOpen){
            gui->storageWidget->deleteDragLabel();
            gui->storageWidget->closeDeleteItemWidget();
        }
    }
}
