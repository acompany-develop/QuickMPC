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
    libboost-all-dev \
    vim \
    cron \
    curl \
    iputils-ping \
    net-tools \
    dnsutils \
    gdb

# Install cmake
RUN wget -q -O cmake-linux.sh https://github.com/Kitware/CMake/releases/download/v3.17.0/cmake-3.17.0-Linux-x86_64.sh && \
    mkdir -p $CMAKE_INSTALL_DIR && \
    sh cmake-linux.sh -- --skip-license --prefix=$CMAKE_INSTALL_DIR && \
    rm cmake-linux.sh && \
    # Install bazel
    echo "deb [arch=amd64] https://storage.googleapis.com/bazel-apt stable jdk1.8" | tee /etc/apt/sources.list.d/bazel.list && \
    wget https://bazel.build/bazel-release.pub.gpg && \
    apt-key add bazel-release.pub.gpg && \
    apt-get update && \
    apt-get -y install bazel-5.0.0 && \
    ln -s /usr/bin/bazel-5.0.0 /usr/bin/bazel

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
RUN GRPC_HEALTH_PROBE_VERSION=v0.4.16 && \
    wget -qO/bin/grpc_health_probe https://github.com/grpc-ecosystem/grpc-health-probe/releases/download/${GRPC_HEALTH_PROBE_VERSION}/grpc_health_probe-linux-amd64 && \
    chmod +x /bin/grpc_health_probe

# Create directory to save core dump file
RUN mkdir -p /tmp/cores

FROM base as src
# 開発環境のフォーマッタを統一
# c_cpp_properties.jsonは残したいのでマウントはせず
COPY ./.vscode/settings.json /QuickMPC/.vscode/

FROM base as builder_base
COPY packages/server/computation_container/ /QuickMPC
# github actionsで使用されているdockerがCOPYを2連続で行うとたまにエラーが出る問題を抱えている
# その問題を回避するために、COPYの間にRun trueを挟んでいる
RUN true
COPY proto/ /proto
RUN true
COPY packages/server/computation_container/sa.json /root/sa.json
WORKDIR /QuickMPC

FROM builder_base as builder
# 実行ファイル生成
RUN --mount=type=cache,id=default:/root/.cache,target=/root/.cache \
    bazel build //:all
RUN --mount=type=cache,id=default:/root/.cache,target=/root/.cache \
    bash -c 'cp /QuickMPC/bazel-bin/computation_container{,_test,_benchmark} /QuickMPC/'

FROM builder_base as builder_debug
# 実行ファイル生成
RUN --mount=type=cache,id=debug:/root/.cache,target=/root/.cache \
    bazel build //:all --config=debug
RUN --mount=type=cache,id=debug:/root/.cache,target=/root/.cache \
    bash -c 'cp /QuickMPC/bazel-bin/computation_container{,_test,_benchmark} /QuickMPC/'

FROM builder as dev

# image for deploy
FROM alpine:3.16.2 as dep-runner
RUN apk update && \
    apk add curl && \
    apk add --no-cache bash && \
    apk add gdb && \
    # CVE-2022-0778 対策
    # 公式イメージの upgrade がされていないため
    apk upgrade
RUN true
COPY --from=builder /QuickMPC/computation_container /QuickMPC/bazel-bin/computation_container
RUN true
COPY --from=builder /QuickMPC/scripts /QuickMPC/scripts
RUN true
COPY --from=builder /usr/local/lib /usr/local/lib
RUN true
COPY --from=builder /usr/lib/x86_64-linux-gnu /usr/lib/x86_64-linux-gnu
RUN true
COPY --from=builder /lib/x86_64-linux-gnu /lib/x86_64-linux-gnu
RUN true
COPY --from=builder /lib64 /lib64
RUN true
COPY --from=ghcr.io/grpc-ecosystem/grpc-health-probe:v0.4.18 /ko-app/grpc-health-probe /bin/grpc_health_probe
# Create directory to save core dump file
RUN mkdir -p /tmp/cores
WORKDIR /QuickMPC

FROM builder_debug as small
ENV PARTY_ID=1
COPY config/computation_container/compute${PARTY_ID}/.src.env /QuickMPC/config/.env

FROM builder_base as small_tmp
ENV PARTY_ID=1
COPY config/computation_container/compute${PARTY_ID}/.src.env /QuickMPC/config/.env

FROM builder_debug as medium

# image for deploy
FROM alpine:latest as large-runner
RUN apk update && apk add curl
RUN true
COPY --from=builder /QuickMPC/bazel-bin/computation_container /QuickMPC/bazel-bin/computation_container
RUN true
COPY --from=builder /QuickMPC/scripts /QuickMPC/scripts
RUN true
# テスト用バイナリ
COPY --from=builder /QuickMPC/bazel-bin/computation_container_test /QuickMPC/bazel-bin/computation_container_test
RUN true
COPY --from=builder /usr/local/lib /usr/local/lib
RUN true
COPY --from=builder /usr/lib/x86_64-linux-gnu /usr/lib/x86_64-linux-gnu
RUN true
COPY --from=builder /lib/x86_64-linux-gnu /lib/x86_64-linux-gnu
RUN true
COPY --from=builder /lib64 /lib64
# Create directory to save core dump file
RUN mkdir -p /tmp/cores
WORKDIR /QuickMPC
