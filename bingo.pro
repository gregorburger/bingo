#-------------------------------------------------
#
# Project created by QtCreator 2012-06-14T14:06:25
#
#-------------------------------------------------

QT       += core gui declarative

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = bingo
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

OTHER_FILES += \
    mainview.qml \
    Front.qml \
    Countdown.qml

RESOURCES += \
    bingo.qrc
