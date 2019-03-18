#!/usr/bin/env bash

BUILD_TYPE="Release"
ROOT_DIR=$(git rev-parse --show-toplevel)
DIST_DIR="${ROOT_DIR}/dist/windows"
BUILD_DIR="${ROOT_DIR}/src/windows/cmake-build-release"
TOOLCHAIN_FILE="${ROOT_DIR}/src/windows/toolchains/linux-mingw32.cmake"

mkdir -p ${BUILD_DIR} && cd ${BUILD_DIR}
cmake .. -DCMAKE_TOOLCHAIN_FILE=${TOOLCHAIN_FILE} -DCMAKE_BUILD_TYPE=${BUILD_TYPE} -DCMAKE_INSTALL_PREFIX=${DIST_DIR}
make install
