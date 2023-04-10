#!/bin/bash
cd `dirname $0`

# pipenv がinstallされていなかったらexit
PIP_LIST=`pip list 2> /dev/null`
if [[ ! $PIP_LIST =~ "pipenv" ]]; then
  echo "[ERROR!] container_test needs 'pipenv'"
  exit 1
fi

# 環境を構築してTestを走らせる
pipenv sync
pipenv install --skip-lock ../../packages/client/libclient-py
pipenv run pytest ./tests -s -v -log-cli-level=DEBUG
