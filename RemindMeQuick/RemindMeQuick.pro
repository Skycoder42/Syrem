TEMPLATE = app

QT += core gui qml quick remoteobjects datasync
android: QT += androidextras

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

	RESOURCES += \
		remindme_android.qrc
}

RESOURCES += \
	remindmequick.qrc

TRANSLATIONS += remindme_quick_de.ts \
	remindme_quick_template.ts

EXTRA_TRANSLATIONS +=  \
	remindme_de.ts \
	remindme_template.ts

ANDROID_FILES += \
	android/AndroidManifest.xml \
	android/build.gradle \
	android/src/de/skycoder42/remindme/RemindmeService.java \
	android/src/de/skycoder42/remindme/RemindmeActivity.java \
	android/src/de/skycoder42/remindme/BootReceiver.java \
	android/res/values/libs.xml \
	android/res/values/strings.xml \
	android/res/values/styles.xml \
	android/res/drawable-hdpi/ic_check_black_24dp.png \
	android/res/drawable-hdpi/ic_launcher.png \
	android/res/drawable-hdpi/ic_notification.png \
	android/res/drawable-hdpi/ic_snooze_black_24dp.png \
	android/res/drawable-mdpi/ic_check_black_24dp.png \
	android/res/drawable-mdpi/ic_launcher.png \
	android/res/drawable-mdpi/ic_notification.png \
	android/res/drawable-mdpi/ic_snooze_black_24dp.png \
	android/res/drawable-xhdpi/ic_check_black_24dp.png \
	android/res/drawable-xhdpi/ic_launcher.png \
	android/res/drawable-xhdpi/ic_notification.png \
	android/res/drawable-xhdpi/ic_snooze_black_24dp.png \
	android/res/drawable-xxhdpi/ic_check_black_24dp.png \
	android/res/drawable-xxhdpi/ic_launcher.png \
	android/res/drawable-xxhdpi/ic_notification.png \
	android/res/drawable-xxhdpi/ic_snooze_black_24dp.png \
	android/res/drawable-xxxhdpi/ic_check_black_24dp.png \
	android/res/drawable-xxxhdpi/ic_launcher.png \
	android/res/drawable-xxxhdpi/ic_notification.png \
	android/res/drawable-xxxhdpi/ic_snooze_black_24dp.png

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android

DISTFILES += \
	$$ANDROID_FILES \
	$$EXTRA_TRANSLATIONS

android {
	LIBS += -lcrypto -lssl
	ANDROID_EXTRA_LIBS += $$[QT_INSTALL_PREFIX]/lib/libcrypto.so $$[QT_INSTALL_PREFIX]/lib/libssl.so
}

qpmx_ts_target.path = $$TS_INSTALL_DIR
tsqtInstall.path = $$TS_INSTALL_DIR
extra_ts_target.path = $$TS_INSTALL_DIR
INSTALLS += qpmx_ts_target extra_ts_target

tsqtInstall.files = \
	$$[QT_INSTALL_TRANSLATIONS]/qtbase_*.qm \
	$$[QT_INSTALL_TRANSLATIONS]/qtdeclarative_*.qm \
	$$[QT_INSTALL_TRANSLATIONS]/qtquickcontrols_*.qm \
	$$[QT_INSTALL_TRANSLATIONS]/qtwebsockets_*.qm
android: INSTALLS += tsqtInstall

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
