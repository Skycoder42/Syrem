TEMPLATE = subdirs

SUBDIRS += \
	RemindMeLib \
	daemons \
	gui

gui.depends += RemindMeLib
daemons.depends += RemindMeLib

OTHER_FILES += \
	.qmake.conf \
	README.md \
	lib.pri
