TEMPLATE = app

QT += quick mvvmquick mvvmdatasyncquick
android: QT += androidextras

android: TARGET = RemindMeQuick
else: TARGET = remind-me

QMAKE_TARGET_PRODUCT = "Remind-Me"
DEFINES += "DISPLAY_NAME=\"\\\"$$QMAKE_TARGET_PRODUCT\\\"\""

HEADERS += \
	settingsqmlwrapper.h

SOURCES += main.cpp \
	settingsqmlwrapper.cpp

RESOURCES += \
	remindmequick.qrc

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android

TRANSLATIONS += remindme_quick_de.ts \
	remindme_quick_template.ts

EXTRA_TRANSLATIONS +=  \
	remindme_de.ts \
	remindme_template.ts

DISTFILES += \
	$$TRANSLATIONS \
	$$EXTRA_TRANSLATIONS \
	android/AndroidManifest.xml \
	android/build.gradle \
	android/src/de/skycoder42/remindme/* \
	android/res/values/* \
	android/res/xml/* \
	android/res/drawable/* \
	android/res/drawable-hdpi/* \
	android/res/drawable-mdpi/* \
	android/res/drawable-xhdpi/* \
	android/res/drawable-xxhdpi/* \
	android/res/drawable-xxxhdpi/* \
	android/res/mipmap-hdpi/* \
	android/res/mipmap-mdpi/* \
	android/res/mipmap-xhdpi/* \
	android/res/mipmap-xxhdpi/* \
	android/res/mipmap-xxxhdpi/* \
	android/res/mipmap-anydpi/*

# actual install
target.path = $$INSTALL_BINS
qpmx_ts_target.path = $$INSTALL_TRANSLATIONS
extra_ts_target.path = $$INSTALL_TRANSLATIONS
android: extra_ts_target.files += \
	$$[QT_INSTALL_TRANSLATIONS]/qtbase_*.qm \
	$$[QT_INSTALL_TRANSLATIONS]/qtdeclarative_*.qm \
	$$[QT_INSTALL_TRANSLATIONS]/qtquickcontrols_*.qm \
	$$[QT_INSTALL_TRANSLATIONS]/qtwebsockets_*.qm \
	$$[QT_INSTALL_TRANSLATIONS]/qtdatasync_*.qm \
	$$[QT_INSTALL_TRANSLATIONS]/qtmvvmcore_*.qm \
	$$[QT_INSTALL_TRANSLATIONS]/qtmvvmdatasynccore_*.qm \
	$$[QT_INSTALL_TRANSLATIONS]/qtmvvmquick_*.qm \
	$$[QT_INSTALL_TRANSLATIONS]/qtmvvmdatasyncquick_*.qm
INSTALLS += target qpmx_ts_target extra_ts_target

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

# link against main lib
include(../../lib.pri)

android {
	LIBS += -L$$PWD/openssl/openssl -lcrypto -lssl
	ANDROID_EXTRA_LIBS += \
		$$PWD/openssl/openssl/libcrypto.so \
		$$PWD/openssl/openssl/libssl.so
	RESOURCES += remindme_android.qrc
}

!ReleaseBuild:!DebugBuild:!system(qpmx -d $$shell_quote($$_PRO_FILE_PWD_) --qmake-run init $$QPMX_EXTRA_OPTIONS $$shell_quote($$QMAKE_QMAKE) $$shell_quote($$OUT_PWD)): error(qpmx initialization failed. Check the compilation log for details.)
else: include($$OUT_PWD/qpmx_generated.pri)
