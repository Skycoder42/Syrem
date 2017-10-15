TEMPLATE = subdirs

SUBDIRS += \
	RemindMeWidgets \
	RemindMeQuick \
	RemindMeCore

RemindMeWidgets.depends += RemindMeCore
RemindMeQuick.depends += RemindMeCore

DISTFILES += .qmake.conf
