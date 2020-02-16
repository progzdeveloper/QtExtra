CONFIG += debug_and_release

CONFIG(debug, debug|release) {
	TARGET = qtpluginsd
	MOC_DIR	    = tmp/debug_shared/moc
	OBJECTS_DIR = tmp/debug_shared/obj
	RCC_DIR     = tmp/debug_shared/rcc
} else {
	TARGET = qtplugins
	MOC_DIR	    = tmp/release_shared/moc
	OBJECTS_DIR = tmp/release_shared/obj
	RCC_DIR     = tmp/release_shared/rcc
}
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
TEMPLATE    = lib
DESTDIR     = ../libs
DLLDESTDIR     = ../bin
INCLUDEPATH = . include
DEPENDPATH = . src include

DEFINES += QTPLUGINS_MAKEDLL QTPLUGINS_BUILD_LIB
#win32-msvc*|win32-icc:QMAKE_LFLAGS += /BASE:0x67000000

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

include(qtplugins.pri)
