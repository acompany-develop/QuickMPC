FROM ubuntu:18.04 as base
# 環境変数
ENV CMAKE_INSTALL_DIR /local
ENV PATH $PATH:/grpc/cmake/build
ENV PATH $PATH:/grpc/cmake/build/third_party/protobuf
ENV PATH $PATH:$CMAKE_INSTALL_DIR/bin

RUN apt-get update && \
  apt-get install -y \
  git \
  wget \
  clang \
  ca-certificates \
  build-essential \
  libssl-dev \
  make \
  autoconf \
  automake \
  pkg-config \
  libtool \
  vim \
  cron \
  netcat \
  cmake

# grpcサーバのhealthcheckをするためのツールをインストール
RUN GRPC_HEALTH_PROBE_VERSION=v0.4.12 && \
  wget -qO/bin/grpc_health_probe https://github.com/grpc-ecosystem/grpc-health-probe/releases/download/${GRPC_HEALTH_PROBE_VERSION}/grpc_health_probe-linux-amd64 && \
  chmod +x /bin/grpc_health_probe

ENV PATH $PATH:/root/go/bin
ENV PATH $PATH:/usr/local/go/bin

# Install go
RUN wget https://go.dev/dl/go1.18.6.linux-amd64.tar.gz && \
  tar -C /usr/local -xzf go1.18.6.linux-amd64.tar.gz

FROM base as src
# goの自動成型ツール
RUN go install -v github.com/ramya-rao-a/go-outline@latest
# goの自動成型ツールimportが勝手に消されるのでお好みかも．．．
RUN go install -v golang.org/x/tools/gopls@latest

# Install Protocolbuf
RUN apt-get install -y unzip curl && \
  PROTOC_VER=3.18.0 && \
  curl -LO https://github.com/protocolbuffers/protobuf/releases/download/v${PROTOC_VER}/protoc-${PROTOC_VER}-linux-x86_64.zip && \
  unzip protoc-${PROTOC_VER}-linux-x86_64.zip -d /root/.local && \
  go install -v google.golang.org/protobuf/cmd/protoc-gen-go@v1.28.0 && \
  go install -v google.golang.org/grpc/cmd/protoc-gen-go-grpc@v1.2.0

ENV PATH=$PATH:/root/.local/bin

WORKDIR /QuickMPC

FROM base as builder
COPY src/ManageContainer/ /QuickMPC
# github actionsで使用されているdockerがCOPYを2連続で行うとたまにエラーが出る問題を抱えている
# その問題を回避するために、COPYの間にRun trueを挟んでいる
RUN true
COPY src/Proto/ /Proto
WORKDIR /QuickMPC
RUN go mod vendor && go build

FROM builder as dev

# image for deploy
FROM alpine:3.15.1 as dep-runner
RUN apk update && \
  apk add --no-cache bash && \
  # CVE-2022-0778 対策
  # 公式イメージの upgrade がされていないため
  apk upgrade
COPY --from=builder /QuickMPC/ManageContainer /QuickMPC/ManageContainer
RUN mkdir /lib64 && ln -s /lib/libc.musl-x86_64.so.1 /lib64/ld-linux-x86-64.so.2
COPY --from=ghcr.io/grpc-ecosystem/grpc-health-probe:v0.4.12 /ko-app/grpc-health-probe /bin/grpc_health_probe
WORKDIR /QuickMPC

FROM builder as small
ENV PARTY_ID=1
COPY Config/ManageContainer/Manage${PARTY_ID} /QuickMPC/Config

FROM builder as medium

FROM builder as large
WORKDIR /QuickMPC
