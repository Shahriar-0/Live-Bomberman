QT       += core gui widgets
QT       += network

win32:CONFIG += console

CONFIG   += c++17

# Source files
SOURCES += \
    src/animation.cpp \
    src/block.cpp \
    src/bomb.cpp \
    src/breakableblock.cpp \
    src/explosioneffect.cpp \
    src/game.cpp \
    src/gameNetworkManager.cpp \
    src/gameobject.cpp \
    src/gameview.cpp \
    src/hud.cpp \
    src/main.cpp \
    src/maploader.cpp \
    src/networkmanager.cpp \
    src/player.cpp \
    src/settingsdialog.cpp \
    src/tcpmanager.cpp \
    src/udpmanager.cpp

# Header files
HEADERS += \
    include/animation.h \
    include/block.h \
    include/bomb.h \
    include/breakableblock.h \
    include/explosioneffect.h \
    include/game.h \
    include/gameNetworkManager.h \
    include/gameobject.h \
    include/gameview.h \
    include/hud.h \
    include/maploader.h \
    include/player.h \
    include/settingsdialog.h \
    include/networkmanager.h \
    include/tcpmanager.h \
    include/udpmanager.h \

# Resource file
RESOURCES += \
    resources/resources.qrc

# Default deployment rules
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
