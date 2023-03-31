FROM ubuntu:18.04 as base


FROM base as builder

# install docker
RUN apt update && \
    apt install -y \
      apt-transport-https \
      ca-certificates \
      curl \
      software-properties-common && \
    curl -fsSL https://download.docker.com/linux/ubuntu/gpg | apt-key add - && \
    add-apt-repository \
      "deb [arch=amd64] https://download.docker.com/linux/ubuntu focal stable" && \
    apt update && \
    apt install -y docker-ce

# install docker-compose
RUN curl -L "https://github.com/docker/compose/releases/download/1.29.2/docker-compose-$(uname -s)-$(uname -m)" -o /usr/local/bin/docker-compose && \
    chmod +x /usr/local/bin/docker-compose

# install python, pytest
RUN apt update && \
    apt install -y python3.7 python3-pip && \
    python3.7 -m pip install -U pip && \
    python3.7 -m pip install pytest

FROM builder as medium

FROM builder as large
