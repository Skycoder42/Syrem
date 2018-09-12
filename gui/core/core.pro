TEMPLATE = lib

QT += mvvmcore mvvmdatasynccore service
android: QT += androidextras
CONFIG += static

TARGET = $${PROJECT_TARGET}_core

HEADERS += \
	syremapp.h \
	mainviewmodel.h \
	createreminderviewmodel.h \
	snoozeviewmodel.h \
	termselectionviewmodel.h

SOURCES += \
	syremapp.cpp \
	mainviewmodel.cpp \
	createreminderviewmodel.cpp \
	snoozeviewmodel.cpp \
	termselectionviewmodel.cpp

RESOURCES += \
	syrem_core.qrc

TRANSLATIONS += syrem_core_de.ts \
	syrem_core_template.ts

DISTFILES += \
	$$TRANSLATIONS \
	de.skycoder42.syrem.desktop

SETTINGS_TRANSLATIONS = settings.xml

# install
include(../../install.pri)

create_icons.target = create_icons
create_icons.commands += $$shell_path($$PWD/../../create_icons.sh) de.skycoder42.syrem $$shell_path($$PWD/../../icon/pngs/syrem/syrem) $$ICON_SIZES
QMAKE_EXTRA_TARGETS += create_icons

qpmx_ts_target.path = $$INSTALL_TRANSLATIONS
desktop_install.files = de.skycoder42.syrem.desktop
desktop_install.path = $$INSTALL_SHARE/applications/
install_icons.files += $$shadowed(icon_export/hicolor)
install_icons.path = $$INSTALL_SHARE/icons/
install_icons.CONFIG += no_check_exist
install_icons.depends += create_icons
INSTALLS += qpmx_ts_target
linux:!android: INSTALLS += desktop_install install_icons

# link against main lib
include(../../lib.pri)

!ReleaseBuild:!DebugBuild:!system(qpmx -d $$shell_quote($$_PRO_FILE_PWD_) --qmake-run init $$QPMX_EXTRA_OPTIONS $$shell_quote($$QMAKE_QMAKE) $$shell_quote($$OUT_PWD)): error(qpmx initialization failed. Check the compilation log for details.)
else: include($$OUT_PWD/qpmx_generated.pri)
