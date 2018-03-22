TEMPLATE = subdirs

SUBDIRS += \
	RemindMeCore \
	RemindMeWidgets \
	RemindMeQuick

RemindMeWidgets.depends += RemindMeCore
RemindMeQuick.depends += RemindMeCore
android: SUBDIRS -= RemindMeWidgets

prepareRecursiveTarget(lrelease)
QMAKE_EXTRA_TARGETS += lrelease
