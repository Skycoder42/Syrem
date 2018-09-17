win32: DEPLOY_BINS = "$$INSTALL_BINS/$${PROJECT_TARGET}.exe" "$$INSTALL_BINS/$${PROJECT_TARGET}d.exe"
else:mac: DEPLOY_BINS = "$$PREFIX/$${PROJECT_NAME}.app" "$$PREFIX/$${APP_PREFIX}/MacOs/syremd"
DEPLOY_PLUGINS += keystores
systemd_service: DEPLOY_PLUGINS += servicebackends

# begin deploy script
deploy_target.target = deploy
deploy_target.depends += install
QMAKE_EXTRA_TARGETS += deploy_target

!isEmpty(DEPLOY_BINS) {
	# run actual deployment tools
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

# deploy missing plugins (as part of make install)
for(plgdir, DEPLOY_PLUGINS) {
	plugin_fixup_$${plgdir}.path = $$INSTALL_PLUGINS/$$plgdir
	INSTALLS += plugin_fixup_$${plgdir}

	ALL_PLGS=$$files($$[QT_INSTALL_PLUGINS]/$$plgdir/*)
	for(plg, ALL_PLGS) {
		PLG_BASE = $$split(plg, ".")
		$$take_last(PLG_BASE)
		PLG_BASE = $$join(PLG_BASE, ".")
		win32: exists($${PLG_BASE}d.dll): PLG_ACTUAL += $${PLG_BASE}
		mac: exists($${PLG_BASE}_debug.dylib): PLG_ACTUAL += $${PLG_BASE}
		linux: exists($${PLG_BASE}.so.debug): PLG_ACTUAL += $${PLG_BASE}
	}

	for(PLG_BASE, PLG_ACTUAL) {
		win32:CONFIG(release, debug|release): PLG_PATH = $${PLG_BASE}.dll
		else:win32:CONFIG(debug, debug|release): PLG_PATH = $${PLG_BASE}d.dll
		else:mac:CONFIG(release, debug|release): PLG_PATH = $${PLG_BASE}.dylib
		else:mac:CONFIG(debug, debug|release): PLG_PATH = $${PLG_BASE}_debug.dylib
		else:linux: PLG_PATH = $${PLG_BASE}.so
		plugin_fixup_$${plgdir}.files += $$PLG_PATH
	}
}
