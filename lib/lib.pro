TEMPLATE = lib

QT -= gui
QT += datasync mvvmcore mvvmdatasynccore remoteobjects concurrent

TARGET = $$PROJECT_TARGET

DEFINES += SYREM_LIBRARY

HEADERS += \
	libsyrem_global.h \
	libsyrem.h \
	dateparser.h \
	reminder.h \
	schedule.h \
	conflictresolver.h \
	snoozetimes.h \
	eventexpressionparser.h \
	terms.h \
	termconverter.h

SOURCES += \
	libsyrem.cpp \
	dateparser.cpp \
	reminder.cpp \
	schedule.cpp \
	conflictresolver.cpp \
	snoozetimes.cpp \
	eventexpressionparser.cpp \
	terms.cpp \
	termconverter.cpp

SETTINGS_DEFINITIONS += \
	localsettings.xml \
	syncedsettings.xml

TRANSLATIONS += syrem_lib_de.ts \
	syrem_lib_template.ts

DISTFILES += $$TRANSLATIONS \
	qpmx.json

!ReleaseBuild:!DebugBuild:!system(qpmx -d $$shell_quote($$_PRO_FILE_PWD_) --qmake-run init $$QPMX_EXTRA_OPTIONS $$shell_quote($$QMAKE_QMAKE) $$shell_quote($$OUT_PWD)): error(qpmx initialization failed. Check the compilation log for details.)
else: include($$OUT_PWD/qpmx_generated.pri)

# actual install
target.path = $$INSTALL_LIBS
qpmx_ts_target.path = $$INSTALL_TRANSLATIONS
header_install.files = $$HEADERS $$QSETTINGSGENERATOR_DIR/localsettings.h $$QSETTINGSGENERATOR_DIR/syncedsettings.h
header_install.path = $$INSTALL_HEADERS/$$PROJECT_TARGET
INSTALLS += target qpmx_ts_target
!android: INSTALLS += header_install
