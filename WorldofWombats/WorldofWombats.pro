#-------------------------------------------------
#
# Project created by QtCreator 2017-01-05T14:07:43
#
#-------------------------------------------------

QT       += core gui multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = WorldofWombats
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
        mainwindow.cpp \
    WombatRunner/wr_player.cpp \
    WombatRunner/wr_game.cpp \
    WombatRunner/wr_announcer.cpp \
    WombatRunner/wr_highscore.cpp \
    WombatRunner/wr_game_ui.cpp \
    HabitatWombat/libnoise/latlon.cpp \
    HabitatWombat/libnoise/noisegen.cpp \
    HabitatWombat/libnoise/model/cylinder.cpp \
    HabitatWombat/libnoise/model/line.cpp \
    HabitatWombat/libnoise/model/plane.cpp \
    HabitatWombat/libnoise/model/sphere.cpp \
    HabitatWombat/libnoise/module/voronoi.cpp \
    HabitatWombat/libnoise/module/turbulence.cpp \
    HabitatWombat/libnoise/module/translatepoint.cpp \
    HabitatWombat/libnoise/module/terrace.cpp \
    HabitatWombat/libnoise/module/spheres.cpp \
    HabitatWombat/libnoise/module/select.cpp \
    HabitatWombat/libnoise/module/scalepoint.cpp \
    HabitatWombat/libnoise/module/scalebias.cpp \
    HabitatWombat/libnoise/module/rotatepoint.cpp \
    HabitatWombat/libnoise/module/ridgedmulti.cpp \
    HabitatWombat/libnoise/module/power.cpp \
    HabitatWombat/libnoise/module/perlin.cpp \
    HabitatWombat/libnoise/module/multiply.cpp \
    HabitatWombat/libnoise/module/modulebase.cpp \
    HabitatWombat/libnoise/module/min.cpp \
    HabitatWombat/libnoise/module/max.cpp \
    HabitatWombat/libnoise/module/invert.cpp \
    HabitatWombat/libnoise/module/exponent.cpp \
    HabitatWombat/libnoise/module/displace.cpp \
    HabitatWombat/libnoise/module/cylinders.cpp \
    HabitatWombat/libnoise/module/curve.cpp \
    HabitatWombat/libnoise/module/const.cpp \
    HabitatWombat/libnoise/module/clamp.cpp \
    HabitatWombat/libnoise/module/checkerboard.cpp \
    HabitatWombat/libnoise/module/cache.cpp \
    HabitatWombat/libnoise/module/blend.cpp \
    HabitatWombat/libnoise/module/billow.cpp \
    HabitatWombat/libnoise/module/add.cpp \
    HabitatWombat/libnoise/module/abs.cpp \
    HabitatWombat/libnoise/noiseutils.cpp \
    HabitatWombat/Widgets/Menu/newworlddialog.cpp \
    HabitatWombat/Widgets/Menu/loadworlddialog.cpp \
    HabitatWombat/Widgets/Game/infobox.cpp \
    HabitatWombat/Widgets/Game/burrowlist.cpp \
    HabitatWombat/Widgets/Game/deathnotice.cpp \
    HabitatWombat/Widgets/Game/createburrow.cpp \
    HabitatWombat/Widgets/Game/ingamemenu.cpp \
    HabitatWombat/Widgets/Game/console.cpp \
    HabitatWombat/Widgets/Game/burrowmenu.cpp \
    HabitatWombat/Widgets/Game/storagewidget.cpp \
    HabitatWombat/Widgets/Game/winventorywidget.cpp \
    HabitatWombat/Widgets/Game/amountwidget.cpp \
    HabitatWombat/worker.cpp \
    HabitatWombat/controller.cpp \
    HabitatWombat/worldbuilder.cpp \
    HabitatWombat/global.cpp \
    HabitatWombat/sound.cpp \
    HabitatWombat/registry.cpp \
    HabitatWombat/herder.cpp \
    HabitatWombat/herdmember.cpp \
    HabitatWombat/gui.cpp \
    HabitatWombat/opengl.cpp \
    HabitatWombat/menu_ui.cpp \
    HabitatWombat/Widgets/Game/deleteitemwidget.cpp \
    HabitatWombat/Widgets/Game/progresswidget.cpp \
    HabitatWombat/level.cpp \
    HabitatWombat/world.cpp \
    HabitatWombat/worldgenerator.cpp \
    HabitatWombat/imagepainter.cpp \
    HabitatWombat/imageloader.cpp

HEADERS  += mainwindow.h \
    WombatRunner/wr_player.h \
    WombatRunner/wr_game.h \
    WombatRunner/wr_announcer.h \
    WombatRunner/wr_highscore.h \
    WombatRunner/wr_game_ui.h \
    HabitatWombat/libnoise/basictypes.h \
    HabitatWombat/libnoise/exception.h \
    HabitatWombat/libnoise/interp.h \
    HabitatWombat/libnoise/latlon.h \
    HabitatWombat/libnoise/mathconsts.h \
    HabitatWombat/libnoise/misc.h \
    HabitatWombat/libnoise/noise.h \
    HabitatWombat/libnoise/noisegen.h \
    HabitatWombat/libnoise/vectortable.h \
    HabitatWombat/libnoise/model/cylinder.h \
    HabitatWombat/libnoise/model/line.h \
    HabitatWombat/libnoise/model/model.h \
    HabitatWombat/libnoise/model/plane.h \
    HabitatWombat/libnoise/model/sphere.h \
    HabitatWombat/libnoise/module/abs.h \
    HabitatWombat/libnoise/module/add.h \
    HabitatWombat/libnoise/module/billow.h \
    HabitatWombat/libnoise/module/blend.h \
    HabitatWombat/libnoise/module/cache.h \
    HabitatWombat/libnoise/module/checkerboard.h \
    HabitatWombat/libnoise/module/clamp.h \
    HabitatWombat/libnoise/module/const.h \
    HabitatWombat/libnoise/module/curve.h \
    HabitatWombat/libnoise/module/cylinders.h \
    HabitatWombat/libnoise/module/displace.h \
    HabitatWombat/libnoise/module/exponent.h \
    HabitatWombat/libnoise/module/invert.h \
    HabitatWombat/libnoise/module/max.h \
    HabitatWombat/libnoise/module/min.h \
    HabitatWombat/libnoise/module/module.h \
    HabitatWombat/libnoise/module/modulebase.h \
    HabitatWombat/libnoise/module/multiply.h \
    HabitatWombat/libnoise/module/perlin.h \
    HabitatWombat/libnoise/module/power.h \
    HabitatWombat/libnoise/module/ridgedmulti.h \
    HabitatWombat/libnoise/module/rotatepoint.h \
    HabitatWombat/libnoise/module/scalebias.h \
    HabitatWombat/libnoise/module/scalepoint.h \
    HabitatWombat/libnoise/module/select.h \
    HabitatWombat/libnoise/module/spheres.h \
    HabitatWombat/libnoise/module/terrace.h \
    HabitatWombat/libnoise/module/translatepoint.h \
    HabitatWombat/libnoise/module/turbulence.h \
    HabitatWombat/libnoise/module/voronoi.h \
    HabitatWombat/libnoise/noiseutils.h \
    HabitatWombat/Widgets/Menu/newworlddialog.h \
    HabitatWombat/Widgets/Menu/loadworlddialog.h \
    HabitatWombat/Widgets/Game/infobox.h \
    HabitatWombat/Widgets/Game/burrowlist.h \
    HabitatWombat/Widgets/Game/deathnotice.h \
    HabitatWombat/Widgets/Game/createburrow.h \
    HabitatWombat/Widgets/Game/ingamemenu.h \
    HabitatWombat/Widgets/Game/console.h \
    HabitatWombat/Widgets/Game/burrowmenu.h \
    HabitatWombat/Widgets/Game/storagewidget.h \
    HabitatWombat/Widgets/Game/winventorywidget.h \
    HabitatWombat/Widgets/Game/amountwidget.h \
    HabitatWombat/worker.h \
    HabitatWombat/controller.h \
    HabitatWombat/worldbuilder.h \
    HabitatWombat/global.h \
    HabitatWombat/sound.h \
    HabitatWombat/registry.h \
    HabitatWombat/herder.h \
    HabitatWombat/herdmember.h \
    HabitatWombat/gui.h \
    HabitatWombat/opengl.h \
    HabitatWombat/menu_ui.h \
    HabitatWombat/Widgets/Game/deleteitemwidget.h \
    HabitatWombat/Widgets/Game/progresswidget.h \
    HabitatWombat/level.h \
    HabitatWombat/world.h \
    HabitatWombat/worldgenerator.h \
    HabitatWombat/imagepainter.h \
    HabitatWombat/imageloader.h

FORMS    += mainwindow.ui \
    WombatRunner/wr_game_ui.ui \
    HabitatWombat/Widgets/Menu/newworlddialog.ui \
    HabitatWombat/Widgets/Menu/loadworlddialog.ui \
    HabitatWombat/Widgets/Game/infobox.ui \
    HabitatWombat/Widgets/Game/burrowlist.ui \
    HabitatWombat/Widgets/Game/deathnotice.ui \
    HabitatWombat/Widgets/Game/createburrow.ui \
    HabitatWombat/Widgets/Game/ingamemenu.ui \
    HabitatWombat/Widgets/Game/console.ui \
    HabitatWombat/Widgets/Game/burrowmenu.ui \
    HabitatWombat/Widgets/Game/storagewidget.ui \
    HabitatWombat/Widgets/Game/winventorywidget.ui \
    HabitatWombat/Widgets/Game/amountwidget.ui \
    HabitatWombat/gui.ui \
    HabitatWombat/menu_ui.ui \
    HabitatWombat/Widgets/Game/deleteitemwidget.ui \
    HabitatWombat/Widgets/Game/progresswidget.ui

RESOURCES += \
    resources.qrc
