#include "amountwidget.h"
#include "ui_amountwidget.h"

AmountWidget::AmountWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AmountWidget)
{
    ui->setupUi(this);
}

AmountWidget::~AmountWidget()
{
    delete ui;
}

void AmountWidget::setup(int amount, bool parent){
    ui->horizontalSlider->setMaximum(amount);
    ui->horizontalSlider->setMinimum(0);
    ui->lineEdit->setText(QString::number(amount/2));
    ui->horizontalSlider->setValue(amount/2);

    validator = new QIntValidator(1,amount,this);
    ui->lineEdit->setValidator(validator);

    this->parent = parent;

}

void AmountWidget::on_close_clicked(){
    if(parent){
        wInventoryWidget->moveCancelled();
        wInventoryWidget->closeAmountWidget();
    }else{
        storageWidget->moveCancelled();
        storageWidget->closeAmountWidget();
    }
}

void AmountWidget::on_horizontalSlider_valueChanged(int value){
    ui->lineEdit->setText(QString::number(value));
}

void AmountWidget::on_accept_clicked(){
    if(parent){
        wInventoryWidget->moveFinished(ui->horizontalSlider->value());
        wInventoryWidget->closeAmountWidget();
    }else{
        storageWidget->moveFinished(ui->horizontalSlider->value());
        storageWidget->closeAmountWidget();
    }
}

void AmountWidget::on_lineEdit_textChanged(const QString &arg1){
    ui->horizontalSlider->setValue(arg1.toInt());
}
