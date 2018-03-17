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

DISTFILES += .qmake.conf \
	README.md

android: SUBDIRS -= RemindMeWidgets Tests
else:widgets_only: SUBDIRS -= RemindMeQuick Tests

Tests.CONFIG += no_lrelease_target

prepareRecursiveTarget(lrelease)
QMAKE_EXTRA_TARGETS += lrelease
