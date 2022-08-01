#!/bin/bash

# Create and deploy events

if [ -e "/startup/events/" ]; then
  echo "Creating events..."

  for filename in /startup/$bucketName/events/*.json; do
    eventName=$(basename $filename .json)

    cat event_template.json |
      jq -c ".appname = \"$eventName\"" |
      jq --argfile source $filename -c ".depcfg = \$source.depcfg" |
      jq --argfile source $filename -c ".settings = .settings * \$source.settings" |
      jq --rawfile source /startup/events/$eventName.js -c ".appcode = \$source" |
      curl -Ss -X POST -u "$CB_USERNAME:$CB_PASSWORD" -H "Content-Type: application/json" \
        -d @- http://127.0.0.1:8096/api/v1/functions/$eventName \
      && echo
  done
fi
