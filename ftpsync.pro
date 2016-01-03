#-------------------------------------------------
#
# Project created by QtCreator 2015-12-28T17:44:52
#
#-------------------------------------------------

QT       -= core gui

CONFIG   += console c++11
CONFIG   -= app_bundle

                             # to disable some qtlib warnings
#QMAKE_CXXFLAGS += -std=c++11         -lPocoNet -lPocoFoundation
QMAKE_CXXFLAGS += -std=c++11

LIBS += -lPocoNet -lPocoFoundation

TARGET = ftpsync
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    FtpMirrorPoco.cpp \
    ftpparse.c

HEADERS += \
    FtpMirrorPoco.h \
    FtpData.h \
    ftpparse.h
