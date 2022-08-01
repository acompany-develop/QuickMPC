#!/bin/bash

bucketName=$1

# Run index scripts

if [ -e "/startup/$bucketName/indexes.n1ql" ]; then
  echo "Building indexes on $bucketName..."

  /opt/couchbase/bin/cbq -e http://127.0.0.1:8093/ -u $CB_USERNAME -p $CB_PASSWORD -q=true -f="/startup/$bucketName/indexes.n1ql"

  # Wait for index build completion
  until [ $(/opt/couchbase/bin/cbq -e http://127.0.0.1:8093/ -u $CB_USERNAME -p $CB_PASSWORD -q=true \
    -s="SELECT COUNT(*) as unbuilt FROM system:indexes WHERE keyspace_id = '$bucket' AND state <> 'online'" |
    sed -n -e '/{/,$p' |
    jq -r '.results[].unbuilt') -eq 0 ]; do
    echo "Waiting for index build on $bucketName..."
    sleep 2
  done
fi

# Also check for couchbase-index-manager YAML index definitions

if [ -e "/startup/$bucketName/indexes/" ]; then
  echo "Building indexes on $bucketName..."

  if [[ $CB_VERSION < "5." ]]; then
    /scripts/node_modules/couchbase-index-manager/bin/couchbase-index-manager \
      -u $CB_USERNAME -p $CB_PASSWORD --no-rbac \
      sync -f $bucketName /startup/$bucketName/indexes/
  else
    /scripts/node_modules/couchbase-index-manager/bin/couchbase-index-manager \
      -u $CB_USERNAME -p $CB_PASSWORD \
      sync -f $bucketName /startup/$bucketName/indexes/
  fi
fi
