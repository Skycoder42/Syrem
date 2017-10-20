TEMPLATE = lib

QT += core gui remoteobjects datasync
CONFIG += c++11 staticlib #important because dlls are problematic

TARGET = RemindMeDaemon
VERSION = $$RM_VERSION

DEFINES += QT_DEPRECATED_WARNINGS

HEADERS += \
	remindermanager.h \
	reminder.h \
	dateparser.h \
	schedule.h

SOURCES += \
	remindermanager.cpp \
	reminder.cpp \
	dateparser.cpp \
	schedule.cpp

REPC_SOURCE += $$fromfile(rep.pri, REPC_FILES)

TRANSLATIONS += remindme_daemon_de.ts \
	remindme_daemo_template.ts

DISTFILES += \
	remindersyntax.md

!ReleaseBuild:!DebugBuild:!system(qpmx -d $$shell_quote($$_PRO_FILE_PWD_) --qmake-run init $$QPMX_EXTRA_OPTIONS $$shell_quote($$QMAKE_QMAKE) $$shell_quote($$OUT_PWD)): error(qpmx initialization failed. Check the compilation log for details.)
else: include($$OUT_PWD/qpmx_generated.pri)
