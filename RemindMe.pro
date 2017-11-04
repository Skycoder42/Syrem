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

DISTFILES += .qmake.conf \
	README.md

android: SUBDIRS -= RemindMeWidgets Tests

qpmxlrelease.target = lrelease
qpmxlrelease.CONFIG += recursive
qpmxlrelease.recurse_target = lrelease
QMAKE_EXTRA_TARGETS += qpmxlrelease
