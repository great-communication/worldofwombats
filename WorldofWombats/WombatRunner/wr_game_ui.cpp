#include "wr_game_ui.h"
#include "ui_wr_game_ui.h"
#include "wr_announcer.h"
#include <QIntValidator>
#include <QRegExp>
#include <QMovie>
#include <string>


WR_GAME_UI::WR_GAME_UI(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WR_GAME_UI)
{
    ui->setupUi(this);
    //WR game
    ui->wr_startrunsel->setVisible(false);
    ui->wr_startrunmousearea->installEventFilter(this);
    ui->wr_exitsel->setVisible(false);
    ui->wr_exitmousearea->installEventFilter(this);
    ui->wr_addplayer1sel->setVisible(false);
    ui->wr_addplayer1mousearea->installEventFilter(this);
    ui->wr_addplayer2->setVisible(false);
    ui->wr_addplayer2sel->setVisible(false);
    ui->wr_addplayer3->setVisible(false);
    ui->wr_addplayer3sel->setVisible(false);

    QPalette p = ui->wr_bet_1->palette();
    p.setColor(QPalette::Base, QColor(0,0,0,0)); // r,g,b,A
    p.setColor(QPalette::Text,Qt::yellow);
    ui->wr_bet_1->setPalette(p);
    ui->wr_bet_1->setVisible(false);
    ui->wr_bet_2->setPalette(p);
    ui->wr_bet_2->setVisible(false);
    ui->wr_bet_3->setPalette(p);
    ui->wr_bet_3->setVisible(false);
    ui->wr_bet_1_enterbettext->setStyleSheet("QLabel { color : yellow; }");
    ui->wr_bet_1_enterbettext->setVisible(false);
    ui->wr_bet_2_enterbettext->setStyleSheet("QLabel { color : yellow; }");
    ui->wr_bet_2_enterbettext->setVisible(false);
    ui->wr_bet_3_enterbettext->setStyleSheet("QLabel { color : yellow; }");
    ui->wr_bet_3_enterbettext->setVisible(false);

    wr_bet.append(ui->wr_bet_1);
    wr_bet.append(ui->wr_bet_2);
    wr_bet.append(ui->wr_bet_3);

    p = ui->wr_betw_1->palette();
    p.setColor(QPalette::Base, QColor(0,0,0,0)); // r,g,b,A
    p.setColor(QPalette::Text,Qt::yellow);
    ui->wr_betw_1->setPalette(p);
    ui->wr_betw_1->setVisible(false);
    ui->wr_betw_2->setPalette(p);
    ui->wr_betw_2->setVisible(false);
    ui->wr_betw_3->setPalette(p);
    ui->wr_betw_3->setVisible(false);
    ui->wr_betw_1_enterlanetext->setStyleSheet("QLabel { color : yellow; }");
    ui->wr_betw_1_enterlanetext->setVisible(false);
    ui->wr_betw_2_enterlanetext->setStyleSheet("QLabel { color : yellow; }");
    ui->wr_betw_2_enterlanetext->setVisible(false);
    ui->wr_betw_3_enterlanetext->setStyleSheet("QLabel { color : yellow; }");
    ui->wr_betw_3_enterlanetext->setVisible(false);

    wr_betw.append(ui->wr_betw_1);
    wr_betw.append(ui->wr_betw_2);
    wr_betw.append(ui->wr_betw_3);

    p = ui->wr_messageboard->palette();
    p.setColor(QPalette::Text,QColor(229,185,48,255));
    ui->wr_messageboard->setPalette(p);
    ui->wr_messageboard->setStyleSheet("QTextEdit { border: 6px solid black; padding: 5px; background: rgba(170, 55, 44, 80)}");

    QRegExp rx("^[^|]*$");
    QValidator *nameValidator = new QRegExpValidator(rx,this);

    ui->wr_playername1->setValidator(nameValidator);
    ui->wr_playername2->setValidator(nameValidator);
    ui->wr_playername3->setValidator(nameValidator);

    ui->wr_playername1->setStyleSheet("QLabel { color : yellow; }");
    ui->wr_playername2->setStyleSheet("QLabel { color : yellow; }");
    ui->wr_playername3->setStyleSheet("QLabel { color : yellow; }");

    p = ui->wr_playername1->palette();
    p.setColor(QPalette::Base, QColor(0,0,0,0)); // r,g,b,A
    p.setColor(QPalette::Text,Qt::yellow);
    ui->wr_playername1->setPalette(p);
    ui->wr_playername1->setStyleSheet("QLineEdit { background: rgba(0, 0, 0, 0)}");
    ui->wr_playername1->setVisible(false);
    ui->wr_playername2->setPalette(p);
    ui->wr_playername2->setStyleSheet("QLineEdit { background: rgba(0, 0, 0, 0)}");
    ui->wr_playername2->setVisible(false);
    ui->wr_playername3->setPalette(p);
    ui->wr_playername3->setStyleSheet("QLineEdit { background: rgba(0, 0, 0, 0)}");
    ui->wr_playername3->setVisible(false);

    wr_playername.append(ui->wr_playername1);
    wr_playername.append(ui->wr_playername2);
    wr_playername.append(ui->wr_playername3);

    ui->wr_cash_1->setStyleSheet("QLabel { color : yellow; }");
    ui->wr_cash_1->setVisible(false);
    ui->wr_cash_1_dollarsign->setStyleSheet("QLabel { color : yellow; }");
    ui->wr_cash_1_dollarsign->setVisible(false);
    ui->wr_bet_1_dollarsign->setStyleSheet("QLabel { color : yellow; }");
    ui->wr_bet_1_dollarsign->setVisible(false);
    ui->wr_cash_2->setStyleSheet("QLabel { color : yellow; }");
    ui->wr_cash_2->setVisible(false);
    ui->wr_cash_2_dollarsign->setStyleSheet("QLabel { color : yellow; }");
    ui->wr_cash_2_dollarsign->setVisible(false);
    ui->wr_bet_2_dollarsign->setStyleSheet("QLabel { color : yellow; }");
    ui->wr_bet_2_dollarsign->setVisible(false);
    ui->wr_cash_3->setStyleSheet("QLabel { color : yellow; }");
    ui->wr_cash_3->setVisible(false);
    ui->wr_cash_3_dollarsign->setStyleSheet("QLabel { color : yellow; }");
    ui->wr_cash_3_dollarsign->setVisible(false);
    ui->wr_bet_3_dollarsign->setStyleSheet("QLabel { color : yellow; }");
    ui->wr_bet_3_dollarsign->setVisible(false);

    wr_cash.append(ui->wr_cash_1);
    wr_cash.append(ui->wr_cash_2);
    wr_cash.append(ui->wr_cash_3);
    wr_cash_dollarsign.append(ui->wr_cash_1_dollarsign);
    wr_cash_dollarsign.append(ui->wr_cash_2_dollarsign);
    wr_cash_dollarsign.append(ui->wr_cash_3_dollarsign);
    wr_bet_dollarsign.append(ui->wr_bet_1_dollarsign);
    wr_bet_dollarsign.append(ui->wr_bet_2_dollarsign);
    wr_bet_dollarsign.append(ui->wr_bet_3_dollarsign);

    startDisabled = false;

    wombatRange = new QIntValidator(1,3);
    ui->wr_betw_1->setValidator(wombatRange);
    ui->wr_betw_2->setValidator(wombatRange);
    ui->wr_betw_3->setValidator(wombatRange);
    betRangePlayer1 = new QIntValidator(1,100);
    ui->wr_bet_1->setValidator(betRangePlayer1);
    betRangePlayer2 = new QIntValidator(1,100);
    ui->wr_bet_2->setValidator(betRangePlayer2);
    betRangePlayer3 = new QIntValidator(1,100);
    ui->wr_bet_3->setValidator(betRangePlayer3);

    betRange.append(betRangePlayer1);
    betRange.append(betRangePlayer2);
    betRange.append(betRangePlayer3);





    //Gif Animations
    wombatAnimation = new QMovie(":/Images/wr/Images/wr/runwombatrun.gif");
    ui->wombat1->setMovie(wombatAnimation);
    ui->wombat2->setMovie(wombatAnimation);
    ui->wombat3->setMovie(wombatAnimation);
    wombatAnimation->start();
    wombatAnimation->stop();
}

WR_GAME_UI::~WR_GAME_UI()
{
    delete ui;
}

void WR_GAME_UI::updateCash(int cash, int index){
        wr_cash.value(index)->setText(QString::number(cash));
        betRange.value(index)->setRange(1,cash);
}

void WR_GAME_UI::startWombatAnimation() {
    wombatAnimation->start();
}

void WR_GAME_UI::stopWombatAnimation() {

    wombatAnimation->stop();
    wombatAnimation->start();  //Have to start and stop again to reset
    wombatAnimation->stop();
}

void WR_GAME_UI::updateWombatPositions(int w1Pos, int w2Pos, int w3Pos){
    ui->wombat1->setGeometry(w1Pos,560,300,150);
    ui->wombat2->setGeometry(w2Pos,660,300,150);
    ui->wombat3->setGeometry(w3Pos,760,300,150);
}

void WR_GAME_UI::disableInput(){
    ui->wr_betw_1->setDisabled(1);
    ui->wr_bet_1->setDisabled(1);
    ui->wr_betw_2->setDisabled(1);
    ui->wr_bet_2->setDisabled(1);
    ui->wr_betw_3->setDisabled(1);
    ui->wr_bet_3->setDisabled(1);
    startDisabled = true;
}

void WR_GAME_UI::enableInput(){
    ui->wr_bet_1->setEnabled(1);
    ui->wr_betw_1->setEnabled(1);
    ui->wr_bet_2->setEnabled(1);
    ui->wr_betw_2->setEnabled(1);
    ui->wr_bet_3->setEnabled(1);
    ui->wr_betw_3->setEnabled(1);
    startDisabled = false;
}

void WR_GAME_UI::clearInvalidBets(int index, int money){
        int bet = wr_bet.value(index)->text().toInt();
        if (bet>money) {
            wr_bet.value(index)->setText("");
        }
}

void WR_GAME_UI::updateMessageBoard(QString output){
    ui->wr_messageboard->append(output);
}

void WR_GAME_UI::resetMessageBoard(){
    ui->wr_messageboard->setText("");
}

void WR_GAME_UI::disablePlayer(int index){
    wr_bet.value(index)->setVisible(false);
    wr_betw.value(index)->setVisible(false);
    QPalette p = wr_playername.value(index)->palette();
    p.setColor(QPalette::Text,Qt::black);
    wr_playername.value(index)->setPalette(p);
    wr_cash_dollarsign.value(index)->setStyleSheet("QLabel { color : black; }");
    wr_cash.value(index)->setStyleSheet("QLabel { color : black; }");
    wr_bet_dollarsign.value(index)->setVisible(false);
}






//To get mouse enter/leave and mouse click functionality on qLabels
bool WR_GAME_UI::eventFilter(QObject *obj, QEvent *event)
{
    // This function repeatedly call for those QObjects
    // which have installed eventFilter (Step 2)

    if (obj == (QObject*)ui->wr_startrunmousearea){

                if (event->type() == QEvent::Enter)
                {
                    if (startDisabled == false) {
                        ui->wr_startrun->setVisible(false);
                        ui->wr_startrunsel->setVisible(true);
                    }
                }else if (event->type() == QEvent::Leave) {
                    ui->wr_startrun->setVisible(true);
                    ui->wr_startrunsel->setVisible(false);
                }else if (event->type() == QEvent::MouseButtonPress)
                {
                    if (startDisabled == false) {
                        int wombat, betAmount;
                        if (ui->wr_betw_1->text().isEmpty()) {
                            wombat = 0;
                        }else{
                            wombat = ui->wr_betw_1->text().toInt();
                        }
                        if (ui->wr_bet_1->text().isEmpty()){
                            betAmount = 0;
                        }else{
                            betAmount = ui->wr_bet_1->text().toInt();
                        }
                        QList<int> wombatArray;
                        wombatArray.append(wombat);

                        QList<int> betAmountArray;
                        betAmountArray.append(betAmount);

                        if (ui->wr_betw_2->text().isEmpty()) {
                            wombat = 0;
                        }else{
                            wombat = ui->wr_betw_2->text().toInt();
                        }
                        if (ui->wr_bet_2->text().isEmpty()){
                            betAmount = 0;
                        }else{
                            betAmount = ui->wr_bet_2->text().toInt();
                        }

                        wombatArray.append(wombat);

                        betAmountArray.append(betAmount);

                        if (ui->wr_betw_3->text().isEmpty()) {
                            wombat = 0;
                        }else{
                            wombat = ui->wr_betw_3->text().toInt();
                        }
                        if (ui->wr_bet_3->text().isEmpty()){
                            betAmount = 0;
                        }else{
                            betAmount = ui->wr_bet_3->text().toInt();
                        }

                        wombatArray.append(wombat);

                        betAmountArray.append(betAmount);

                        emit s_setAllBets(wombatArray, betAmountArray);
                        emit s_startRace();
                    }
                }

            return QWidget::eventFilter(obj, event);

        }else if (obj == (QObject*)ui->wr_exitmousearea){
           if (event->type() == QEvent::Enter)
           {
               ui->wr_exit->setVisible(false);
               ui->wr_exitsel->setVisible(true);

           }else if (event->type() == QEvent::Leave) {
               ui->wr_exit->setVisible(true);
               ui->wr_exitsel->setVisible(false);
           }else if (event->type() == QEvent::MouseButtonRelease)
           {
               emit s_checkIfNewHighScoreForAllPlayers();
               emit s_storeHighScoreOnFile();
               emit s_stopRace();
               emit s_backToWRGameMenu();
               resetMessageBoard();
               ui->wr_bet_1->setText("");
               ui->wr_betw_1->setText("");
               ui->wr_addplayer1->setVisible(true);
               ui->wr_addplayer1sel->setVisible(false);
               ui->wr_addplayer1mousearea->removeEventFilter(this);
               ui->wr_addplayer1mousearea->installEventFilter(this);
               ui->wr_addplayer1mousearea->setVisible(true);
               ui->wr_playername1->setVisible(false);
               ui->wr_playername1->setText("Enter name");
               ui->wr_cash_1->setVisible(false);
               ui->wr_cash_1_dollarsign->setVisible(false);
               ui->wr_bet_1->setVisible(false);
               ui->wr_bet_1_dollarsign->setVisible(false);
               ui->wr_betw_1->setVisible(false);
               ui->wr_bet_2->setText("");
               ui->wr_betw_2->setText("");
               ui->wr_addplayer2->setVisible(false);
               ui->wr_addplayer2sel->setVisible(false);
               ui->wr_addplayer2mousearea->removeEventFilter(this);
               ui->wr_addplayer2mousearea->setVisible(true);
               ui->wr_playername2->setVisible(false);
               ui->wr_playername2->setText("Enter name");
               ui->wr_cash_2->setVisible(false);
               ui->wr_cash_2_dollarsign->setVisible(false);
               ui->wr_bet_2->setVisible(false);
               ui->wr_bet_2_dollarsign->setVisible(false);
               ui->wr_betw_2->setVisible(false);
               ui->wr_bet_3->setText("");
               ui->wr_betw_3->setText("");
               ui->wr_addplayer3->setVisible(false);
               ui->wr_addplayer3sel->setVisible(false);
               ui->wr_addplayer3mousearea->removeEventFilter(this);
               ui->wr_addplayer3mousearea->setVisible(true);
               ui->wr_playername3->setVisible(false);
               ui->wr_playername3->setText("Enter name");
               ui->wr_cash_3->setVisible(false);
               ui->wr_cash_3_dollarsign->setVisible(false);
               ui->wr_bet_3->setVisible(false);
               ui->wr_bet_3_dollarsign->setVisible(false);
               ui->wr_betw_3->setVisible(false);
               ui->wr_bet_1_enterbettext->setVisible(false);
               ui->wr_bet_2_enterbettext->setVisible(false);
               ui->wr_bet_3_enterbettext->setVisible(false);
               ui->wr_betw_1_enterlanetext->setVisible(false);
               ui->wr_betw_2_enterlanetext->setVisible(false);
               ui->wr_betw_3_enterlanetext->setVisible(false);
               enableInput();

               for (int i=0;i<3;i++){
                   QPalette p = wr_playername.value(i)->palette();
                   p.setColor(QPalette::Text,Qt::yellow);
                   wr_playername.value(i)->setPalette(p);
                   wr_cash_dollarsign.value(i)->setStyleSheet("QLabel { color : yellow; }");
                   wr_cash.value(i)->setStyleSheet("QLabel { color : yellow; }");
               }
               connect(ui->wr_bet_1, SIGNAL(textEdited(QString)),this,SLOT(on_wr_bet_1_textEdited(QString)));
               connect(ui->wr_bet_2, SIGNAL(textEdited(QString)),this,SLOT(on_wr_bet_2_textEdited(QString)));
               connect(ui->wr_bet_3, SIGNAL(textEdited(QString)),this,SLOT(on_wr_bet_3_textEdited(QString)));
               connect(ui->wr_betw_1, SIGNAL(textEdited(QString)),this,SLOT(on_wr_betw_1_textEdited(QString)));
               connect(ui->wr_betw_2, SIGNAL(textEdited(QString)),this,SLOT(on_wr_betw_2_textEdited(QString)));
               connect(ui->wr_betw_3, SIGNAL(textEdited(QString)),this,SLOT(on_wr_betw_3_textEdited(QString)));


           }

           return QWidget::eventFilter(obj, event);

       }else if (obj == (QObject*)ui->wr_addplayer1mousearea){
           if (event->type() == QEvent::Enter)
           {
               ui->wr_addplayer1->setVisible(false);
               ui->wr_addplayer1sel->setVisible(true);

           }else if (event->type() == QEvent::Leave) {
               ui->wr_addplayer1->setVisible(true);
               ui->wr_addplayer1sel->setVisible(false);
           }else if (event->type() == QEvent::MouseButtonRelease)
           {
               ui->wr_addplayer1->setVisible(false);
               ui->wr_addplayer1sel->setVisible(false);
               ui->wr_addplayer1mousearea->removeEventFilter(this);
               ui->wr_addplayer1mousearea->setVisible(false);
               ui->wr_playername1->setVisible(true);
               ui->wr_playername1->setEnabled(1);
               ui->wr_playername1->setFocus();
               ui->wr_playername1->selectAll();
           }

           return QWidget::eventFilter(obj, event);

       }else if (obj == (QObject*)ui->wr_addplayer2mousearea){
           if (event->type() == QEvent::Enter)
           {
               ui->wr_addplayer2->setVisible(false);
               ui->wr_addplayer2sel->setVisible(true);

           }else if (event->type() == QEvent::Leave) {
               ui->wr_addplayer2->setVisible(true);
               ui->wr_addplayer2sel->setVisible(false);
           }else if (event->type() == QEvent::MouseButtonRelease)
           {
               ui->wr_addplayer2->setVisible(false);
               ui->wr_addplayer2sel->setVisible(false);
               ui->wr_addplayer2mousearea->removeEventFilter(this);
               ui->wr_addplayer2mousearea->setVisible(false);
               ui->wr_playername2->setVisible(true);
               ui->wr_playername2->setEnabled(1);
               ui->wr_playername2->setFocus();
               ui->wr_playername2->selectAll();
           }

           return QWidget::eventFilter(obj, event);

       }else if (obj == (QObject*)ui->wr_addplayer3mousearea){
           if (event->type() == QEvent::Enter)
           {
               ui->wr_addplayer3->setVisible(false);
               ui->wr_addplayer3sel->setVisible(true);

           }else if (event->type() == QEvent::Leave) {
               ui->wr_addplayer3->setVisible(true);
               ui->wr_addplayer3sel->setVisible(false);
           }else if (event->type() == QEvent::MouseButtonRelease)
           {
               ui->wr_addplayer3->setVisible(false);
               ui->wr_addplayer3sel->setVisible(false);
               ui->wr_addplayer3mousearea->removeEventFilter(this);
               ui->wr_addplayer3mousearea->setVisible(false);
               ui->wr_playername3->setVisible(true);
               ui->wr_playername3->setEnabled(1);
               ui->wr_playername3->setFocus();
               ui->wr_playername3->selectAll();
           }

           return QWidget::eventFilter(obj, event);

       }else {
        // pass the event on to the parent class
        return QWidget::eventFilter(obj, event);
    }
}






void WR_GAME_UI::on_flag_uk_clicked()
{
    emit s_setLanguage(Language_English);
}

void WR_GAME_UI::on_flag_sweden_clicked()
{
    emit s_setLanguage(Language_Swedish);
}

void WR_GAME_UI::on_flag_china_clicked()
{
    emit s_setLanguage(Language_Chinese);
}

void WR_GAME_UI::on_wr_playername1_returnPressed()
{
    QString temp = ui->wr_playername1->text();
    emit s_addPlayer(temp);
    ui->wr_playername1->setDisabled(1);
    ui->wr_cash_1->setVisible(true);
    ui->wr_cash_1_dollarsign->setVisible(true);
    ui->wr_bet_1->setVisible(true);
    ui->wr_bet_1_dollarsign->setVisible(true);
    ui->wr_betw_1->setVisible(true);
    if (startDisabled == false) {
        enableInput();
    }
    ui->wr_addplayer2->setVisible(true);
    ui->wr_addplayer2mousearea->installEventFilter(this);
    ui->wr_bet_1_enterbettext->setVisible(true);
    ui->wr_betw_1_enterlanetext->setVisible(true);
}

void WR_GAME_UI::on_wr_playername2_returnPressed()
{
    QString temp = ui->wr_playername2->text();
     emit s_addPlayer(temp);
    ui->wr_playername2->setDisabled(1);
    ui->wr_cash_2->setVisible(true);
    ui->wr_cash_2_dollarsign->setVisible(true);
    ui->wr_bet_2->setVisible(true);
    ui->wr_bet_2_dollarsign->setVisible(true);
    ui->wr_betw_2->setVisible(true);
    if (startDisabled == false) {
        enableInput();
    }
    ui->wr_addplayer3->setVisible(true);
    ui->wr_addplayer3mousearea->installEventFilter(this);
    ui->wr_bet_2_enterbettext->setVisible(true);
    ui->wr_betw_2_enterlanetext->setVisible(true);
}

void WR_GAME_UI::on_wr_playername3_returnPressed()
{
    QString temp = ui->wr_playername3->text();
     emit s_addPlayer(temp);
    ui->wr_playername3->setDisabled(1);
    ui->wr_cash_3->setVisible(true);
    ui->wr_cash_3_dollarsign->setVisible(true);
    ui->wr_bet_3->setVisible(true);
    ui->wr_bet_3_dollarsign->setVisible(true);
    ui->wr_betw_3->setVisible(true);
    if (startDisabled == false) {
        enableInput();
    }
    ui->wr_bet_3_enterbettext->setVisible(true);
    ui->wr_betw_3_enterlanetext->setVisible(true);
}



void WR_GAME_UI::on_wr_bet_1_textEdited(const QString &arg1)
{
    ui->wr_bet_1_enterbettext->setVisible(false);
    disconnect(ui->wr_bet_1, SIGNAL(textEdited(QString)),this,SLOT(on_wr_bet_1_textEdited(QString)));
}

void WR_GAME_UI::on_wr_bet_2_textEdited(const QString &arg1)
{
    ui->wr_bet_2_enterbettext->setVisible(false);
    disconnect(ui->wr_bet_2, SIGNAL(textEdited(QString)),this,SLOT(on_wr_bet_2_textEdited(QString)));
}

void WR_GAME_UI::on_wr_bet_3_textEdited(const QString &arg1)
{
    ui->wr_bet_3_enterbettext->setVisible(false);
    disconnect(ui->wr_bet_3, SIGNAL(textEdited(QString)),this,SLOT(on_wr_bet_3_textEdited(QString)));
}

void WR_GAME_UI::on_wr_betw_1_textEdited(const QString &arg1)
{
    ui->wr_betw_1_enterlanetext->setVisible(false);
    disconnect(ui->wr_betw_1, SIGNAL(textEdited(QString)),this,SLOT(on_wr_betw_1_textEdited(QString)));
}

void WR_GAME_UI::on_wr_betw_2_textEdited(const QString &arg1)
{
    ui->wr_betw_2_enterlanetext->setVisible(false);
    disconnect(ui->wr_betw_2, SIGNAL(textEdited(QString)),this,SLOT(on_wr_betw_2_textEdited(QString)));
}

void WR_GAME_UI::on_wr_betw_3_textEdited(const QString &arg1)
{
    ui->wr_betw_3_enterlanetext->setVisible(false);
    disconnect(ui->wr_betw_3, SIGNAL(textEdited(QString)),this,SLOT(on_wr_betw_3_textEdited(QString)));
}


