#include "infobox.h"
#include "ui_infobox.h"

InfoBox::InfoBox(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::InfoBox)
{
    ui->setupUi(this);
}

InfoBox::~InfoBox()
{
    delete ui;
}

void InfoBox::on_infoBoxClose_clicked()
{
    gui->closeInfoBox();
}

void InfoBox::on_infoBoxPossess_clicked()
{
    gui->possess(wombatId);
}
