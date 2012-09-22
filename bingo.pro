#-------------------------------------------------
#
# Project created by QtCreator 2012-06-14T14:06:25
#
#-------------------------------------------------

QT       += core gui declarative

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = bingo
TEMPLATE = app

INCLUDEPATH += ./3rdparty/

SOURCES += main.cpp\
        mainwindow.cpp \
    cardgenerator.cpp \
    bingowindow.cpp

HEADERS  += mainwindow.h \
    cardgenerator.h \
    bingowindow.h

FORMS    += mainwindow.ui \
    cardsdialog.ui \
    bingowindow.ui

OTHER_FILES += \
    mainview.qml \
    Front.qml \
    Countdown.qml

RESOURCES += \
    bingo.qrc
