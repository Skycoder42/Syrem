TEMPLATE = subdirs

SUBDIRS += \
	RemindMeCore \
	RemindMeWidgets \
	RemindMeQuick

RemindMeWidgets.depends += RemindMeCore
RemindMeQuick.depends += RemindMeCore
