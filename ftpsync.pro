#-------------------------------------------------
#
# Project created by QtCreator 2015-12-28T17:44:52
#
#-------------------------------------------------

QT       += core network

QT       -= gui
                  # to disable some qtlib warnings
QMAKE_CXXFLAGS += -std=c++11 -Wno-deprecated-register

LIBS += -lboost_filesystem -lboost_system

TARGET = ftpsync

CONFIG   += c++11

TEMPLATE = app


SOURCES += main.cpp \
    FtpMirror.cpp \
    Utilities.cpp \
    FtpClient.cpp

HEADERS += \
    FtpMirror.h \
    Utilities.h \
    FtpClient.h
