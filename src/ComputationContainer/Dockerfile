FROM ubuntu:18.04 as base
# 環境変数
ENV CMAKE_INSTALL_DIR /local
ENV PATH $PATH:/grpc/cmake/build
ENV PATH $PATH:/grpc/cmake/build/third_party/protobuf
ENV PATH $PATH:$CMAKE_INSTALL_DIR/bin

ARG TARGETPLATFORM

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
    libboost-all-dev \
    libeigen3-dev \
    vim \
    cron \
    curl \
    iputils-ping \
    net-tools \
    dnsutils

# Install cmake
RUN case "$TARGETPLATFORM" in \
    linux/amd64) ARCH=linux-x86_64 ;; \
    linux/arm64) ARCH=linux-aarch64 ;; \
    esac; \
    wget -q -O cmake-linux.sh https://github.com/Kitware/CMake/releases/download/v3.24.1/cmake-3.24.1-${ARCH}.sh && \
    mkdir -p $CMAKE_INSTALL_DIR && \
    sh cmake-linux.sh -- --skip-license --prefix=$CMAKE_INSTALL_DIR && \
    rm cmake-linux.sh

# Install bazelisk
RUN case "$TARGETPLATFORM" in \
    linux/amd64) ARCH=linux-amd64 ;; \
    linux/arm64) ARCH=linux-arm64 ;; \
    esac; \
    wget -q -O /bin/bazelisk https://github.com/bazelbuild/bazelisk/releases/download/v1.12.2/bazelisk-${ARCH} && \
    chmod +x /bin/bazelisk && \
    ln -s /bin/bazelisk /bin/bazel

# Install libsodium
RUN cd / && git clone https://github.com/jedisct1/libsodium --branch stable && \
    cd libsodium && ./configure && make -j && make -j check && make -j install && \
    cp -r /libsodium/src/libsodium/include /usr/local/lib/libsodium && \
    echo "/usr/local/lib" > /etc/ld.so.conf.d/libsodium.conf && \
    ldconfig

# Install spdlog
RUN cd / && wget https://github.com/gabime/spdlog/archive/refs/tags/v1.9.2.tar.gz && \
    tar -zxvf v1.9.2.tar.gz && rm v1.9.2.tar.gz && cd spdlog-1.9.2 && cp -r include/spdlog /usr/local/lib/spdlog

# grpcサーバのhealthcheckをするためのツールをインストール
RUN GRPC_HEALTH_PROBE_VERSION=v0.4.11 && \
    case "$TARGETPLATFORM" in \
    linux/amd64) ARCH=linux-amd64 ;; \
    linux/arm64) ARCH=linux-arm64 ;; \
    esac; \
    wget -qO/bin/grpc_health_probe https://github.com/grpc-ecosystem/grpc-health-probe/releases/download/${GRPC_HEALTH_PROBE_VERSION}/grpc_health_probe-${ARCH} && \
    chmod +x /bin/grpc_health_probe

FROM base as src
# 開発環境のフォーマッタを統一
# c_cpp_properties.jsonは残したいのでマウントはせず
COPY ./.vscode/settings.json /QuickMPC/.vscode/

FROM base as builder
COPY src/ComputationContainer/ /QuickMPC
# github actionsで使用されているdockerがCOPYを2連続で行うとたまにエラーが出る問題を抱えている
# その問題を回避するために、COPYの間にRun trueを挟んでいる
RUN true
COPY src/Proto/ /Proto
RUN true
COPY src/ComputationContainer/.cache /root/.cache
RUN true
COPY src/ComputationContainer/sa.json /root/sa.json
WORKDIR /QuickMPC
ARG BUILD_OPT
ENV BUILD_OPT=${BUILD_OPT}
RUN echo ${BUILD_OPT}
# 実行ファイル生成
RUN bazel build //:all ${BUILD_OPT}
RUN mkdir -p /QuickMPC/__shared_libs && \
    ldd /QuickMPC/bazel-bin/computation_container | perl -pe 's/\s+/\n/g' | grep '^/' && \
    xargs -I{} cp {} /QuickMPC/__shared_libs

FROM builder as dev

# image for deploy
FROM alpine:3.15.1 as dep-runner
RUN apk update && \
    apk add curl && \
    apk add --no-cache bash && \
    # CVE-2022-0778 対策
    # 公式イメージの upgrade がされていないため
    apk upgrade
RUN true
COPY --from=builder /QuickMPC/bazel-bin/computation_container /QuickMPC/bazel-bin/computation_container
RUN true
COPY --from=builder /QuickMPC/__shared_libs/. /QuickMPC/bazel-bin/
RUN true
COPY --from=ghcr.io/grpc-ecosystem/grpc-health-probe:v0.4.11 /ko-app/grpc-health-probe /bin/grpc_health_probe
WORKDIR /QuickMPC

FROM builder as small
ENV PARTY_ID=1
COPY Config/ComputationContainer/Compute${PARTY_ID}/.src.env /QuickMPC/Config/.env

FROM builder as medium

# image for deploy
FROM alpine:latest as large-runner
RUN apk update && apk add curl
RUN true
COPY --from=builder /QuickMPC/bazel-bin/computation_container /QuickMPC/bazel-bin/computation_container
# テスト用バイナリ
COPY --from=builder /QuickMPC/bazel-bin/computation_container_test /QuickMPC/bazel-bin/computation_container_test
RUN true
COPY --from=builder /QuickMPC/__shared_libs/. /QuickMPC/bazel-bin/
RUN true
WORKDIR /QuickMPC
