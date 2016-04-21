#-------------------------------------------------
#
# Project created by QtCreator 2016-01-21T20:56:32
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ipmsg
TEMPLATE = app


SOURCES += main.cpp\
        maindialog.cpp \
    classicmaindialog.cpp \
    chatdialog.cpp

HEADERS  += maindialog.h \
    classicmaindialog.h \
    chatdialog.h

FORMS    += maindialog.ui \
    classicmaindialog.ui \
    chatdialog.ui

RESOURCES += \
    res.qrc
