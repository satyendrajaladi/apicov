#-------------------------------------------------
#
# Project created by QtCreator 2010-12-02T17:58:51
#
#-------------------------------------------------

QT       += core gui

TARGET = ApiCovTool
TEMPLATE = app


SOURCES += main.cpp\
        apicovtool.cpp \
    commentextractor.cpp \
    headerparser.cpp \
    apitemplate.cpp \
    mappingfilecreation.cpp

HEADERS  += apicovtool.h \
    commentextractor.h \
    headerparser.h \
    apitemplate.h \
    mappingfilecreation.h

FORMS    += apicovtool.ui
