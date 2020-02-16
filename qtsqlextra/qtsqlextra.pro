#-------------------------------------------------
#
# Project created by QtCreator 2018-07-29T15:05:35
#
#-------------------------------------------------

QT       += sql
QT       -= gui

TARGET = qtsqlextra

CONFIG += debug_and_release

CONFIG(debug, debug|release) {
        TARGET = qtsqlextrad
        MOC_DIR	    = tmp/debug_shared/moc
        OBJECTS_DIR = tmp/debug_shared/obj
        RCC_DIR     = tmp/debug_shared/rcc
} else {
        TARGET = qtsqlextra
        MOC_DIR	    = tmp/release_shared/moc
        OBJECTS_DIR = tmp/release_shared/obj
        RCC_DIR     = tmp/release_shared/rcc
}

TEMPLATE    = lib
DESTDIR     = ../libs
DLLDESTDIR     = ../bin
INCLUDEPATH = . include
DEPENDPATH = . src include

LIBS += -lodbc

DEFINES += QTSQLEXTRA_MAKEDLL QTSQLEXTRA_BUILD_LIB

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS


# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

include(qtsqlextra.pri)



#unix {
#    target.path = /usr/lib
#    INSTALLS += target
#}

