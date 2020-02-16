CONFIG += debug_and_release

CONFIG(debug, debug|release) {
	TARGET = qtpropertybrowserd
	MOC_DIR	    = tmp/debug_shared/moc
	OBJECTS_DIR = tmp/debug_shared/obj
	RCC_DIR     = tmp/debug_shared/rcc
} else {
	TARGET = qtpropertybrowser
	MOC_DIR	    = tmp/release_shared/moc
	OBJECTS_DIR = tmp/release_shared/obj
	RCC_DIR     = tmp/release_shared/rcc
}

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATE    = lib
DESTDIR     = ../libs
DLLDESTDIR  = ../bin
INCLUDEPATH = . include
win32-msvc*|win32-icc:DEFINES += QT_QTPROPERTYBROWSER_EXPORT
win32-msvc*|win32-icc:QMAKE_LFLAGS += /BASE:0x67000000

include(qtpropertybrowser.pri)

