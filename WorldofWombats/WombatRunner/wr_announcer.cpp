#include "wr_announcer.h"

WR_Announcer::WR_Announcer(QObject *parent) : QObject(parent)
{
    //Set default language
    language = Language_English;

}

void WR_Announcer::setLanguage(Language selection){

    if (selection != language) {
        language = selection;
        emit s_resetMessageBoard();
        sendMessage(Message_Welcome,"",0,0);
    }
}

void WR_Announcer::sendMessage (Message message, QString pstring, int pwombat, int pamount){

    QString output;    
    QString name = pstring;
    QString wombat = QString::number(pwombat);
    QString amount = QString::number(pamount);

    switch (language)
        {
            case Language_English:
                switch (message){
                    case Message_Welcome: output = "Welcome to the International Arena of Wombats Rapid Advance! Here proud wombats compete for honour and glory!"; break;
                    case Message_WelcomeNewPlayer: output = "Welcome "+name+"!";                                                                                    break;
                    case Message_NoBets: output = "No bets made.";                                                                                                  break;
                    case Message_ABetHasBeenMade: output = name+" bets $"+amount+" on wombat nr "+wombat+".";                                                       break;
                    case Message_StartingRace: output = "Now they are jokingly jumping!";                                                                           break;
                    case Message_RaceWinner: output = "Wombat nr "+wombat+" wins the competition!";                                                                 break;
                    case Message_TwoRaceWinners: output = "Wombat nr "+wombat+" and nr "+amount+" complete tie!";                                                   break;
                    case Message_ThreeRaceWinners: output = "Amazing! All three wombats at the same time through the finish line! This is unheard of!";             break;
                    case Message_WonMoney: output = name+" wins $"+amount+"!";                                                                                      break;
                    case Message_IsBroke: output = name+" bankrupted! Go home!";                                                                                    break;
                } break;
            case Language_Swedish:
                switch (message){
                    case Message_Welcome: output = "Välkommen till Internationella arenan för vombaters snabba framfart! Här tävlar stolta vombater om ära och stolthet!";  break;
                    case Message_WelcomeNewPlayer: output = "Välkommen " +name+"!";                                                                                         break;
                    case Message_NoBets: output = "Ingen bet.";                                                                                                             break;
                    case Message_ABetHasBeenMade: output = name+" bet $"+amount+" på vombat nr "+wombat+".";                                                                break;
                    case Message_StartingRace: output = "Nu skämt hoppar dom!";                                                                                             break;
                    case Message_RaceWinner: output = "Vombat nr "+wombat+" vinner tävlingen!";                                                                             break;
                    case Message_TwoRaceWinners: output = "Vombat nr "+wombat+" och nr "+amount+" slutar i en slips!";                                                      break;
                    case Message_ThreeRaceWinners: output = "Fantastiskt! Alla tre vombater samtidigt genom mållinjen! Detta är oerhört!";                                  break;
                    case Message_WonMoney: output = name+" vinner $"+amount+"!";                                                                                            break;
                    case Message_IsBroke: output = name+" konkurs! Gå hem!";                                                                                                break;
                } break;
            case Language_Chinese:
                switch (message){
                    case Message_Welcome: output = "欢迎来到Wombat Racing国际竞技场，骄傲的熊熊争夺荣誉与荣耀！";      break;
                    case Message_WelcomeNewPlayer: output = "欢迎" +name+"！";                                 break;
                    case Message_NoBets: output = "没有下注。";                                                  break;
                    case Message_ABetHasBeenMade: output = name+" 咬 $"+amount+" 在 袋熊 号码 "+wombat+".";       break;
                    case Message_StartingRace: output = "现在他们开玩笑跳！";                                   break;
                    case Message_RaceWinner: output = "袋熊 号码 "+wombat+" 赢得比赛！";                             break;
                    case Message_TwoRaceWinners: output = "袋熊 号码 "+wombat+" 和 号码 "+amount+" 完成领带！";         break;
                    case Message_ThreeRaceWinners: output = "惊人！所有三条熊都在同一时间穿过终点线！这是闻所未闻！";      break;
                    case Message_WonMoney: output = name+" 赢得 $"+amount+"！";                                    break;
                    case Message_IsBroke: output = name+" 破产了！ 回家！";                                            break;
                } break;


            default:
                output = "";
            break;
        }

    emit s_updateMessageBoard(output);


}





