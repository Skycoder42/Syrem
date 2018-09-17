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
!no_platform_paths:mac {
	install_join.commands += cp -pPRfv "$(INSTALL_ROOT)$$INSTALL_APPS/" "$(INSTALL_ROOT)$${INSTALL_PREFIX}/" \
		$$escape_expand(\n\t)rm -rf "$(INSTALL_ROOT)$$INSTALL_APPS" \
		$$escape_expand(\n\t)install_name_tool -change $${PROJECT_TARGET}.framework/Versions/2/$${PROJECT_TARGET} @rpath/$${PROJECT_TARGET}.framework/Versions/2/$${PROJECT_TARGET} "$(INSTALL_ROOT)$${INSTALL_BINS}/$${PROJECT_TARGET}"
	install_join.target = install
	QMAKE_EXTRA_TARGETS += install_join
}

include(deploy/deploy.pri)
