TEMPLATE = app

TARGET = remind-med

QMAKE_TARGET_PRODUCT = "Remind-Me Daemon"
DEFINES += "DISPLAY_NAME=\"\\\"$$QMAKE_TARGET_PRODUCT\\\"\""

QT += widgets

SOURCES += main.cpp

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

RESOURCES += \
    remindmedesktopdaemon.qrc
