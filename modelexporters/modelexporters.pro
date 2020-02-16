TEMPLATE = subdirs

SUBDIRS += \
    csvexporter \
    jsonexporter \
    htmlexporter \
    xmlexporter

win32 {
    SUBDIRS += excelexporter
}

