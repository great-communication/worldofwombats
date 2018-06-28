#include "deleteitemwidget.h"
#include "ui_deleteitemwidget.h"

DeleteItemWidget::DeleteItemWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DeleteItemWidget)
{
    ui->setupUi(this);
}

DeleteItemWidget::~DeleteItemWidget(){
    delete ui;
}

void DeleteItemWidget::setup(bool parent){
    this->parent = parent;
}

void DeleteItemWidget::on_accept_clicked(){
    if(parent){
        wInventoryWidget->deleteItem(0);
    }else{
        storageWidget->deleteItem(0);
    }
}

void DeleteItemWidget::on_close_clicked(){
    if(parent){
        wInventoryWidget->deleteDragLabel();
        wInventoryWidget->closeDeleteItemWidget();
    }else{
        storageWidget->deleteDragLabel();
        storageWidget->closeDeleteItemWidget();
    }
}
