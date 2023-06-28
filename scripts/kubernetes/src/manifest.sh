#!/bin/bash
set -e
party_list=(${PARTY_LIST//,/ })

# TODO nパーティ対応
# マニフェストのコピー
cp -r ../manifests/parties/common/* ../manifests/parties/parties/1
cp -r ../manifests/parties/common/* ../manifests/parties/parties/2

sed -e "s/{ip2}/${party_list[1]}/g; s/{ip3}/${party_list[2]}/g" ../manifests/parties/parties/1/config-parties-1-computation-container--env-configmap.yaml.template > ../manifests/parties/parties/1/config-parties-1-computation-container--env-configmap.yaml
sed -e "s/{ip1}/${party_list[0]}/g; s/{ip3}/${party_list[2]}/g" ../manifests/parties/parties/2/config-parties-2-computation-container--env-configmap.yaml.template > ../manifests/parties/parties/2/config-parties-2-computation-container--env-configmap.yaml
sed -e "s/{ip2}/${party_list[1]}/g" ../manifests/parties/parties/1/config-parties-1-manage-container--env-configmap.yaml.template > ../manifests/parties/parties/1/config-parties-1-manage-container--env-configmap.yaml
sed -e "s/{ip1}/${party_list[0]}/g" ../manifests/parties/parties/2/config-parties-2-manage-container--env-configmap.yaml.template > ../manifests/parties/parties/2/config-parties-2-manage-container--env-configmap.yaml
