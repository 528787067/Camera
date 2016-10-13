#-------------------------------------------------
#
# Project created by QtCreator 2016-06-30T00:56:06
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Camera
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

include($(QT_DIR)/config/x8.config)
