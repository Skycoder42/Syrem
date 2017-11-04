TEMPLATE = lib

QT += core gui remoteobjects datasync
CONFIG += c++11 staticlib #important because dlls are problematic

TARGET = RemindMeDaemon
VERSION = $$RM_VERSION

DEFINES += QT_DEPRECATED_WARNINGS

HEADERS += \
	remindmedaemon.h \
	remindermanager.h \
	reminder.h \
	dateparser.h \
	schedule.h \
	registry.h \
	ischeduler.h \
	inotifier.h \
	notificationmanager.h \
	snoozehelper.h

SOURCES += \
	remindmedaemon.cpp \
	remindermanager.cpp \
	reminder.cpp \
	dateparser.cpp \
	schedule.cpp \
	registry.cpp \
	notificationmanager.cpp \
	snoozehelper.cpp

REPC_SOURCE += $$fromfile(rep.pri, REPC_FILES)

TRANSLATIONS += remindme_daemon_de.ts \
	remindme_daemon_template.ts

DISTFILES += \
	remindersyntax.md

qpmx_ts_target.path = $$TS_INSTALL_DIR
INSTALLS += qpmx_ts_target

!ReleaseBuild:!DebugBuild:!system(qpmx -d $$shell_quote($$_PRO_FILE_PWD_) --qmake-run init $$QPMX_EXTRA_OPTIONS $$shell_quote($$QMAKE_QMAKE) $$shell_quote($$OUT_PWD)): error(qpmx initialization failed. Check the compilation log for details.)
else: include($$OUT_PWD/qpmx_generated.pri)
