#!/bin/bash

set -uo pipefail

readonly KERNEL_CORE_PATTERN_FILE="/proc/sys/kernel/core_pattern"

SCRIPT_DIR=$(cd "$(dirname "$0")" || exit 1; pwd)
readonly SCRIPT_DIR
readonly CORE_PATTERN_FILE="${SCRIPT_DIR}/data/core_pattern.txt"
readonly GDB_CMD_FILE="${SCRIPT_DIR}/data/gdb_print_stacktrace_cmd.txt"

readonly JOB_RESULT_ROOT='/db/result'
readonly ERROR_JSON='{"what": "Unexpected error occured.  Contact your administrator."}'

function write_status_error()
{
  for job_result_dir in "${JOB_RESULT_ROOT}"/*
  do
    local completed_file_path="${job_result_dir}/status_COMPLETED"
    if [[ -f "$completed_file_path" ]]
    then
      continue
    fi

    local error_file_path="${job_result_dir}/status_ERROR"
    if [[ -f "$error_file_path" ]]
    then
      continue
    fi

    echo "$ERROR_JSON" > "$error_file_path"
  done
}

function core_pattern_check() {
  declare -r result="$(grep "^$(tr -d '\n' < "${CORE_PATTERN_FILE}")$" "${KERNEL_CORE_PATTERN_FILE}")"

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
  declare -r executable_file="${*: -1}"
  if [[ ! -x "$executable_file" ]]
  then
    echo "ERROR: ${executable_file} is not executable file"
    exit 1
  fi

  env "${@:1:$(($#-1))}" "${executable_file}"

  declare -r status=$?

  if [ $status -eq 0 ]
  then
    return 0
  fi

  echo "INFO: ${*: -1} exit with code: ${status}"

  if ! core_pattern_check
  then
    return 1
  fi

  # find latest core file
  declare -r core_file=$(find /tmp/cores/ -maxdepth 1 -type f -exec stat -c '%Y %n' {} \; | sort -nr | awk 'NR==1,NR==3 {print $2}' | head -1)

  if [[ -z "${core_file}" ]]
  then
    echo 'WARNING: core file was not found'
    return 1
  fi

  gdb "${executable_file}" "${core_file}" -x "${GDB_CMD_FILE}" -batch -quiet

  return $status
}


if [ $# -le 0 ]
then
  echo "Usage: $0 [env_setting]... [executable_file]"
  echo "Example: $0 ./bazel-bin/computation_container"
  exit 1
fi

core_pattern_check

write_status_error
run "$@"
declare -r status=$?
write_status_error

exit $status
