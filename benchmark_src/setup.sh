#!/bin/bash
set -e
# ./setup.sh image_tag encode_service_account_json project_id
image_tag=$1  # パーティの番号を引数から取得
service_account_json=$2
project_id=$3

# tagの書き換え
sed "s/{tag}/${image_tag}/g" ./template/application/variable.tf.template > ./template/application/variable.tf
# project_idの書き換え
sed "s/{project_id}/${project_id}/g" ./template/gcp/variables.tf.template > ./template/gcp/variables.tf


# encodeされたjsonキーをjsonファイルに変換
echo "${service_account_json}" | base64 -d > ./service_account.json

# 必要なファイルを適切なディレクトリに配置
cp ./template/application/variable.tf ../demo/terraform/application/variable.tf
cp ./template/gcp/variables.tf ../demo/terraform/gcp/variables.tf
cp ./service_account.json ../demo/terraform/gcp/json/service_account.json
