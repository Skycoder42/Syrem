TEMPLATE = subdirs

SUBDIRS += \
	lib \
	daemons \
	gui \
	Tests

gui.depends += lib
daemons.depends += lib
Tests.depends += lib

Tests.CONFIG += no_lrelease_target
android: SUBDIRS -= Tests

prepareRecursiveTarget(lrelease)
QMAKE_EXTRA_TARGETS += lrelease

OTHER_FILES += \
	.qmake.conf \
	README.md \
	lib.pri \
	install.pri \
	create_icons.sh

include(install.pri)
!no_bundle_deploy:mac {
	install.commands += cp -pPRfv "$(INSTALL_ROOT)$$INSTALL_APPS/" "$(INSTALL_ROOT)$${PREFIX}/"$$escape_expand(\n\t) \
		rm -rf "$(INSTALL_ROOT)$$INSTALL_APPS"$$escape_expand(\n\t) \
		install_name_tool -change lib$${PROJECT_TARGET}.3.dylib @rpath/lib$${PROJECT_TARGET}.3.dylib "$(INSTALL_ROOT)$${INSTALL_BINS}/$${PROJECT_TARGET}"$$escape_expand(\n\t) \
		install_name_tool -change lib$${PROJECT_TARGET}.3.dylib @rpath/lib$${PROJECT_TARGET}.3.dylib "$(INSTALL_ROOT)$${INSTALL_BINS}/$${PROJECT_TARGET}d"$$escape_expand(\n\t)
	QMAKE_EXTRA_TARGETS += install
}

!isEmpty(PREFIX):!no_bundle_deploy {
	win32: DEPLOY_BINS = "$$INSTALL_BINS/$${PROJECT_TARGET}.exe" "$$INSTALL_BINS/$${PROJECT_TARGET}d.exe"
	else:mac: DEPLOY_BINS = "$$PREFIX/$${PROJECT_NAME}.app" "$$PREFIX/$${APP_PREFIX}/MacOs/syremd"
	else:android: DEPLOY_BINS = "$$OUT_PWD/gui/quick/android-libsyrem_gui.so-deployment-settings.json"
	DEPLOY_PLUGINS += keystores
	systemd_service: DEPLOY_PLUGINS += servicebackends

	!no_widgets: TS_DICTIONARIES += $$PWD/gui/widgets/syrem.tsdict
	!no_quick: TS_DICTIONARIES += $$PWD/gui/quick/syrem.tsdict
	android: TS_DICTIONARIES += $$PWD/daemons/android/syremd.tsdict
	else: TS_DICTIONARIES += $$PWD/daemons/desktop/syremd.tsdict
}

include(deploy/deploy.pri)
