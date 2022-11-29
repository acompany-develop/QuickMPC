#!/bin/bash

set -uo pipefail

KERNEL_CORE_PATTERN_FILE="/proc/sys/kernel/core_pattern"

SCRIPT_DIR=$(cd "$(dirname "$0")" || exit 1; pwd)
CORE_PATTERN_FILE="${SCRIPT_DIR}/data/core_pattern.txt"
GDB_CMD_FILE="${SCRIPT_DIR}/data/gdb_print_stacktrace_cmd.txt"

function core_pattern_check() {
  result="$(grep "^$(tr -d '\n' < "${CORE_PATTERN_FILE}")$" "${KERNEL_CORE_PATTERN_FILE}")"

  if [ -z "${result}" ]
  then
    echo "WARNING: ${KERNEL_CORE_PATTERN_FILE} does not match what QuickMPC expects."
    echo "WARNING: Please run ${SCRIPT_DIR}/host_core_setting.sh on host machine."

    return 1
  fi

  return 0
}

function run()
{
  env "${@:1:$(($#-1))}" "${@: -1}"

  status=$?

  if [ $status -eq 0 ]
  then
    return 0
  fi

  echo "INFO: ${*: -1} exit with code: ${status}"

  if ! core_pattern_check
  then
    return 1
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

  if [[ "$core_file" == "" ]]
  then
    echo 'WARNING: core file was not found'
    return 1
  fi

  gdb "${executable_file}" "${core_file}" -x "${GDB_CMD_FILE}" -batch -quiet
}


if [ $# -le 0 ]
then
  echo "Usage: $0 [env_setting]... [executable_file]"
  echo "Example: $0 ./bazel-bin/computation_container"
  exit 1
fi

core_pattern_check

while true
do
  run "$@"
done
