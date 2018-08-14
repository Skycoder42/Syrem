TEMPLATE = app

TARGET = remind-med

QMAKE_TARGET_PRODUCT = "Remind-Me Daemon"
DEFINES += "DISPLAY_NAME=\"\\\"$$QMAKE_TARGET_PRODUCT\\\"\""

QT += widgets mvvmcore core-private

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
} else {
	HEADERS += widgetsnotifier.h \
		widgetssnoozedialog.h
	SOURCES += widgetsnotifier.cpp \
		widgetssnoozedialog.cpp
}

TRANSLATIONS += remindme_daemon_de.ts \
	remindme_daemon_template.ts

EXTRA_TRANSLATIONS +=  \
	remindmed_de.ts \
	remindmed_template.ts

DISTFILES += \
	remind-me.service.in \
	$$TRANSLATIONS \
	$$EXTRA_TRANSLATIONS \
	remind-me.notifyrc

# install
linux {
	create_service.target = remind-me.service
	create_service.depends += $$PWD/remind-me.service.in
	create_service.commands += sed "s:%{INSTALL_BINS}:$$INSTALL_BINS:g" $$PWD/remind-me.service.in > remind-me.service

	QMAKE_EXTRA_TARGETS += create_service
	PRE_TARGETDEPS += remind-me.service

	install_service.files += $$OUT_PWD/remind-me.service
	install_service.CONFIG += no_check_exist
	install_service.path = $$INSTALL_LIBS/systemd/user/
	install_icons.files += ../../icon/remind-me.svg
	kde_notifier: install_icons.files += ../../icon/remind-me-error.svg
	install_icons.path = $$INSTALL_SHARE/icons/hicolor/scalable/apps
	INSTALLS += install_service install_icons
}
kde_notifier {
	notify_install.path = $$INSTALL_SHARE/knotifications5/
	notify_install.files = remind-me.notifyrc
	INSTALLS += notify_install
}

target.path = $$INSTALL_BINS
qpmx_ts_target.path = $$INSTALL_TRANSLATIONS
extra_ts_target.path = $$INSTALL_TRANSLATIONS
INSTALLS += target qpmx_ts_target extra_ts_target

# link against main lib
include(../../lib.pri)

!ReleaseBuild:!DebugBuild:!system(qpmx -d $$shell_quote($$_PRO_FILE_PWD_) --qmake-run init $$QPMX_EXTRA_OPTIONS $$shell_quote($$QMAKE_QMAKE) $$shell_quote($$OUT_PWD)): error(qpmx initialization failed. Check the compilation log for details.)
else: include($$OUT_PWD/qpmx_generated.pri)
