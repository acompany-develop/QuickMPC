#!/bin/bash

bucketName=$1

# Create FTS indexes

if [ -e "/startup/$bucketName/fts" ]; then
  echo "Creating FTS indexes on $bucketName..."

  for filename in /startup/$bucketName/fts/*.json; do
    indexName=$(basename $filename .json)

    if [ "$indexName" != "aliases" ]; then
      echo "Creating FTS index $indexName..."

      # Replace the bucket name and index name
      # and strip the UUIDs from the file before creating the index

      cat $filename |
        jq -c ".name = \"$indexName\" | .sourceName = \"$bucketName\" | del(.uuid) | del(.sourceUUID)" |
        curl -Ss -X PUT -u "$CB_USERNAME:$CB_PASSWORD" -H "Content-Type: application/json" \
          -d @- http://127.0.0.1:8094/api/index/$indexName
    fi
  done

  # Create FTS aliases

  if [ -e "/startup/$bucketName/fts/aliases.json" ]; then
    while read aliasPair
    do
      # aliasPair will have form key=["value1","value2"], where key is alias name and values are target indexes
      # So make aliasPair an array split by =
      aliasPair=(${aliasPair//=/ })

      jsonBody=$(jq -c ".name = \"${aliasPair[0]}\"" /scripts/fts_alias_template.json)

      # Add a target for each one in the JSON array on the right side of the pair
      while read -r target; do
        jsonBody=$(echo $jsonBody | jq -c ".params.targets.$target = {}")
      done < <(echo ${aliasPair[1]} | jq -r ".[]")

      echo "Creating FTS alias ${aliasPair[0]}..."

      echo $jsonBody |
        curl -Ss -X PUT -u "$CB_USERNAME:$CB_PASSWORD" -H "Content-Type: application/json" \
          -d @- http://127.0.0.1:8094/api/index/${aliasPair[0]}
    done < <(cat /startup/$bucketName/fts/aliases.json | jq -r "to_entries|map(\"\(.key)=\(.value|tostring)\")|.[]")
  fi
fi
