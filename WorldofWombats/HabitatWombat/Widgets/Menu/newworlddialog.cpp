#include "newworlddialog.h"
#include "ui_newworlddialog.h"

NewWorldDialog::NewWorldDialog(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NewWorldDialog)
{
    ui->setupUi(this);
    QRegExp rx("^[a-zA-Z0-9_ ]*$");
    QValidator *nameValidator = new QRegExpValidator(rx,this);
    ui->lineEdit->setValidator(nameValidator);
}

NewWorldDialog::~NewWorldDialog()
{
    delete ui;
}


void NewWorldDialog::on_Ok_clicked()
{
    hwMenuUI->newWorldDialogOkClicked();
}

void NewWorldDialog::on_Cancel_clicked()
{
    hwMenuUI->newWorldDialogCancelClicked();
}
