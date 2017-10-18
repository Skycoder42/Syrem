#-------------------------------------------------
#
# Project created by QtCreator 2017-10-18T14:11:08
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = tst_coreremindertest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

SOURCES += \
		tst_coreremindertest.cpp

DEFINES += SRCDIR=\\\"$$PWD/\\\"

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../RemindMeCore/release/ -lRemindMeCore
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../RemindMeCore/debug/ -lRemindMeCore
else:unix: LIBS += -L$$OUT_PWD/../../RemindMeCore/ -lRemindMeCore

INCLUDEPATH += $$PWD/../../RemindMeCore
DEPENDPATH += $$PWD/../../RemindMeCore

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../RemindMeCore/release/libRemindMeCore.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../RemindMeCore/debug/libRemindMeCore.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../RemindMeCore/release/RemindMeCore.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../RemindMeCore/debug/RemindMeCore.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../../RemindMeCore/libRemindMeCore.a
