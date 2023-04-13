#!/bin/bash -eu

# start kubeadm
sudo kubeadm init

# setup config
mkdir -p $HOME/.kube
sudo cp -f /etc/kubernetes/admin.conf ~/.kube/config
sudo chown $(id -g):$(id -u) ~/.kube/config

# untaint
kubectl taint nodes --all node-role.kubernetes.io/master:NoSchedule-
kubectl taint nodes --all node-role.kubernetes.io/control-plane:NoSchedule-

# install and deploy cilium
helm install cilium cilium/cilium --version 1.11.5 --namespace kube-system
curl -L --remote-name-all https://github.com/cilium/cilium-cli/releases/latest/download/cilium-linux-amd64.tar.gz{,.sha256sum}
sudo tar xzvfC cilium-linux-amd64.tar.gz /usr/local/bin
rm cilium-linux-amd64.tar.gz{,.sha256sum}

# install cert-manager
kubectl apply -f https://github.com/cert-manager/cert-manager/releases/download/v1.9.1/cert-manager.yaml

# install ingress-nginx
kubectl apply -f https://raw.githubusercontent.com/kubernetes/ingress-nginx/controller-v1.4.0/deploy/static/provider/baremetal/deploy.yaml
