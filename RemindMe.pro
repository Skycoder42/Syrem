TEMPLATE = subdirs

SUBDIRS += \
	RemindMeWidgets \
	RemindMeQuick \
	RemindMeCore \
    Tests

RemindMeWidgets.depends += RemindMeCore
RemindMeQuick.depends += RemindMeCore

DISTFILES += .qmake.conf
