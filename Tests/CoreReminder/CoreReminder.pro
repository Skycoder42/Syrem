QT       += testlib

TARGET = tst_coreremindertest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

SOURCES += \
		tst_coreremindertest.cpp

DEFINES += SRCDIR=\\\"$$PWD/\\\"

include(../../lib.pri)
