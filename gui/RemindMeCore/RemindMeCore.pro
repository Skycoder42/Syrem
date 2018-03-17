TEMPLATE = lib

QT += mvvmcore mvvmdatasynccore
CONFIG += static

TARGET = RemindMeCore

QMAKE_TARGET_PRODUCT = "Remind-Me"
DEFINES += "DISPLAY_NAME=\"\\\"$$QMAKE_TARGET_PRODUCT\\\"\""

HEADERS += \
	remindmeapp.h \
	mainviewmodel.h \
    daemoncontroller.h

SOURCES += \
	remindmeapp.cpp \
	mainviewmodel.cpp \
    daemoncontroller.cpp

RESOURCES += \
	remindmecore.qrc

TRANSLATIONS += remindme_core_de.ts \
	remindme_core_template.ts

DISTFILES += $$TRANSLATIONS

QTMVVM_TS_SETTINGS = settings.xml
_never_true_condition: SOURCES += $$files($$PWD/.ts-dummy/*)
PRE_TARGETDEPS += qtmvvm-tsgen

# link against main lib
include(../../lib.pri)
