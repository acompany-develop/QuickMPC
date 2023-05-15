#!/bin/bash

set -e

# ----------------------------------------------------------------
#     Check environment
# ----------------------------------------------------------------
if [ "$(expr substr $(uname -s) 1 5)" != 'Linux' ]; then
  echo "Please use Ubuntu 20.04"
  exit 1
fi

if [ "$(lsb_release -rs)" != "20.04" ]; then
  echo "Please use Ubuntu 20.04"
  exit 1
fi

# ----------------------------------------------------------------
#     Setup Kubernetes
# ----------------------------------------------------------------
# ref: https://kubernetes.io/ja/docs/setup/production-environment/tools/kubeadm/install-kubeadm/

## [Omitted] Verification that MAC address and product_uuid are unique for all nodes

## Allow iptables to handle traffic passing through the bridge
sudo modprobe br_netfilter

cat <<EOF | sudo tee /etc/sysctl.d/k8s.conf
net.bridge.bridge-nf-call-ip6tables = 1
net.bridge.bridge-nf-call-iptables = 1
EOF
sysctl --system

## Prevent iptables from using the nftables backend
sudo apt-get update && sudo apt-get install -y iptables arptables ebtables

sudo update-alternatives --set iptables /usr/sbin/iptables-legacy
sudo update-alternatives --set ip6tables /usr/sbin/ip6tables-legacy
sudo update-alternatives --set arptables /usr/sbin/arptables-legacy
sudo update-alternatives --set ebtables /usr/sbin/ebtables-legacy

# ----------------------------------------------------------------
#     Install container runtime
# ----------------------------------------------------------------
sudo apt-get update
sudo apt-get install -y \
    apt-transport-https \
    ca-certificates \
    curl \
    software-properties-common \
		make
curl -fsSL https://download.docker.com/linux/ubuntu/gpg | sudo apt-key add -
sudo add-apt-repository \
   "deb [arch=amd64] https://download.docker.com/linux/ubuntu \
   $(lsb_release -cs) \
   stable"
sudo apt-get install -y docker-ce
sudo curl -L "https://github.com/docker/compose/releases/download/1.29.2/docker-compose-$(uname -s)-$(uname -m)" -o /usr/local/bin/docker-compose
sudo chmod +x /usr/local/bin/docker-compose
sudo ln -s /usr/local/bin/docker-compose /usr/bin/docker-compose
sudo chmod 777 /var/run/docker.sock
sudo echo "export DOCKER_BUILDKIT=1" | sudo tee -a ~/.bashrc

# ----------------------------------------------------------------
#     Install kubeadm, kubelet, kubectl
# ----------------------------------------------------------------
curl -s https://packages.cloud.google.com/apt/doc/apt-key.gpg | sudo apt-key add -
cat <<EOF | sudo tee /etc/apt/sources.list.d/kubernetes.list
deb https://apt.kubernetes.io/ kubernetes-xenial main
EOF
sudo apt-get update && sudo apt-get install -y kubelet kubeadm kubectl
sudo apt-mark hold kubelet kubeadm kubectl

echo "[INFO] kubeadm, kubelet, kubectl installed successfully!"

sudo mv /etc/containerd/config.toml /etc/containerd/tmp.toml # Move /etc/containerd/config.toml because https://github.com/containerd/containerd/issues/4581
sudo systemctl restart containerd

### Helm install
curl -fsSL -o get_helm.sh https://raw.githubusercontent.com/helm/helm/main/scripts/get-helm-3
chmod 700 get_helm.sh
./get_helm.sh

### Deploy cilium
helm repo add cilium https://helm.cilium.io/

# ----------------------------------------------------------------
#     Install bash-completion
# ----------------------------------------------------------------
sudo apt-get update && sudo apt-get install bash-completion
echo "source <(kubectl completion bash)" >> ~/.bashrc

echo "Please exec 'source ~/.bashrc'"
