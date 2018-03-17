TEMPLATE = lib

QT -= gui
QT += datasync

TARGET = remind-me

DEFINES += REMINDMELIB_LIBRARY

HEADERS += \
	remindmelib_global.h \
	dateparser.h \
	reminder.h \
	schedule.h \
	conflictresolver.h \
    remindmelib.h \
    snoozetimes.h

SOURCES += \
	dateparser.cpp \
	reminder.cpp \
	schedule.cpp \
	conflictresolver.cpp \
    remindmelib.cpp \
    snoozetimes.cpp
