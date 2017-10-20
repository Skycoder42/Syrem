TEMPLATE = subdirs

SUBDIRS += \
	RemindMeWidgets \
	RemindMeQuick \
	RemindMeCore \
	Tests \
    RemindMeDaemon

RemindMeWidgets.depends += RemindMeCore
RemindMeQuick.depends += RemindMeCore
Tests.depends += RemindMeCore

DISTFILES += .qmake.conf
