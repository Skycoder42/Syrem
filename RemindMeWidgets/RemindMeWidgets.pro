TEMPLATE = app

QT += core gui widgets remoteobjects datasync
CONFIG += c++11

kde_notifier {
	QT += KNotifications
	DEFINES += USE_KDE_NOTIFIER
}

TARGET = remind-me
VERSION = $$RM_VERSION

RC_ICONS += ../icn/remindme.ico
QMAKE_TARGET_COMPANY = "Skycoder42"
QMAKE_TARGET_PRODUCT = "Remind-Me"
QMAKE_TARGET_DESCRIPTION = "Simple reminder app"
QMAKE_TARGET_COPYRIGHT = "Felix Barz"
QMAKE_TARGET_BUNDLE_PREFIX = de.skycoder42

ICON = ../icn/remindme.icns
QMAKE_TARGET_BUNDLE_PREFIX = de.skycoder42

DEFINES += "TARGET=\\\"$$TARGET\\\""
DEFINES += "VERSION=\\\"$$VERSION\\\""
DEFINES += "COMPANY=\"\\\"$$QMAKE_TARGET_COMPANY\\\"\""
DEFINES += "BUNDLE=\"\\\"$$QMAKE_TARGET_BUNDLE_PREFIX\\\"\""
DEFINES += "DISPLAY_NAME=\"\\\"$$QMAKE_TARGET_PRODUCT\\\"\""

HEADERS += mainwindow.h \
	createreminderdialog.h \
	widgetsscheduler.h

SOURCES += main.cpp \
	mainwindow.cpp \
	createreminderdialog.cpp \
	widgetsscheduler.cpp

kde_notifier {
	HEADERS += kdenotifier.h \
		kdesnoozedialog.h
	SOURCES += kdenotifier.cpp \
		kdesnoozedialog.cpp
	DISTFILES += remind-me.notifyrc
} else {
	HEADERS += widgetsnotifier.h \
		widgetssnoozedialog.h
	SOURCES += widgetsnotifier.cpp \
		widgetssnoozedialog.cpp
}

FORMS += mainwindow.ui \
	createreminderdialog.ui

RESOURCES += \
	remindmewidgets.qrc

TRANSLATIONS += remindme_widgets_de.ts \
	remindme_widgets_template.ts

DISTFILES += \
	remind-me.desktop

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
