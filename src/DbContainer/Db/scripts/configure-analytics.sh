#!/bin/bash

bucketName=$1

# Run Analytics Dataset scripts

if [ -e "/startup/$bucketName/analytics/dataset.json" ]; then
  echo "Creating datasets on $bucketName..."

  while read statement
  do
    echo "Dataset Statement: $statement"
    curl -Ss -u $CB_USERNAME:$CB_PASSWORD http://localhost:8095/analytics/service --data-urlencode "statement=$statement"
  done < <(cat /startup/$bucketName/analytics/dataset.json | jq -r '.statements | .[]')
fi

# Run Analytics index scripts

if [ -e "/startup/$bucketName/analytics/indexes.json" ]; then
  echo "Building analytics indexes on $bucketName..."

  while read statement
  do
    echo "Index Statement: $statement"
    curl -Ss -u $CB_USERNAME:$CB_PASSWORD http://localhost:8095/analytics/service --data-urlencode "statement=$statement"
  done < <(cat /startup/$bucketName/analytics/indexes.json | jq -r '.statements | .[]')
fi
