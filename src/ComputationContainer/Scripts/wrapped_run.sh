#!/bin/bash

set -euo pipefail

KERNEL_CORE_PATTERN_FILE="/proc/sys/kernel/core_pattern"

SCRIPT_DIR=$(cd "$(dirname "$0")"; pwd)
CORE_PATTERN_FILE="${SCRIPT_DIR}/data/core_pattern.txt"
GDB_CMD_FILE="${SCRIPT_DIR}/data/gdb_print_stacktrace_cmd.txt"

set +e
CORE_PATTERN_CHECK="$(grep "^$(tr -d '\n' < "${CORE_PATTERN_FILE}")$" "${KERNEL_CORE_PATTERN_FILE}")"
set -e

if [ -z "${CORE_PATTERN_CHECK}" ]
then
  echo "WARNING: ${KERNEL_CORE_PATTERN_FILE} does not match what QuickMPC expects."
  echo "WARNING: Please run ${SCRIPT_DIR}/host_core_setting.sh on host machine."
fi

if [ $# -le 0 ]
then
  echo "Usage: $0 [env_setting]... [executable_file]"
  echo "Example: $0 ./bazel-bin/computation_container"
  exit 1
fi

# run
set +e
env "${@:1:$(($#-1))}" "${@: -1}"

STATUS=$?
set -e

if [ $STATUS -eq 0 ]
then
  exit 0
fi

# core pattern check
if [ -z "${CORE_PATTERN_CHECK}" ]
then
  echo "WARNING: ${KERNEL_CORE_PATTERN_FILE} does not match what QuickMPC expects."
  echo "WARNING: core file analyzing is skipped."
  exit $STATUS
fi

# find executable file
executable_file=""

for arg in "$@"
do
  if [[ -x "$arg" ]]
  then
    executable_file="$arg"
  fi
done

# find latest core file
core_file=$(find /tmp/cores/ -maxdepth 1 -type f -exec stat -c '%Y %n' {} \; | sort -nr | awk 'NR==1,NR==3 {print $2}' | head -1)

if [ $STATUS -ne 0 ]
then
  gdb "${executable_file}" "${core_file}" -x "${GDB_CMD_FILE}" -batch -quiet
fi

exit $STATUS
