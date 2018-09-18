TEMPLATE = subdirs

SUBDIRS += \
	core \
	widgets \
	quick

widgets.depends += core
quick.depends += core
no_quick: SUBDIRS -= quick
no_widgets: SUBDIRS -= widgets

prepareRecursiveTarget(lrelease)
QMAKE_EXTRA_TARGETS += lrelease
