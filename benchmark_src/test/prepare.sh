#!/bin/bash
party_str=$1          # パーティのipリストを引数から取得
party_list=(${party_str//,/ })
printf 'PARTY_SIZE=%s\n' "${#party_list[@]}" > ./.env
for ((i=1; i<=(${#party_list[@]}); i++)); do
    printf 'PARTY%s=%s\n' "${i}" "${party_list[$((i-1))]}" >> ./.env
done

# install gcsfuse
export GCSFUSE_REPO=gcsfuse-`lsb_release -c -s`
echo "deb http://packages.cloud.google.com/apt $GCSFUSE_REPO main" | sudo tee /etc/apt/sources.list.d/gcsfuse.list
curl https://packages.cloud.google.com/apt/doc/apt-key.gpg | sudo apt-key add -
sudo apt-get update
sudo apt-get install gcsfuse
