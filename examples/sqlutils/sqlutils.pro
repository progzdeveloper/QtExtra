QT += core sql
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

TARGET = sqlutils
TEMPLATE = app

CONFIG += debug_and_release

CONFIG(debug, debug|release) {
        TARGET = sqlutilsd
        MOC_DIR	    = tmp/debug_shared/moc
        OBJECTS_DIR = tmp/debug_shared/obj
        RCC_DIR     = tmp/debug_shared/rcc
        LIBS += -L../../libs -lqtcoreextrad -lqtsqlextrad -lqtsqlwidgetsd -lqtwidgetsextrad
} else {
        TARGET = sqlutils
        MOC_DIR	    = tmp/release_shared/moc
        OBJECTS_DIR = tmp/release_shared/obj
        RCC_DIR     = tmp/release_shared/rcc
        LIBS += -L../../libs -lqtcoreextra -lqtsqlextra -lqtsqlwidgets -lqtwidgetsextra
}

SOURCES += main.cpp \
    qtsqlitembrowser.cpp

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

DEFINES += QTSQLEXTRA_DLL QTSQLWIDGETS_DLL QTWIDGETSEXTRA_DLL

DESTDIR     = ../bin
INCLUDEPATH += \
    ../../qtcoreextra/include \
    ../../qtsqlextra/include \
    ../../qtsqlwidgets/include \
    ../../qtwidgetsextra/include

DEPENDPATH += \
    ../../qtcoreextra/include \
    ../../qtsqlextra/include \
    ../../qtsqlwidgets/include \
    ../../qtwidgetsextra/include

HEADERS += \
    qtsqlitembrowser.h

RESOURCES += \
    sqlutils.qrc

