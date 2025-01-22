#!/bin/bash

VCPKG_ROOT_UNIX=$(cygpath -u "$VCPKG_ROOT")
cmake . -B build/ -DCMAKE_TOOLCHAIN_FILE=$VCPKG_ROOT_UNIX/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-windows #lazy
