#!/bin/sh
# $1 name
# $2 basepath
# $2.. sizes
set -e

name=$1
base=$2
shift 2

outBase=icon_export/hicolor
mkdir -p $outBase
for size in "$@"; do
	sizeDir=$outBase/${size}x${size}/apps
	mkdir -p $sizeDir
	cp -afu "${base}_${size}.png" "$sizeDir/${name}.png"
done
