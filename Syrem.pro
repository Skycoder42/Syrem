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
	install_join.commands += cp -pPRfv "$(INSTALL_ROOT)$$INSTALL_APPS/" "$(INSTALL_ROOT)$${PREFIX}/" \
		$$escape_expand(\n\t)rm -rf "$(INSTALL_ROOT)$$INSTALL_APPS" \
		$$escape_expand(\n\t)install_name_tool -change lib$${PROJECT_TARGET}.2.dylib @rpath/lib$${PROJECT_TARGET}.2.dylib "$(INSTALL_ROOT)$${INSTALL_BINS}/$${PROJECT_TARGET}" \
		$$escape_expand(\n\t)install_name_tool -change lib$${PROJECT_TARGET}.2.dylib @rpath/lib$${PROJECT_TARGET}.2.dylib "$(INSTALL_ROOT)$${INSTALL_BINS}/$${PROJECT_TARGET}d"
	install_join.target = install
	QMAKE_EXTRA_TARGETS += install_join
}

!isEmpty(PREFIX):!no_bundle_deploy {
	win32: DEPLOY_BINS = "$$INSTALL_BINS/$${PROJECT_TARGET}.exe" "$$INSTALL_BINS/$${PROJECT_TARGET}d.exe"
	else:mac: DEPLOY_BINS = "$$PREFIX/$${PROJECT_NAME}.app" "$$PREFIX/$${APP_PREFIX}/MacOs/syremd"
	DEPLOY_PLUGINS += keystores
	systemd_service: DEPLOY_PLUGINS += servicebackends
}

include(deploy/deploy.pri)
