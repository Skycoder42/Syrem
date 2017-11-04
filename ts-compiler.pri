isEmpty(LRELEASE): qtPrepareTool(LRELEASE, lrelease)

extra_translate.name = $$LRELEASE translate ${QMAKE_FILE_IN}
extra_translate.input = EXTRA_TRANSLATIONS
extra_translate.variable_out = TRANSLATIONS_QM
extra_translate.commands = $$LRELEASE $$shell_quote(${QMAKE_FILE_IN}) -qm $$shell_quote(${QMAKE_FILE_OUT})
extra_translate.output = $$OUT_PWD/${QMAKE_FILE_BASE}.qm
extra_translate.CONFIG += no_link
QMAKE_EXTRA_COMPILERS += extra_translate

extra_translate_target.target = lrelease-extra
win32:!ReleaseBuild:!DebugBuild: {
	extra_translate_subtarget.target = lrelease-subtarget
	extra_translate_subtarget.CONFIG += recursive
	extra_translate_subtarget.recurse_target = lrelease-extra
	QMAKE_EXTRA_TARGETS += extra_translate_subtarget

	CONFIG(debug, debug|release): extra_translate_target.depends += debug-extra_translate_subtarget
	CONFIG(release, debug|release): extra_translate_target.depends += release-extra_translate_subtarget
} else: extra_translate_target.depends += compiler_extra_translate_make_all
extra_translate_target.commands =
lrelease.depends += extra_translate_target
QMAKE_EXTRA_TARGETS += extra_translate_target lrelease

extra_ts_target.CONFIG += no_check_exist
extra_ts_target.path = $$TS_INSTALL_DIR
#extra_ts_target.files = $$TRANSLATIONS_QM
for(tsfile, EXTRA_TRANSLATIONS) {
	tsBase = $$basename(tsfile)
	extra_ts_target.files += "$$OUT_PWD/$$replace(tsBase, \\.ts, .qm)"
}
