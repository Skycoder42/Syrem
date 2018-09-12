TEMPLATE = app

QT += widgets mvvmwidgets mvvmdatasyncwidgets service

TARGET = $$PROJECT_TARGET

QMAKE_TARGET_PRODUCT = $$PROJECT_NAME
RC_ICONS += ../../icon/syrem.ico
ICON = ../../icon/syrem.icns

HEADERS += mainwindow.h \
	createreminderdialog.h \
	snoozedialog.h \
	snoozetimesedit.h \
	termselectiondialog.h

SOURCES += main.cpp \
	mainwindow.cpp \
	createreminderdialog.cpp \
	snoozedialog.cpp \
	snoozetimesedit.cpp \
	termselectiondialog.cpp

FORMS += mainwindow.ui \
	createreminderdialog.ui \
	snoozetimesedit.ui \
	termselectiondialog.ui

RESOURCES += \
	syrem_widgets.qrc

TRANSLATIONS += syrem_widgets_de.ts \
	syrem_widgets_template.ts

EXTRA_TRANSLATIONS +=  \
	syrem_de.ts \
	syrem_template.ts

DISTFILES += \
	$$TRANSLATIONS \
	$$EXTRA_TRANSLATIONS

# actual install
include(../../install.pri)

target.path = $$INSTALL_BINS
qpmx_ts_target.path = $$INSTALL_TRANSLATIONS
extra_ts_target.path = $$INSTALL_TRANSLATIONS
INSTALLS += target qpmx_ts_target extra_ts_target

# Link with core project
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../core/release/ -l$${PROJECT_TARGET}_core
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../core/debug/ -l$${PROJECT_TARGET}_core
else:unix: LIBS += -L$$OUT_PWD/../core/ -l$${PROJECT_TARGET}_core

INCLUDEPATH += $$PWD/../core
DEPENDPATH += $$PWD/../core

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../core/release/lib$${PROJECT_TARGET}_core.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../core/debug/lib$${PROJECT_TARGET}_core.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../core/release/$${PROJECT_TARGET}_core.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../core/debug/$${PROJECT_TARGET}_core.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../core/lib$${PROJECT_TARGET}_core.a

# link against main lib
include(../../lib.pri)

!ReleaseBuild:!DebugBuild:!system(qpmx -d $$shell_quote($$_PRO_FILE_PWD_) --qmake-run init $$QPMX_EXTRA_OPTIONS $$shell_quote($$QMAKE_QMAKE) $$shell_quote($$OUT_PWD)): error(qpmx initialization failed. Check the compilation log for details.)
else: include($$OUT_PWD/qpmx_generated.pri)
