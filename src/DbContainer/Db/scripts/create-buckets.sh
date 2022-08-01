#!/bin/bash

# Create buckets defined in /startup/buckets.json
echo "Creating buckets..."

while read bucketSettings
do
  curl -Ss http://127.0.0.1:8091/pools/default/buckets -u "$CB_USERNAME:$CB_PASSWORD" $bucketSettings
done < <(cat /startup/buckets.json | jq -r '.[] | to_entries | map(.key + "=" + (.value | tostring)) | @sh "-d " + join(" -d ")')

# Wait for the buckets to be healthy
bucketCount=$(cat /startup/buckets.json | jq -r '.[].name' | wc -l)
until [ `curl -Ss http://127.0.0.1:8091/pools/default/buckets -u $CB_USERNAME:$CB_PASSWORD | \
         jq -r .[].nodes[].status | grep '^healthy$' | wc -l` -eq $bucketCount ];
do
  echo "Waiting for bucket initialization..."
  sleep 1
done

echo "Couchbase Version: $CB_VERSION"
if [[ $CB_VERSION > "6." ]]; then
  echo "About to create scopes and collections..."

  scriptPath=$(dirname $(realpath $0))
  # Scopes and collections need created after a bucket is up and running
  while read bucketName
  do
    $scriptPath/create-collections.sh $bucketName
  done < <(cat /startup/buckets.json | jq -r '.[].name')
fi
