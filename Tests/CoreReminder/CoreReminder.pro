TEMPLATE = app

QT += testlib
CONFIG += console
CONFIG -= app_bundle

TARGET = tst_coreremindertest

SOURCES += \
		tst_coreremindertest.cpp

DEFINES += SRCDIR=\\\"$$PWD/\\\"

include(../../lib.pri)
