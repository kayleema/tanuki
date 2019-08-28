#-------------------------------------------------
#
# Project created by QtCreator 2019-07-30T20:18:52
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qtpinpon
TEMPLATE = app

ICON = pinpon.icns

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        main.cpp \
        mainwindow.cpp

HEADERS += \
        mainwindow.h

FORMS += \
        mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../cmake-build-debug/release/ -lpinpon_lib
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../cmake-build-debug/debug/ -lpinpon_lib
else:unix: LIBS += -L$$PWD/../cmake-build-debug/ -lpinpon_lib

INCLUDEPATH += $$PWD/../src/
DEPENDPATH += $$PWD/../src/

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../cmake-build-debug/release/libpinpon_lib.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../cmake-build-debug/debug/libpinpon_lib.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../cmake-build-debug/release/pinpon_lib.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../cmake-build-debug/debug/pinpon_lib.lib
else:unix: PRE_TARGETDEPS += $$PWD/../cmake-build-debug/libpinpon_lib.a
