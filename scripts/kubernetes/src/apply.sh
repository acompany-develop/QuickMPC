#!/bin/bash
set -e

# TODO nパーティ対応
cluster_name=${GKE_CLUSTER_NAME}0
kubectl config use-context gke_${PROJECT_ID}_${GCP_ZONE}_${cluster_name}
cd ../manifests/parties/parties/1
kubectl apply -R -f ./

cluster_name=${GKE_CLUSTER_NAME}1
kubectl config use-context gke_${PROJECT_ID}_${GCP_ZONE}_${cluster_name}
cd ../2
kubectl apply -R -f ./

cluster_name=${GKE_CLUSTER_NAME}2
kubectl config use-context gke_${PROJECT_ID}_${GCP_ZONE}_${cluster_name}
cd ../../../others/beaver-triple-service
kubectl apply -R -f ./
