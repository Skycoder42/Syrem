TEMPLATE = subdirs

SUBDIRS += \
	RemindMeCore \
	RemindMeWidgets \
	RemindMeQuick

RemindMeWidgets.depends += RemindMeCore
RemindMeQuick.depends += RemindMeCore

prepareRecursiveTarget(lrelease)
QMAKE_EXTRA_TARGETS += lrelease
