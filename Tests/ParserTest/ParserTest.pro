TEMPLATE = app

QT += testlib
CONFIG += console
CONFIG -= app_bundle

TARGET = tst_parser

SOURCES += \
    tst_parser.cpp

DEFINES += SRCDIR=\\\"$$PWD/\\\"

include(../../lib.pri)
