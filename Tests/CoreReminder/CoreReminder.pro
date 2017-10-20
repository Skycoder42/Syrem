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

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../RemindMeDaemon/release/ -lRemindMeDaemon
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../RemindMeDaemon/debug/ -lRemindMeDaemon
else:unix: LIBS += -L$$OUT_PWD/../../RemindMeDaemon/ -lRemindMeDaemon

INCLUDEPATH += $$PWD/../../RemindMeDaemon
DEPENDPATH += $$PWD/../../RemindMeDaemon

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../RemindMeDaemon/release/libRemindMeDaemon.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../RemindMeDaemon/debug/libRemindMeDaemon.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../RemindMeDaemon/release/RemindMeDaemon.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../RemindMeDaemon/debug/RemindMeDaemon.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../../RemindMeDaemon/libRemindMeDaemon.a
