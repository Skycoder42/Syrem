TEMPLATE = app

TARGET = $${PROJECT_TARGET}_service

QMAKE_TARGET_PRODUCT = "Syrem Service"

QT += androidextras mvvmcore service

HEADERS += \
	syremservice.h \
	androidscheduler.h \
	androidnotifier.h

SOURCES += main.cpp \
	syremservice.cpp \
	androidscheduler.cpp \
	androidnotifier.cpp

TRANSLATIONS += syrem_daemon_de.ts \
	syrem_daemon_template.ts

EXTRA_TRANSLATIONS +=  \
	syremd_de.ts \
	syremd_template.ts

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
