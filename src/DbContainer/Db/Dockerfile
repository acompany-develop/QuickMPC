FROM couchbase:community-7.0.2 as base

# Configure apt-get for NodeJS
# Install NPM and NodeJS and jq, with apt-get cleanup
RUN apt update && \
    apt install ca-certificates && \
    curl -sL https://deb.nodesource.com/setup_12.x | bash - && \
    apt-get install -yq build-essential nodejs jq && \
    apt-get autoremove && apt-get clean && \
    rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*

# Upgrade to jq 1.6
RUN wget https://github.com/stedolan/jq/releases/download/jq-1.6/jq-linux64 && \
    chmod +x jq-linux64 && \
    mv jq-linux64 $(which jq)

# Copy package.json
WORKDIR /scripts
COPY ./src/DbContainer/Db/scripts/package*.json ./

# Install fakeit, couchbase-index-manager, and couchbase
RUN npm ci && \
    rm -rf /tmp/* /var/tmp/*

# Copy scripts
COPY ./src/DbContainer/Db/scripts/ /scripts/

RUN mkdir /nodestatus

# docker-composeから環境変数受け取り
ARG lowercase_DB_NAME
# ENTRYPOINTで使えるようにENVで登録
ENV lowercase_DB_NAME=${lowercase_DB_NAME}
COPY ./Config/DbContainer/Db/${lowercase_DB_NAME}/startup /startup

# 後続のサービスが素早く立ち上がって欲しいため
# start-period をデフォルトの 0s とし、retries を大きめに設定
# --start-period=0s はオプションとして指定できない
HEALTHCHECK --interval=3s --timeout=5s --retries=20 \
    CMD /bin/bash -l -c /scripts/health-check.sh

FROM base as src
# 動的に生成したユーザ名とパスワードをsecrets-serverからネットワーク経由で受け取り、環境変数に設定して初期化&サーバ立ち上げ
# ドキュメント: Docs/DB-account-name-and-password-settings.md
COPY ./src/DbContainer/.bashrc /root/
# -lオプションをつけるとログイン時に~/.bashrcが読み込まれる
ENTRYPOINT ["/bin/bash", "-l", "-c", "printenv && ./configure-node.sh"]

FROM base as dev
COPY ./src/DbContainer/.bashrc /root/
ENTRYPOINT ["/bin/bash", "-l", "-c", "printenv && ./configure-node.sh"]

FROM base as dep
COPY ./src/DbContainer/.bashrc /root/
ENTRYPOINT ["/bin/bash", "-l", "-c", "./configure-node.sh"]

FROM base as small
COPY ./src/DbContainer/.bashrc /root/
ENTRYPOINT ["/bin/bash", "-l", "-c", "printenv && ./configure-node.sh"]

FROM base as large
COPY ./src/DbContainer/.bashrc /root/
ENTRYPOINT ["/bin/bash", "-l", "-c", "./configure-node.sh"]
