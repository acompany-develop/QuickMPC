# QuickMPCMPCをkubernetes上で動かす手順

下記では、ローカル環境と kubeadm で構築した kubernetes 環境で QuickMPC を動かす手順を記載します。

## local kubernetes 環境

ローカル環境には kubernetes distribution の一つである [k3s](https://k3s.io/) と、k3s を Docker 内で動かすツール [k3d](https://k3d.io/) を用います。

### 2PT 構成における kubernetes 環境を用意する

この例では 2PT と BTS の kubernetes 環境を用意するために、k3d を用いて k3s を 3 台分立ち上げます。k3s 環境を分けるのは、それぞれ管理者が異なるという想定の元です。

まずは、各 kubernetes 環境が互いに通信が行えるように Docker の bridge network を作成します。

```console
$ docker network create k3d-bridge
```

次に 3 台の k3s 環境を立ち上げます。

```console
$ k3d cluster create --volume $(pwd)/config/parties/1:/opt/QuickMPC/config --network k3d-bridge party1
$ k3d cluster create --volume $(pwd)/config/parties/2:/opt/QuickMPC/config --network k3d-bridge party2
$ k3d cluster create --volume $(pwd)/config/others:/opt/QuickMPC/config --network k3d-bridge bts
```

これにより、以下の kubernetes context が作成されます。

```console
$ kubectl config get-contexts
CURRENT   NAME         CLUSTER      AUTHINFO           NAMESPACE
          k3d-bts      k3d-bts      admin@k3d-bts      
          k3d-party1   k3d-party1   admin@k3d-party1   
          k3d-party2   k3d-party2   admin@k3d-party2   
```

## kubeadm で構築する kubernetes 環境
