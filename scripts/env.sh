#!/usr/bin/env bash

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
PROJECT_DIR=$( realpath "$SCRIPT_DIR/.." )
BUILD_DIR="${PROJECT_DIR}/build"
