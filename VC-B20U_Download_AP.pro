#-------------------------------------------------
#
# Project created by QtCreator 2014-12-19T09:12:53
#
#-------------------------------------------------

QT       += core gui
QT       += widgets serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = VC-B20U_Download_AP
TEMPLATE = app


SOURCES += main.cpp\
        maindialog.cpp \
    masterthread.cpp \
    masterchip.cpp \
    serial.cpp \
    s19.cpp

HEADERS  += maindialog.h \
    masterthread.h \
    datatype.h \
    masterchip.h \
    serial.h \
    hc08sprg.h

FORMS    += maindialog.ui

CONFIG += console
