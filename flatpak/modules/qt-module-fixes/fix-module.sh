#!/bin/sh
# $1 module dir
# $2.. modules:tools
set -e

rootdir="/app/$1"
usrdir="$rootdir/usr"
specdir="$usrdir/lib/mkspecs"
moddir="$specdir/modules"
instdir="$specdir/modules-inst"
shift

mkdir -p "$instdir"
for modList in "$@"; do
	toolList=(${modList//:/ })
	mod=${toolList[0]}
	unset toolList[0]

	modpub="$moddir/qt_lib_${mod}.pri"
	modpriv="$moddir/qt_lib_${mod}_private.pri"

	mv "$modpub" "$instdir/"
	echo 'include($$PWD/qt_flatpak_paths.pri)' > "$modpub"
	echo "include(\$\$PWD/../modules-inst/qt_lib_${mod}.pri)" >> "$modpub"
	echo "QT.${mod}.priority = 1" >> "$modpub"

	mv "$modpriv" "$instdir/"
	echo 'include($$PWD/qt_flatpak_paths.pri)' > "$modpriv"
	echo "include(\$\$PWD/../modules-inst/qt_lib_${mod}_private.pri)" >> "$modpriv"
	echo "QT.${mod}_private.priority = 1" >> "$modpriv"
	
	for tool in ${toolList[*]}; do
		modtool="$moddir/qt_tool_${tool}.pri"
		echo "QT_TOOL.$tool.binary = /app/bin/$tool" > "$modtool"
		echo "QT_TOOL.$tool.depends = core" >> "$modtool"
	done
done

cp -ar "$usrdir/." /app/
rm -rf "$rootdir"
