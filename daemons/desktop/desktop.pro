TEMPLATE = app

TARGET = $${PROJECT_TARGET}d

QMAKE_TARGET_DESCRIPTION = "Syrem Background Service"
RC_ICONS += ../../icon/syrem.ico
ICON = ../../icon/syrem.icns

QT += widgets mvvmcore mvvmwidgets core-private
systemd_service: QT += service
CONFIG -= app_bundle

!no_kde_notifier: qtHaveModule(KNotifications): CONFIG += kde_notifier

kde_notifier {
	message(Using KDE notifications)
	QT += KNotifications
	DEFINES += USE_KDE_NOTIFIER
}

HEADERS += \
	syremdaemon.h \
	notificationmanager.h \
	timerscheduler.h \
	inotifier.h \
	traysnoozeviewmodel.h \
	traysnoozedialog.h

SOURCES += main.cpp \
	syremdaemon.cpp \
	notificationmanager.cpp \
	timerscheduler.cpp \
	traysnoozeviewmodel.cpp \
	traysnoozedialog.cpp

RESOURCES += \
	syrem_daemon.qrc

include(widgets-include.pri)

kde_notifier {
	HEADERS += kdenotifier.h
	SOURCES += kdenotifier.cpp
} else {
	HEADERS += widgetsnotifier.h
	SOURCES += widgetsnotifier.cpp
}

TRANSLATIONS += syrem_daemon_de.ts \
	syrem_daemon_template.ts

EXTRA_TRANSLATIONS +=  \
	syremd_de.ts \
	syremd_template.ts

DISTFILES += \
	syrem.service.in \
	$$TRANSLATIONS \
	$$EXTRA_TRANSLATIONS \
	syrem.notifyrc

# install
include(../../install.pri)

systemd_service {
	create_service.target = syrem.service
	create_service.depends += $$PWD/syrem.service.in
	create_service.commands += sed "s:%{INSTALL_BINS}:$$INSTALL_BINS:g" $$PWD/syrem.service.in > syrem.service

	QMAKE_EXTRA_TARGETS += create_service
	PRE_TARGETDEPS += syrem.service

	install_service.files += $$OUT_PWD/syrem.service
	install_service.CONFIG += no_check_exist
	install_service.path = $$INSTALL_LIBS/systemd/user/
	INSTALLS += install_service
}
kde_notifier {
	create_icons.target = create_icons
	create_icons.commands += $$shell_path($$PWD/../../create_icons.sh) de.skycoder42.syrem-error $$shell_path($$PWD/../../icon/pngs/syrem-error/syrem-error) $$ICON_SIZES
	QMAKE_EXTRA_TARGETS += create_icons

	notify_install.path = $$INSTALL_SHARE/knotifications5/
	notify_install.files = syrem.notifyrc
	install_icons.files += $$shadowed(icon_export/hicolor)
	install_icons.path = $$INSTALL_SHARE/icons/
	install_icons.CONFIG += no_check_exist
	install_icons.depends += create_icons
	INSTALLS += notify_install install_icons
}

target.path = $$INSTALL_BINS
qpmx_ts_target.path = $$INSTALL_TRANSLATIONS
extra_ts_target.path = $$INSTALL_TRANSLATIONS
INSTALLS += target qpmx_ts_target extra_ts_target

# link against main lib
include(../../lib.pri)

!ReleaseBuild:!DebugBuild:!system(qpmx -d $$shell_quote($$_PRO_FILE_PWD_) --qmake-run init $$QPMX_EXTRA_OPTIONS $$shell_quote($$QMAKE_QMAKE) $$shell_quote($$OUT_PWD)): error(qpmx initialization failed. Check the compilation log for details.)
else: include($$OUT_PWD/qpmx_generated.pri)
