#!/bin/bash
# コンテナを起動し、終了次第削除するスクリプト
set -e

job_id="$1"

sed -e "s/original/${job_id}/" ../docker-compose.yml > "../docker-compose-${job_id}.yml" &&
docker-compose -f "../docker-compose-${job_id}.yml" up compute > "cc_test_log_${job_id}" && # CCのテスト実行
# docker-compose -f "../docker-compose-${job_id}.yml" up -d --build secrets-server sharedb dbg &&
# docker-compose -f "../docker-compose-${job_id}.yml" up manage > "mc_test_log_${job_id}" && # MCのテスト実行
# docker stop "dbgate_container" &&
# docker-compose -f "../docker-compose-${job_id}.yml" up dbg-test > "dc_test_log_${job_id}" # DCのテスト実行
docker-compose -f "../docker-compose-${job_id}.yml" down &&
docker rmi $(docker images -q)
rm "../docker-compose-${job_id}.yml"
cd ../ && make rm-db