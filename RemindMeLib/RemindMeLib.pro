TEMPLATE = lib

QT -= gui
QT += datasync mvvmcore mvvmdatasynccore remoteobjects

TARGET = remind-me

DEFINES += REMINDMELIB_LIBRARY

HEADERS += \
	remindmelib_global.h \
	dateparser.h \
	reminder.h \
	schedule.h \
	conflictresolver.h \
	remindmelib.h \
	snoozetimes.h \
    eventexpressionparser.h

SOURCES += \
	dateparser.cpp \
	reminder.cpp \
	schedule.cpp \
	conflictresolver.cpp \
	remindmelib.cpp \
	snoozetimes.cpp \
    eventexpressionparser.cpp

SETTINGS_DEFINITIONS += \
	localsettings.xml \
	syncedsettings.xml

TRANSLATIONS += remindme_lib_de.ts \
	remindme_lib_template.ts

DISTFILES += $$TRANSLATIONS \
	qpmx.json

!ReleaseBuild:!DebugBuild:!system(qpmx -d $$shell_quote($$_PRO_FILE_PWD_) --qmake-run init $$QPMX_EXTRA_OPTIONS $$shell_quote($$QMAKE_QMAKE) $$shell_quote($$OUT_PWD)): error(qpmx initialization failed. Check the compilation log for details.)
else: include($$OUT_PWD/qpmx_generated.pri)

# actual install
target.path = $$INSTALL_LIBS
qpmx_ts_target.path = $$INSTALL_TRANSLATIONS
header_install.files = $$HEADERS $$QSETTINGSGENERATOR_DIR/localsettings.h $$QSETTINGSGENERATOR_DIR/syncedsettings.h
header_install.path = $$INSTALL_HEADERS/remind-me
INSTALLS += target qpmx_ts_target
!android: INSTALLS += header_install
