#!/bin/bash
# $1 local path
# $2 git repo
# $3 git tag
set -e

export QPMX_CACHE_DIR="$(realpath "$1")"
mkdir -p "$QPMX_CACHE_DIR"
tdir=$(mktemp -d)
pushd $tdir

git clone --depth 1 --branch "$3" "$2" .
for qpmxfile in $(find . -name "qpmx.json"); do
	echo $qpmxfile
	qpmx install --cache --no-prepare --dir $(dirname "$qpmxfile")
done
