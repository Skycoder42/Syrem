TEMPLATE = subdirs

SUBDIRS += \
	RemindMeWidgets \
	RemindMeQuick \
	RemindMeCore \
	Tests

RemindMeWidgets.depends += RemindMeCore
RemindMeQuick.depends += RemindMeCore
Tests.depends += RemindMeCore

DISTFILES += .qmake.conf
