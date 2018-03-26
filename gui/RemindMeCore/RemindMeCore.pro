TEMPLATE = lib

QT += mvvmcore mvvmdatasynccore
android: QT += androidextras
CONFIG += static

TARGET = RemindMeCore

QMAKE_TARGET_PRODUCT = "Remind-Me"
DEFINES += "DISPLAY_NAME=\"\\\"$$QMAKE_TARGET_PRODUCT\\\"\""

HEADERS += \
	remindmeapp.h \
	mainviewmodel.h \
	createreminderviewmodel.h \
	snoozeviewmodel.h \
	datasyncsettingsviewmodel.h

SOURCES += \
	remindmeapp.cpp \
	mainviewmodel.cpp \
	createreminderviewmodel.cpp \
	snoozeviewmodel.cpp \
	datasyncsettingsviewmodel.cpp

!android {
	HEADERS += daemoncontroller.h
	SOURCES += daemoncontroller.cpp
}

RESOURCES += \
	remindmecore.qrc

TRANSLATIONS += remindme_core_de.ts \
	remindme_core_template.ts

DISTFILES += \
	remind-me.desktop \
	$$TRANSLATIONS

QTMVVM_TS_SETTINGS = settings.xml
_never_true_condition: SOURCES += $$files($$PWD/.ts-dummy/*)
PRE_TARGETDEPS += qtmvvm-tsgen

# install
qpmx_ts_target.path = $$INSTALL_TRANSLATIONS
desktop_install.files = remind-me.desktop
desktop_install.path = $$INSTALL_SHARE/applications/
INSTALLS += qpmx_ts_target
linux:!android: INSTALLS += desktop_install

# link against main lib
include(../../lib.pri)

!ReleaseBuild:!DebugBuild:!system(qpmx -d $$shell_quote($$_PRO_FILE_PWD_) --qmake-run init $$QPMX_EXTRA_OPTIONS $$shell_quote($$QMAKE_QMAKE) $$shell_quote($$OUT_PWD)): error(qpmx initialization failed. Check the compilation log for details.)
else: include($$OUT_PWD/qpmx_generated.pri)
