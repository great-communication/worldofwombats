#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QApplication>
#include <QFontDatabase>
#include <QSurfaceFormat>

int main(int argc, char *argv[])
{
    //Q_INIT_RESOURCE(textures);
    QApplication a(argc, argv);

    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    format.setSwapInterval(1);
    QSurfaceFormat::setDefaultFormat(format);

    QFontDatabase::addApplicationFont(":/Fonts/Fonts/LifeCraft_Font.ttf");

    qApp->setFont(QFont("LifeCraft_Font", 11, QFont::Normal, false));

    MainWindow w;
    w.show();
    //w.showFullScreen();

    return a.exec();
}
