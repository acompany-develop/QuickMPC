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
    curl

ENV PATH $PATH:/root/go/bin
ENV PATH $PATH:/usr/local/go/bin

# Install go
RUN wget https://go.dev/dl/go1.18.6.linux-amd64.tar.gz && \
    tar -C /usr/local -xzf go1.18.6.linux-amd64.tar.gz

# grpcサーバのhealthcheckをするためのツールをインストール
RUN GRPC_HEALTH_PROBE_VERSION=v0.4.13 && \
    wget -qO/bin/grpc_health_probe https://github.com/grpc-ecosystem/grpc-health-probe/releases/download/${GRPC_HEALTH_PROBE_VERSION}/grpc_health_probe-linux-amd64 && \
    chmod +x /bin/grpc_health_probe

# docker-composeから環境変数受け取り
# 例) lowercase_DB_NAMES=sharedb
ARG lowercase_DB_NAMES
# ENTRYPOINTで使えるようにENVで登録
ENV lowercase_DB_NAMES=${lowercase_DB_NAMES}


FROM base as src
# goの自動成型ツール
RUN go install -v github.com/ramya-rao-a/go-outline@latest
# goの自動成型ツールimportが勝手に消されるのでお好みかも．．．
RUN go install -v golang.org/x/tools/gopls@latest

# Install Protocolbuf
RUN apt-get update && \
    apt-get install -y unzip && \
    PROTOC_VER=3.18.0 && \
    curl -LO https://github.com/protocolbuffers/protobuf/releases/download/v${PROTOC_VER}/protoc-${PROTOC_VER}-linux-x86_64.zip && \
    unzip protoc-${PROTOC_VER}-linux-x86_64.zip -d /root/.local && \
    go install -v google.golang.org/protobuf/cmd/protoc-gen-go@v1.28.0 && \
    go install -v google.golang.org/grpc/cmd/protoc-gen-go-grpc@v1.2.0

ENV PATH=$PATH:/root/.local/bin

# 動的に生成したユーザ名とパスワードをsecrets-serverからネットワーク経由で受け取り、環境変数に設定して初期化&サーバ立ち上げ
# ドキュメント: Docs/DB-account-name-and-password-settings.md
COPY ./src/DbContainer/.bashrc /root/
WORKDIR /QuickMPC


FROM base as builder
COPY src/DbContainer/DbGate /QuickMPC
# github actionsで使用されているdockerがCOPYを2連続で行うとたまにエラーが出る問題を抱えている
# その問題を回避するために、COPYの間にRun trueを挟んでいる
RUN true
COPY src/Proto/ /Proto
WORKDIR /QuickMPC
RUN go mod vendor && go build

FROM builder as dev
COPY ./src/DbContainer/.bashrc /root/
WORKDIR /QuickMPC

# image for deploy
FROM builder as dep
COPY ./src/DbContainer/.bashrc /root/
WORKDIR /QuickMPC

FROM alpine:3.15.1 as dep-runner
RUN apk update && \
    apk add curl && \
    # .bashrcのログイン時実行を行いためbashを入れる
    apk add --no-cache bash && \
    # CVE-2022-0778 対策
    # 公式イメージの upgrade がされていないため
    apk upgrade
ARG lowercase_DB_NAMES
ENV lowercase_DB_NAMES=${lowercase_DB_NAMES}
ENV BASH_ENV="/root/.bashrc"
RUN true
COPY --from=builder /QuickMPC/DbGate /QuickMPC/DbGate
RUN mkdir /lib64 && ln -s /lib/libc.musl-x86_64.so.1 /lib64/ld-linux-x86-64.so.2
RUN true
COPY --from=ghcr.io/grpc-ecosystem/grpc-health-probe:v0.4.13 /ko-app/grpc-health-probe /bin/grpc_health_probe
RUN true
COPY ./src/DbContainer/.bashrc /root/
WORKDIR /QuickMPC

FROM builder as small
ENV PARTY_ID=1
COPY Config/DbContainer/DbGate${PARTY_ID} /QuickMPC/Config
RUN true
COPY ./src/DbContainer/.bashrc /root/
WORKDIR /QuickMPC

FROM builder as medium
COPY ./src/DbContainer/.bashrc /root/
WORKDIR /QuickMPC

FROM builder as large
COPY ./src/DbContainer/.bashrc /root/
WORKDIR /QuickMPC
