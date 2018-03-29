#!/bin/bash
set -e

echo 'add-apt-repository ppa:deadsnakes/ppa' >> ./qtmodules-travis/ci/$TRAVIS_OS_NAME/setup/setup-common.sh
echo 'apt-get -qq update' >> ./qtmodules-travis/ci/$TRAVIS_OS_NAME/setup/setup-common.sh
echo 'apt-get -qq install python3.6' >> ./qtmodules-travis/ci/$TRAVIS_OS_NAME/setup/setup-common.sh
echo 'rm /usr/bin/python3' >> ./qtmodules-travis/ci/$TRAVIS_OS_NAME/setup/setup-common.sh
echo 'ln -s python3.6 /usr/python3' >> ./qtmodules-travis/ci/$TRAVIS_OS_NAME/setup/setup-common.sh

if [[ "$PLATFORM" == "android"* ]]; then
	./gui/RemindMeQuick/openssl/openssl.sh
fi
