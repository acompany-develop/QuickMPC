#!/bin/bash
set -m

/entrypoint.sh couchbase-server &

if [ ! -e "/nodestatus/initialized" ]; then
  export CB_VERSION=$(cat /opt/couchbase/VERSION.txt | grep -o "^[0-9]*\.[0-9]*\.[0-9]*")
  echo "CB_VERSION=$CB_VERSION"

  scriptPath=$(dirname $(realpath $0))

  $scriptPath/init-node.sh
  if [ $? -ne 0 ]; then
    rm -f /nodestatus/initialized
    echo "初期化失敗"
    exit 1
  fi

  $scriptPath/wait-for-services.sh
  if [ $? -ne 0 ]; then
    rm -f /nodestatus/initialized
    echo "初期化失敗"
    exit 1
  fi

  $scriptPath/create-buckets.sh
  if [ $? -ne 0 ]; then
    rm -f /nodestatus/initialized
    echo "初期化失敗"
    exit 1
  fi

  # $scriptPath/run-fakeit.sh

  while read bucketName; do
    $scriptPath/create-views.sh $bucketName
    if [ $? -ne 0 ]; then
      rm -f /nodestatus/initialized
      echo "初期化失敗"
      exit 1
    fi

    $scriptPath/create-n1ql-indexes.sh $bucketName
    if [ $? -ne 0 ]; then
      rm -f /nodestatus/initialized
      echo "初期化失敗"
      exit 1
    fi

    $scriptPath/create-fts-indexes.sh $bucketName
    if [ $? -ne 0 ]; then
      rm -f /nodestatus/initialized
      echo "初期化失敗"
      exit 1
    fi

    $scriptPath/configure-analytics.sh $bucketName
    if [ $? -ne 0 ]; then
      rm -f /nodestatus/initialized
      echo "初期化失敗"
      exit 1
    fi

  done < <(cat /startup/buckets.json | jq -r '.[].name')

  $scriptPath/create-events.sh
  if [ $? -ne 0 ]; then
    rm -f /nodestatus/initialized
    echo "初期化失敗"
    exit 1
  fi

  $scriptPath/create-rbac-users.sh
  if [ $? -ne 0 ]; then
    rm -f /nodestatus/initialized
    echo "初期化失敗"
    exit 1
  fi

  # Done
  echo "Couchbase Server initialized."
  echo "Initialized $(date +"%D %T")" >/nodestatus/initialized
else
  echo "Couchbase Server already initialized."
fi

# Wait for Couchbase Server shutdown
fg 1
