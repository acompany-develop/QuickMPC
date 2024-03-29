#!/bin/bash
# ./prepare.sh 127.0.0.1,127.0.0.2 version
set -e
party_str=$1          # パーティのipリストを引数から取得
version=$2
party_list=(${party_str//,/ })
printf 'PARTY_SIZE=%s\n' "${#party_list[@]}" > ./.env
for ((i=1; i<=(${#party_list[@]}); i++)); do
    printf 'PARTY%s=%s\n' "${i}" "${party_list[$((i-1))]}" >> ./.env
done
printf 'QMPC_VERSION=%s\n' "${version}" >> ./.env

# install gcsfuse
export GCSFUSE_REPO=gcsfuse-`lsb_release -c -s`
echo "deb https://packages.cloud.google.com/apt $GCSFUSE_REPO main" | sudo tee /etc/apt/sources.list.d/gcsfuse.list
curl https://packages.cloud.google.com/apt/doc/apt-key.gpg | sudo apt-key add -
sudo apt-get update
sudo apt-get install gcsfuse


ssh_user=$(whoami)
mkdir /home/${ssh_user}/test/data

# gcsを./dataにmount
export GOOGLE_APPLICATION_CREDENTIALS=/home/${ssh_user}/test/service_account.json
gcsfuse benchmark-ci-data /home/${ssh_user}/test/data

# NOTE: gcsをmountしたディレクトリがdockerにmountできなかったためディレクトリを作成している
# TODO 直接mountできるようにする
mkdir ./test_data
cp ./data/* ./test_data
