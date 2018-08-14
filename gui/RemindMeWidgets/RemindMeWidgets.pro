TEMPLATE = app

QT += widgets mvvmwidgets mvvmdatasyncwidgets service

TARGET = remind-me

QMAKE_TARGET_PRODUCT = "Remind-Me"
DEFINES += "DISPLAY_NAME=\"\\\"$$QMAKE_TARGET_PRODUCT\\\"\""

HEADERS += mainwindow.h \
	createreminderdialog.h \
	snoozedialog.h \
	snoozetimesedit.h

SOURCES += main.cpp \
	mainwindow.cpp \
	createreminderdialog.cpp \
	snoozedialog.cpp \
	snoozetimesedit.cpp

FORMS += mainwindow.ui \
	createreminderdialog.ui \
	snoozetimesedit.ui

RESOURCES += \
	remindmewidgets.qrc

TRANSLATIONS += remindme_widgets_de.ts \
	remindme_widgets_template.ts

EXTRA_TRANSLATIONS +=  \
	remindme_de.ts \
	remindme_template.ts

DISTFILES += \
	$$TRANSLATIONS \
	$$EXTRA_TRANSLATIONS

# actual install
target.path = $$INSTALL_BINS
qpmx_ts_target.path = $$INSTALL_TRANSLATIONS
extra_ts_target.path = $$INSTALL_TRANSLATIONS
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

!ReleaseBuild:!DebugBuild:!system(qpmx -d $$shell_quote($$_PRO_FILE_PWD_) --qmake-run init $$QPMX_EXTRA_OPTIONS $$shell_quote($$QMAKE_QMAKE) $$shell_quote($$OUT_PWD)): error(qpmx initialization failed. Check the compilation log for details.)
else: include($$OUT_PWD/qpmx_generated.pri)
