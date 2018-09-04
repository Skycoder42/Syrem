#!/bin/bash
set -e

curl -Lo "openssl.tar.xz" "https://github.com/Skycoder42/ci-builds/releases/download/${OPENSSL_VERSION}/openssl_${OPENSSL_VERSION}_${PLATFORM}.tar.xz"
tar -xf openssl.tar.xz
rm -f openssl.tar.xz
