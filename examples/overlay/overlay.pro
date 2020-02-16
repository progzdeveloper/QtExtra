#-------------------------------------------------
#
# Project created by QtCreator 2019-06-30T22:48:32
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATE = app

CONFIG += debug_and_release

CONFIG(debug, debug|release) {
        TARGET = overlayd
        MOC_DIR	    = tmp/debug_shared/moc
        OBJECTS_DIR = tmp/debug_shared/obj
        RCC_DIR     = tmp/debug_shared/rcc
        LIBS += -L../../libs -lqtwidgetsextrad
} else {
        TARGET = overlay
        MOC_DIR	    = tmp/release_shared/moc
        OBJECTS_DIR = tmp/release_shared/obj
        RCC_DIR     = tmp/release_shared/rcc
        LIBS += -L../../libs -lqtwidgetsextra
}

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS QTWIDGETSEXTRA_DLL

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

DESTDIR     = ../bin
INCLUDEPATH += \
    ../../qtwidgetsextra/include

DEPENDPATH += \
    ../../qtwidgetsextra/include

SOURCES += \
        main.cpp \
        widget.cpp

HEADERS += \
        widget.h
