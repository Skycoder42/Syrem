TEMPLATE = subdirs

android: SUBDIRS += RemindMeAndroidDaemon
else: SUBDIRS += RemindMeDesktopDaemon
