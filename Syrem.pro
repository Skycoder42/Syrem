TEMPLATE = subdirs

SUBDIRS += \
	lib \
	daemons \
	gui \
	Tests

gui.depends += lib
daemons.depends += lib
Tests.depends += lib

Tests.CONFIG += no_lrelease_target
android: SUBDIRS -= Tests

prepareRecursiveTarget(lrelease)
QMAKE_EXTRA_TARGETS += lrelease

OTHER_FILES += \
	.qmake.conf \
	README.md \
	lib.pri \
	install.pri
