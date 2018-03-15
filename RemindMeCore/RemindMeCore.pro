TEMPLATE = lib

QT += core gui remoteobjects mvvmcore
CONFIG += staticlib #important because dlls are problematic

TARGET = RemindMeCore
VERSION = $$RM_VERSION

DEFINES += QT_DEPRECATED_WARNINGS

HEADERS += \
	remindmeapp.h \
	snoozetimes.h \
	mainviewmodel.h \
	createreminderviewmodel.h \
	snoozeviewmodel.h

SOURCES += \
	remindmeapp.cpp \
	snoozetimes.cpp \
	mainviewmodel.cpp \
	createreminderviewmodel.cpp \
	snoozeviewmodel.cpp

REPC_REPLICA += $$fromfile(../RemindMeDaemon/rep.pri, REPC_FILES)

RESOURCES += \
	remindmecore.qrc

TRANSLATIONS += remindme_core_de.ts \
	remindme_core_template.ts

QTMVVM_TS_SETTINGS = settings.xml
_never_true_condition: SOURCES += $$files($$PWD/.ts-dummy/*)
PRE_TARGETDEPS += qtmvvm-tsgen

qpmx_ts_target.path = $$TS_INSTALL_DIR
INSTALLS += qpmx_ts_target

!ReleaseBuild:!DebugBuild:!system(qpmx -d $$shell_quote($$_PRO_FILE_PWD_) --qmake-run init $$QPMX_EXTRA_OPTIONS $$shell_quote($$QMAKE_QMAKE) $$shell_quote($$OUT_PWD)): error(qpmx initialization failed. Check the compilation log for details.)
else: include($$OUT_PWD/qpmx_generated.pri)
