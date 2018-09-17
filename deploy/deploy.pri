win32: deploy_target.binary = "$$INSTALL_BINS/$${PROJECT_TARGET}.exe"
else:mac: deploy_target.binary = "$$PREFIX/$${PROJECT_NAME}.app"
else: deploy_target.binary = "$$INSTALL_BINS/$${PROJECT_TARGET}"

# begin deploy script
deploy_target.target = deploy
deploy_target.depends += install

win32 {
	qtPrepareTool(QMAKE_WINDEPLOYQT, windeployqt)
	CONFIG(release, debug|release): QMAKE_WINDEPLOYQT += --release
	CONFIG(debug, debug|release): QMAKE_WINDEPLOYQT += --debug
	QMAKE_WINDEPLOYQT += --no-translations $${deploy_target.extra_args}
	deploy_target.commands += $$QMAKE_WINDEPLOYQT \"$(INSTALL_ROOT)$${deploy_target.binary}\" \
		$$escape_expand(\n\t)del \"vc*redist*.exe\"
} else:mac {
	qtPrepareTool(QMAKE_MACDEPLOYQT, macdeployqt)
	deploy_target.commands += $$QMAKE_MACDEPLOYQT \"$(INSTALL_ROOT)$${deploy_target.binary}\"
	QMAKE_MACDEPLOYQT += $${deploy_target.extra_args}
} else: deploy_target.commands += echo noop

!isEmpty(deploy_target.binary): QMAKE_EXTRA_TARGETS += deploy_target
