#!/bin/bash

set -eu

# クエリが実行できるかどうかで Health Check を行う
# cbq 自身はエラーステータスを設定しないため jq を用いる
function healthcheck() {
    status=$( \
        cbq -user $CB_USERNAME -password $CB_PASSWORD \
            -quiet \
            -script 'select * from `'$1'` limit 1' \
        | jq -r .status \
    )

    if [ !"$status" = 'success' ]; then
      exit 1
    fi
}

healthcheck share
healthcheck result

