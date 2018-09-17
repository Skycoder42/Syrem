win32: DEPLOY_BINS = "$$INSTALL_BINS/$${PROJECT_TARGET}.exe" "$$INSTALL_BINS/$${PROJECT_TARGET}d.exe"
else:mac: DEPLOY_BINS = "$$PREFIX/$${PROJECT_NAME}.app" "$$PREFIX/$${APP_PREFIX}/MacOs/syremd"

# begin deploy script
deploy_target.target = deploy
deploy_target.depends += install
QMAKE_EXTRA_TARGETS += deploy_target

!isEmpty(DEPLOY_BINS) {
	run_deploy.target = run-deploy-tools
	run_deploy.depends += install
	QMAKE_EXTRA_TARGETS += run_deploy
	deploy_target.depends += run-deploy-tools

	win32 {
		qtPrepareTool(QMAKE_WINDEPLOYQT, windeployqt)
		CONFIG(release, debug|release): QMAKE_WINDEPLOYQT += --release
		CONFIG(debug, debug|release): QMAKE_WINDEPLOYQT += --debug
		QMAKE_WINDEPLOYQT += --no-translations $$DEPLOY_EXTRA_ARGS

		for(bin, DEPLOY_BINS): run_deploy.commands += $$QMAKE_WINDEPLOYQT \"$(INSTALL_ROOT)$$bin\"$$escape_expand(\n\t)
	} else:mac {
		qtPrepareTool(QMAKE_MACDEPLOYQT, macdeployqt)
		BINS_COPY = $$DEPLOY_BINS
		run_deploy.commands += $$QMAKE_MACDEPLOYQT \"$(INSTALL_ROOT)$$take_first(BINS_COPY)\"
		for(bin, BINS_COPY): run_deploy.commands += \"-executable=$(INSTALL_ROOT)$$bin\"
		run_deploy.commands += $${deploy_target.extra_args}$$escape_expand(\n\t)
	}
}
