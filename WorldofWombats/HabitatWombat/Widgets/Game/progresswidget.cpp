#include "progresswidget.h"
#include "ui_progresswidget.h"

ProgressWidget::ProgressWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ProgressWidget)
{
    ui->setupUi(this);
}

ProgressWidget::~ProgressWidget(){
    delete ui;
}

void ProgressWidget::setup(int time){
    ui->progressBar->setStyleSheet("QProgressBar{border: 1px solid transparent;color:rgba(0,0,0,100);border-radius: 5px; padding: 1px; background-color: rgba(209, 209, 209, 100);} QProgressBar::chunk{ border-radius: 5px; background: qlineargradient(x1: 0, y1: 0.5, x2: 1, y2: 0.5, stop: 0 rgba(130, 179, 09, 100), stop: 1 rgba(160, 243, 09, 255));}");
    progressCount = 0;
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(addProgress()));
    timer->start(time/100);
}

void ProgressWidget::addProgress(){
    if(progressCount<100){
        progressCount += 1;
        ui->progressBar->setValue(progressCount);
    }else{
        timer->stop();
        gui->progressWidgetDone();
    }
}
