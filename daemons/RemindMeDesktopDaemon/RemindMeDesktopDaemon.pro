TEMPLATE = app

TARGET = remind-med

QMAKE_TARGET_PRODUCT = "Remind-Me Daemon"
DEFINES += "DISPLAY_NAME=\"\\\"$$QMAKE_TARGET_PRODUCT\\\"\""

QT += widgets mvvmcore

!no_kde_notifier: qtHaveModule(KNotifications): CONFIG += kde_notifier

kde_notifier {
	message(Using KDE notifications)
	QT += KNotifications
	DEFINES += USE_KDE_NOTIFIER
}

HEADERS += \
	remindmedaemon.h \
	notificationmanager.h \
	timerscheduler.h \
	inotifier.h

SOURCES += main.cpp \
	remindmedaemon.cpp \
	notificationmanager.cpp \
	timerscheduler.cpp

RESOURCES += \
	remindmedesktopdaemon.qrc

kde_notifier {
	HEADERS += kdenotifier.h \
		kdesnoozedialog.h
	SOURCES += kdenotifier.cpp \
		kdesnoozedialog.cpp
	DISTFILES += remind-me.notifyrc
} else {
	HEADERS += widgetsnotifier.h \
		widgetssnoozedialog.h
	SOURCES += widgetsnotifier.cpp \
		widgetssnoozedialog.cpp
}

DISTFILES += \
	remind-me.service.in

# link against main lib
include(../../lib.pri)

linux {
	create_service.target = remind-me.service
	create_service.depends += $$PWD/remind-me.service.in
	create_service.commands += sed "s:%{INSTALL_BINS}:$$[QT_INSTALL_BINS]:g" $$PWD/remind-me.service.in > remind-me.service

	QMAKE_EXTRA_TARGETS += create_service
	PRE_TARGETDEPS += remind-me.service

	install_service.files += $$OUT_PWD/remind-me.service
	install_service.path = /usr/lib/systemd/user/
	INSTALLS += install_service
}

!ReleaseBuild:!DebugBuild:!system(qpmx -d $$shell_quote($$_PRO_FILE_PWD_) --qmake-run init $$QPMX_EXTRA_OPTIONS $$shell_quote($$QMAKE_QMAKE) $$shell_quote($$OUT_PWD)): error(qpmx initialization failed. Check the compilation log for details.)
else: include($$OUT_PWD/qpmx_generated.pri)
