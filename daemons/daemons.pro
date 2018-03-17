TEMPLATE = subdirs

android: SUBDIRS += RemindMeAndroidDaemon
else: SUBDIRS += RemindMeDesktopDaemon

prepareRecursiveTarget(lrelease)
QMAKE_EXTRA_TARGETS += lrelease
