CONFIG += c++1z
CONFIG += debug_and_release

CONFIG(debug, debug|release) {
	TARGET = qtwidgetsextrad
	MOC_DIR	    = tmp/debug_shared/moc
	OBJECTS_DIR = tmp/debug_shared/obj
	RCC_DIR     = tmp/debug_shared/rcc
} else {
	TARGET = qtwidgetsextra
	MOC_DIR	    = tmp/release_shared/moc
	OBJECTS_DIR = tmp/release_shared/obj
	RCC_DIR     = tmp/release_shared/rcc
}

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATE    = lib
DESTDIR     = ../libs
DLLDESTDIR  = ../bin
INCLUDEPATH = . include
DEPENDPATH  = . src include

DEFINES += QTWIDGETSEXTRA_MAKEDLL QTWIDGETSEXTRA_BUILD_LIB
#win32-msvc*|win32-icc:QMAKE_LFLAGS += /BASE:0x67000000


include(qtwidgetsextra.pri)

