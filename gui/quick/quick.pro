TEMPLATE = app

QT += quick mvvmquick mvvmdatasyncquick service
android: QT += androidextras
CONFIG += qtquickcompiler

android: TARGET = $${PROJECT_TARGET}_gui
else: TARGET = $${PROJECT_TARGET}

QMAKE_TARGET_PRODUCT = $$PROJECT_NAME

HEADERS += \
	snoozetimesformatter.h

SOURCES += main.cpp \
	snoozetimesformatter.cpp

QML_SETTINGS_DEFINITIONS += \
	../../lib/syncedsettings.xml

RESOURCES += \
	syrem_quick.qrc

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android

TRANSLATIONS += syrem_quick_de.ts \
	syrem_quick_template.ts

EXTRA_TRANSLATIONS +=  \
	syrem_de.ts \
	syrem_template.ts

DISTFILES += \
	$$TRANSLATIONS \
	$$EXTRA_TRANSLATIONS \
	android/AndroidManifest.xml \
	android/build.gradle \
	android/src/de/skycoder42/syrem/* \
	android/res/values/* \
	android/res/values-de/* \
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
	android/res/mipmap-anydpi-v26/*

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
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../core/release/ -l$${PROJECT_TARGET}_core
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../core/debug/ -l$${PROJECT_TARGET}_core
else:unix: LIBS += -L$$OUT_PWD/../core/ -l$${PROJECT_TARGET}_core

INCLUDEPATH += $$PWD/../core
DEPENDPATH += $$PWD/../core

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../core/release/lib$${PROJECT_TARGET}_core.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../core/debug/lib$${PROJECT_TARGET}_core.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../core/release/lib$${PROJECT_TARGET}_core.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../core/debug/lib$${PROJECT_TARGET}_core.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../core/lib$${PROJECT_TARGET}_core.a

# link against main lib
include(../../lib.pri)

android {
	LIBS += -L$$PWD/openssl/openssl -lcrypto -lssl
	ANDROID_EXTRA_LIBS += \
		$$PWD/openssl/openssl/libcrypto.so \
		$$PWD/openssl/openssl/libssl.so
	RESOURCES += syrem_quick_android.qrc
}

!ReleaseBuild:!DebugBuild:!system(qpmx -d $$shell_quote($$_PRO_FILE_PWD_) --qmake-run init $$QPMX_EXTRA_OPTIONS $$shell_quote($$QMAKE_QMAKE) $$shell_quote($$OUT_PWD)): error(qpmx initialization failed. Check the compilation log for details.)
else: include($$OUT_PWD/qpmx_generated.pri)
