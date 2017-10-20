TEMPLATE = subdirs

SUBDIRS += \
	RemindMeDaemon \
	RemindMeCore \
	RemindMeWidgets \
	RemindMeQuick \
	Tests

RemindMeWidgets.depends += RemindMeCore RemindMeDaemon
RemindMeQuick.depends += RemindMeCore RemindMeDaemon
Tests.depends += RemindMeDaemon

#DEBUG qpmx bug
RemindMeCore.depends += RemindMeDaemon

DISTFILES += .qmake.conf
