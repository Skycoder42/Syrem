TEMPLATE = app

QT += testlib mvvmcore datasync
CONFIG += console
CONFIG -= app_bundle

TARGET = tst_coreremindertest

SOURCES += \
		tst_coreremindertest.cpp

DEFINES += SRCDIR=\\\"$$PWD/\\\"

include(../../lib.pri)
