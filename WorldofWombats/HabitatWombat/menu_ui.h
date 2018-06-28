//The menu, responsible for loading/creating new games etc.
#ifndef MENU_UI_H
#define MENU_UI_H

#include <QWidget>
#include "Widgets/Menu/newworlddialog.h"
#include "Widgets/Menu/loadworlddialog.h"
#include <QLabel>
#include "gui.h"

class NewWorldDialog;
class loadWorldDialog;

namespace Ui {
class Menu_ui;
}

class Menu_ui : public QWidget
{
    Q_OBJECT

public:
    explicit Menu_ui(QWidget *parent = 0);
    ~Menu_ui();
    GUI *gui;
    bool guiLoaded;
    bool eventFilter(QObject *obj, QEvent *event);    
    void setup();
    void newWorldDialogOkClicked();
    void newWorldDialogCancelClicked();
    void loadWorldDialogOkClicked();
    void loadWorldDialogCancelClicked();

public slots:
    void backToHWMenu();
    void getMainWindowState(); //Retarded

signals:
    void s_backToMainMenu();
    void s_playSoundFX(int index);
    void s_getMainWindowState(); //Retarded    

private:
    Ui::Menu_ui *ui;
    QVector<QString> levels;
    bool dialogOpen;
    NewWorldDialog *newWorldDialog;
    loadWorldDialog *lloadWorldDialog;
    QPoint newWorldDialogOffset;
    QPoint loadWorldDialogOffset;
    QPoint cursorPos;
    bool movingWindow;
    void showNewWorldDialog();
    void showLoadWorldDialog();
    void moveWidget(QWidget *widget,QPoint offset);
    void readLevelsFile(); // Levels contain a list of all levels
    void saveLevelsFile();
    bool fileExists(QString path);    
};

#endif // MENU_UI_H
