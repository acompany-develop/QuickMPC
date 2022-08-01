# See here for image contents: https://github.com/microsoft/vscode-dev-containers/tree/v0.134.1/containers/cpp/.devcontainer/base.Dockerfile
ARG VARIANT="buster"
FROM mcr.microsoft.com/vscode/devcontainers/cpp:0-${VARIANT}

RUN apt install -y lsb-release wget software-properties-common

RUN wget https://apt.llvm.org/llvm.sh && chmod +x llvm.sh && sudo ./llvm.sh 14

RUN apt-get update && export DEBIAN_FRONTEND=noninteractive \
    && apt-get -y install --no-install-recommends clang-format-14
