CONFIG += debug_and_release

CONFIG(debug, debug|release) {
	TARGET = qtcoreextrad
	MOC_DIR	    = tmp/debug_shared/moc
	OBJECTS_DIR = tmp/debug_shared/obj
	RCC_DIR     = tmp/debug_shared/rcc
} else {
	TARGET = qtcoreextra
	MOC_DIR	    = tmp/release_shared/moc
	OBJECTS_DIR = tmp/release_shared/obj
	RCC_DIR     = tmp/release_shared/rcc
}

TEMPLATE    = lib
DESTDIR     = ../libs
DLLDESTDIR  = ../bin
INCLUDEPATH = . include
DEPENDPATH  = . src include

DEFINES += QTCOREEXTRA_MAKEDLL QTCOREEXTRA_BUILD_LIB
#win32-msvc*|win32-icc:QMAKE_LFLAGS += /BASE:0x67000000

include(qtcoreextra.pri)

