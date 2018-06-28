#ifndef CONSOLE_H
#define CONSOLE_H

#include <QWidget>
#include "../../gui.h"
#include "../../opengl.h"
#include "../../controller.h"

class GUI;
class OpenGL;
class Controller;

namespace Ui {
class Console;
}

class Console : public QWidget
{
    Q_OBJECT

public:
    explicit Console(QWidget *parent = 0);
    ~Console();
    bool eventFilter(QObject *obj, QEvent *event);
    GUI *gui;
    Controller *controller; 
    OpenGL *openGL;
    QPoint oldCursorPos;
    bool stopped;
    void print(QString string);
    void sizeChanged();
    void toggleConsole();
    bool isConsoleLineSelected();

private slots:
    void on_consoleLine_returnPressed();

private:
    Ui::Console *ui;
    int blockLimit;
    double pct; //Window height, % of mainWindow height
    void dragging();
};

#endif // CONSOLE_H
