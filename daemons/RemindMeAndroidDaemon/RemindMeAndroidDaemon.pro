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

RESOURCES +=

# link against main lib
include(../../lib.pri)

!ReleaseBuild:!DebugBuild:!system(qpmx -d $$shell_quote($$_PRO_FILE_PWD_) --qmake-run init $$QPMX_EXTRA_OPTIONS $$shell_quote($$QMAKE_QMAKE) $$shell_quote($$OUT_PWD)): error(qpmx initialization failed. Check the compilation log for details.)
else: include($$OUT_PWD/qpmx_generated.pri)
