#!/bin/bash
# コンテナを起動し、終了次第削除するスクリプト
set -e

function urldecode() { : "${*//+/ }"; echo -e "${_//%/\\x}"; }

job_id="$1"

sed -e "s/original/${job_id}/" ../docker-compose.yml > "../docker-compose-${job_id}.yml" &&
docker-compose -f "../docker-compose-${job_id}.yml" up compute &&
docker rm "computation_container"
docker rmi "ghcr.io/acompany-develop/cc_test:${job_id}"
rm "../docker-compose-${job_id}.yml"