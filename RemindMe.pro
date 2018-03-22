TEMPLATE = subdirs

SUBDIRS += \
	RemindMeLib \
	daemons \
	gui \
	Tests

gui.depends += RemindMeLib
daemons.depends += RemindMeLib
Tests.depends += RemindMeLib

Tests.CONFIG += no_lrelease_target
android: SUBDIRS -= Tests

prepareRecursiveTarget(lrelease)
QMAKE_EXTRA_TARGETS += lrelease

OTHER_FILES += \
	.qmake.conf \
	README.md \
	lib.pri
