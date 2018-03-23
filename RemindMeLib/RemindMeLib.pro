TEMPLATE = lib

QT -= gui
QT += datasync mvvmcore remoteobjects

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

SETTINGS_GENERATORS += \
	localsettings.xml \
	syncedsettings.xml

MVVM_SETTINGS_FILES += $$PWD/../gui/RemindMeCore/settings.xml

TRANSLATIONS += remindme_lib_de.ts \
	remindme_lib_template.ts

DISTFILES += $$TRANSLATIONS

!ReleaseBuild:!DebugBuild:!system(qpmx -d $$shell_quote($$_PRO_FILE_PWD_) --qmake-run init $$QPMX_EXTRA_OPTIONS $$shell_quote($$QMAKE_QMAKE) $$shell_quote($$OUT_PWD)): error(qpmx initialization failed. Check the compilation log for details.)
else: include($$OUT_PWD/qpmx_generated.pri)

# install copy
for(header, SETTINGSGENERATOR_BUILD_HEADERS) {
	theader = $$shadowed($$SETTINGSGENERATOR_DIR/$$basename(header))
	!exists($$theader):system($$QMAKE_COPY_FILE $$shell_quote($$shell_path($$header)) $$shell_quote($$shell_path($$theader)))
}

# actual install
target.path = $$INSTALL_LIBS
qpmx_ts_target.path = $$INSTALL_TRANSLATIONS
header_install.files = $$HEADERS $$SETTINGSGENERATOR_BUILD_HEADERS $$SETTINGSGENERATOR_DIR/settings.h
header_install.path = $$INSTALL_HEADERS/remind-me
INSTALLS += target qpmx_ts_target
!android: INSTALLS += header_install
