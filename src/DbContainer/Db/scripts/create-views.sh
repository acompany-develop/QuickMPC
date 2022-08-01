#!/bin/bash

# Create view design documents

bucketName=$1

if [ -e "/startup/$bucketName/views.json" ]; then
  echo "Building views on $bucketName..."

  while read designDocName
  do
    cat /startup/$bucketName/views.json |
      jq -r ".$designDocName" |
        curl -Ss -X PUT -u "$CB_USERNAME:$CB_PASSWORD" -H "Content-Type: application/json" \
          -d @- http://127.0.0.1:8092/$bucketName/_design/$designDocName
  done < <(cat /startup/$bucketName/views.json | jq -r 'keys[]')
fi
