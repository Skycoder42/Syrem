DISTFILES += \
	$$PWD/qm-combine.py

# begin deploy script
deploy_target.target = deploy
deploy_target.depends += install
QMAKE_EXTRA_TARGETS += deploy_target

# deploy binaries
!isEmpty(DEPLOY_BINS) {
	# run actual deployment tools
	run_deploy.target = run-deploy-tools
	run_deploy.depends += install
	QMAKE_EXTRA_TARGETS += run_deploy
	deploy_target.depends += run-deploy-tools

	win32 {
		isEmpty(QMAKE_WINDEPLOYQT) {
			qtPrepareTool(QMAKE_WINDEPLOYQT, windeployqt)
			CONFIG(release, debug|release): QMAKE_WINDEPLOYQT += --release
			CONFIG(debug, debug|release): QMAKE_WINDEPLOYQT += --debug
			QMAKE_WINDEPLOYQT += --no-translations $$DEPLOY_EXTRA_ARGS
		}

		for(bin, DEPLOY_BINS): run_deploy.commands += $$QMAKE_WINDEPLOYQT \"$(INSTALL_ROOT)$$bin\"$$escape_expand(\n\t)
	} else:mac {
		isEmpty(QMAKE_MACDEPLOYQT): qtPrepareTool(QMAKE_MACDEPLOYQT, macdeployqt)
		BINS_COPY = $$DEPLOY_BINS
		run_deploy.commands += $$QMAKE_MACDEPLOYQT \"$(INSTALL_ROOT)$$take_first(BINS_COPY)\"
		for(bin, BINS_COPY): run_deploy.commands += \"-executable=$(INSTALL_ROOT)$$bin\"
		run_deploy.commands += $${deploy_target.extra_args}$$escape_expand(\n\t)
	}

	# deploy missing plugins (as part of make install)
	for(plgdir, DEPLOY_PLUGINS) {
		plugin_fixup_$${plgdir}.path = $$INSTALL_PLUGINS/$$plgdir
		INSTALLS += plugin_fixup_$${plgdir}

		ALL_PLGS=$$files($$[QT_INSTALL_PLUGINS]/$$plgdir/*)
		PLG_ACTUAL=
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
}

# deploy translations
!isEmpty(TS_DICTIONARIES) {
	QMAKE_EXTRA_TARGETS += install

	win32: QMAKE_QM_COMBINE = python
	QMAKE_QM_COMBINE += $$shell_path($$PWD/qm-combine.py)
	!no_deploy_qt_qm: QMAKE_QM_COMBINE += --deploy-qt
	QMAKE_QM_COMBINE += $$shell_path($$[QT_INSTALL_TRANSLATIONS])
	isEmpty(QMAKE_LRELEASE): qtPrepareTool(QMAKE_LRELEASE, lrelease)
	for(tsdict, TS_DICTIONARIES): install.commands += $$QMAKE_QM_COMBINE $$shell_quote($$tsdict) \"$(INSTALL_ROOT)$$INSTALL_TRANSLATIONS\" $$QMAKE_LRELEASE $$escape_expand(\n\t)
}
