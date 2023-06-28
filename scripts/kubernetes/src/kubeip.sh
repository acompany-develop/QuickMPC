#!/bin/bash
set -e
cd kubeip

for ((i=0; i<$INSTANCE_COUNT; i++));
do
    git reset --hard origin/master
    cluster_name=${GKE_CLUSTER_NAME}${i}
    kubectl config use-context gke_${PROJECT_ID}_${GCP_ZONE}_${cluster_name}

    kubectl create secret generic kubeip-key --from-file=../json/key.json -n kube-system
    kubectl create clusterrolebinding cluster-admin-binding --clusterrole cluster-admin --user `gcloud config list --format 'value(core.account)'`

    gcloud beta compute addresses update ${INSTANCE_NAME}-ip$i --update-labels kubeip=$cluster_name --region $GCP_REGION

    # 間隔を1分間隔に変更
    sed -i -e "s/KUBEIP_TICKER: \"5\"/KUBEIP_TICKER: \"1\"/g" deploy/kubeip-deployment.yaml
    sed -i -e "s/reserved/$cluster_name/g; s/default-pool/$KUBEIP_NODEPOOL/g" deploy/kubeip-configmap.yaml
    sed -i -e "s/pool-kubip/$KUBEIP_SELF_NODEPOOL/g" deploy/kubeip-deployment.yaml
    kubectl apply -f deploy/.
done