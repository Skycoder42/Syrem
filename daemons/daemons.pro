TEMPLATE = subdirs

android: SUBDIRS += android
else: SUBDIRS += desktop

prepareRecursiveTarget(lrelease)
QMAKE_EXTRA_TARGETS += lrelease
