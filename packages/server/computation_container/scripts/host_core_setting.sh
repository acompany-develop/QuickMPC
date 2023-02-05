#!/bin/bash

set -euxo pipefail

SCRIPT_DIR=$(cd "$(dirname "$0")"; pwd)
CORE_PATTERN_FILE="${SCRIPT_DIR}/data/core_pattern.txt"

# core file size limit setting
# `-c` option is undefined in POSIX shell, so this script use `bash`
ulimit -c unlimited

# core file name setting
CORE_FILE=$(cat "${CORE_PATTERN_FILE}")
echo "${CORE_FILE}" > /proc/sys/kernel/core_pattern
