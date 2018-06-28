#include "loadworlddialog.h"
#include "ui_loadworlddialog.h"

loadWorldDialog::loadWorldDialog(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::loadWorldDialog)
{
    ui->setupUi(this);
}

loadWorldDialog::~loadWorldDialog()
{
    delete ui;
}

void loadWorldDialog::on_Ok_clicked()
{
    hwMenuUI->loadWorldDialogOkClicked();
}

void loadWorldDialog::on_Cancel_clicked()
{
    hwMenuUI->loadWorldDialogCancelClicked();
}
