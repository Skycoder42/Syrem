TEMPLATE = subdirs

SUBDIRS += \
	core \
	widgets \
	quick

widgets.depends += core
quick.depends += core
android: SUBDIRS -= widgets

prepareRecursiveTarget(lrelease)
QMAKE_EXTRA_TARGETS += lrelease
