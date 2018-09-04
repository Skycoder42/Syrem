#!/bin/bash
set -e

currDir=$(dirname $0)

# install 7z
if [[ $TRAVIS_OS_NAME == "linux" ]]; then	
	# append post build script
	echo "$currDir/travis_postbuild.sh" >> qtmodules-travis/ci/linux/build-docker.sh
fi

# install openssl for android
if [[ $PLATFORM == "android_"* ]]; then
	pushd gui/quick/openssl
	./get_openssl.sh
	popd
fi
