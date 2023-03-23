FROM python:3.7 as src

RUN apt-get update && \
    apt-get install -y \
    wget \
    make

RUN pip install -U pip && \
    pip install \
    autopep8 \
    mypy-protobuf \
    grpcio-tools==1.46.3

ENV PATH $PATH:/root/go/bin
ENV PATH $PATH:/usr/local/go/bin

# Install go
RUN wget https://go.dev/dl/go1.18.6.linux-amd64.tar.gz && \
    tar -C /usr/local -xzf go1.18.6.linux-amd64.tar.gz

# Install Protocolbuf
RUN apt-get install -y unzip curl && \
    PROTOC_VER=3.19.1 && \
    curl -LO https://github.com/protocolbuffers/protobuf/releases/download/v${PROTOC_VER}/protoc-${PROTOC_VER}-linux-x86_64.zip && \
    unzip protoc-${PROTOC_VER}-linux-x86_64.zip -d /root/.local && \
    go install google.golang.org/protobuf/cmd/protoc-gen-go@v1.28.0 && \
    go install google.golang.org/grpc/cmd/protoc-gen-go-grpc@v1.2.0

ENV PATH $PATH:/usr/local
ENV PATH=$PATH:/root/.local/bin

WORKDIR /QuickMPC/proto
