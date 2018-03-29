TEMPLATE = app

QMAKE_TARGET_PRODUCT = "Remind-Me Daemon"
DEFINES += "DISPLAY_NAME=\"\\\"$$QMAKE_TARGET_PRODUCT\\\"\""

QT += androidextras mvvmcore

HEADERS += \
	androidscheduler.h \
	remindmeservice.h \
	androidnotifier.h

SOURCES += main.cpp \
	androidscheduler.cpp \
	remindmeservice.cpp \
	androidnotifier.cpp

TRANSLATIONS += remindme_daemon_de.ts \
	remindme_daemon_template.ts

EXTRA_TRANSLATIONS +=  \
	remindmed_de.ts \
	remindmed_template.ts

DISTFILES += \
	$$TRANSLATIONS \
	$$EXTRA_TRANSLATIONS

qpmx_ts_target.path = $$INSTALL_TRANSLATIONS
extra_ts_target.path = $$INSTALL_TRANSLATIONS
INSTALLS += qpmx_ts_target extra_ts_target

# link against main lib
include(../../lib.pri)

!ReleaseBuild:!DebugBuild:!system(qpmx -d $$shell_quote($$_PRO_FILE_PWD_) --qmake-run init $$QPMX_EXTRA_OPTIONS $$shell_quote($$QMAKE_QMAKE) $$shell_quote($$OUT_PWD)): error(qpmx initialization failed. Check the compilation log for details.)
else: include($$OUT_PWD/qpmx_generated.pri)
