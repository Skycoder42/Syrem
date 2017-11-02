TEMPLATE = app

QT += core gui qml quick remoteobjects datasync
android: QT += androidextras
CONFIG += c++11

TARGET = remind-me
VERSION = $$RM_VERSION

QMAKE_TARGET_BUNDLE_PREFIX = de.skycoder42

DEFINES += "TARGET=\\\"$$TARGET\\\""
DEFINES += "VERSION=\\\"$$VERSION\\\""
DEFINES += "COMPANY=\"\\\"Skycoder42\\\"\""
DEFINES += "BUNDLE=\"\\\"$$QMAKE_TARGET_BUNDLE_PREFIX\\\"\""
DEFINES += "DISPLAY_NAME=\"\\\"Remind-Me\\\"\""

HEADERS +=

SOURCES += main.cpp

android {
HEADERS += \
	androidscheduler.h \
	androidnotifier.h

SOURCES += \
	androidscheduler.cpp \
	androidnotifier.cpp
}

RESOURCES += \
	remindmequick.qrc

TRANSLATIONS += remindme_quick_de.ts \
	remindme_quick_template.ts

DISTFILES += \
	android/AndroidManifest.xml \
	android/res/values/libs.xml \
	android/build.gradle \
	android/src/de/skycoder42/remindme/RemindmeService.java \
	android/src/de/skycoder42/remindme/RemindmeActivity.java \
    android/src/de/skycoder42/remindme/BootReceiver.java

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android

# Link with core project
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../RemindMeCore/release/ -lRemindMeCore
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../RemindMeCore/debug/ -lRemindMeCore
else:unix: LIBS += -L$$OUT_PWD/../RemindMeCore/ -lRemindMeCore

INCLUDEPATH += $$PWD/../RemindMeCore
DEPENDPATH += $$PWD/../RemindMeCore

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../RemindMeCore/release/libRemindMeCore.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../RemindMeCore/debug/libRemindMeCore.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../RemindMeCore/release/RemindMeCore.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../RemindMeCore/debug/RemindMeCore.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../RemindMeCore/libRemindMeCore.a

# Link with daemon project
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../RemindMeDaemon/release/ -lRemindMeDaemon
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../RemindMeDaemon/debug/ -lRemindMeDaemon
else:unix: LIBS += -L$$OUT_PWD/../RemindMeDaemon/ -lRemindMeDaemon

INCLUDEPATH += $$PWD/../RemindMeDaemon
DEPENDPATH += $$PWD/../RemindMeDaemon

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../RemindMeDaemon/release/libRemindMeDaemon.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../RemindMeDaemon/debug/libRemindMeDaemon.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../RemindMeDaemon/release/RemindMeDaemon.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../RemindMeDaemon/debug/RemindMeDaemon.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../RemindMeDaemon/libRemindMeDaemon.a

!ReleaseBuild:!DebugBuild:!system(qpmx -d $$shell_quote($$_PRO_FILE_PWD_) --qmake-run init $$QPMX_EXTRA_OPTIONS $$shell_quote($$QMAKE_QMAKE) $$shell_quote($$OUT_PWD)): error(qpmx initialization failed. Check the compilation log for details.)
else: include($$OUT_PWD/qpmx_generated.pri)
