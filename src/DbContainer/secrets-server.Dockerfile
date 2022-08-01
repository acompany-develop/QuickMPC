FROM ubuntu:18.04 as base

RUN apt-get update && \
    apt-get install -y \
    build-essential \
    libssl-dev \
    curl \
    python3-pip

# docker-composeから環境変数受け取り
# 例) lowercase_DB_NAMES=sharedb
ARG lowercase_DB_NAMES
# ENTRYPOINTで使えるようにENVで登録
ENV lowercase_DB_NAMES=${lowercase_DB_NAMES}

RUN mkdir /files

# NOTE: 秘密情報を消し去りたいとき、Imageの削除だけでなくビルドキャッシュも削除する必要がある
# その際に`docker system prune --filter label=secrets-server`でこのImageのキャッシュだけ削除できるようにするためのもの
LABEL secrets-server=

FROM alpine:3.15.1 as dep-runner
ARG lowercase_DB_NAMES
ENV lowercase_DB_NAMES=${lowercase_DB_NAMES}
RUN mkdir /files
RUN apk update && apk add --no-cache python3 bash openssl
RUN /bin/bash -c 'list=(${lowercase_DB_NAMES//,/ }) && for lowercase_DB_NAME in "${list[@]}"; do echo -e "CB_USERNAME=$(openssl rand -hex 8)\nCB_PASSWORD=$(openssl rand -base64 18 | tr "=+/" 012 | cut -c1-24)" > /files/.${lowercase_DB_NAME}.env; done'
ENTRYPOINT ["/bin/bash", "-c", "ls files; pushd files/; python3 -m http.server; popd"]


FROM base as src
# srcでは固定化
ENV CB_USERNAME=Administrator
ENV CB_PASSWORD=password
RUN /bin/bash -c 'list=(${lowercase_DB_NAMES//,/ }) && for lowercase_DB_NAME in "${list[@]}"; do echo -e "CB_USERNAME=${CB_USERNAME}\nCB_PASSWORD=${CB_PASSWORD}" > /files/.${lowercase_DB_NAME}.env; done'
ENTRYPOINT ["/bin/bash", "-c", "ls files; pushd files/; python3 -m http.server; popd"]

FROM base as dev
# DB_NAMESを`,`区切りでパースしてすべてのdbに対してユーザ名とパスワードを動的に生成
# CB_USERNAMEは8[bytes]*2[桁/bytes]=16[桁]
# CB_PASSWORDはLENGTH=24[桁]にするために、--base64を18[bytes]*4/3[桁/bytes]=24[桁]生成しておき、
# Couchbaseのパスワードに使えないらしき`=+/`使えないのでtrコマンドで置換し、念の為cutコマンドで1-24文字分切り取る
RUN /bin/bash -c 'list=(${lowercase_DB_NAMES//,/ }) && for lowercase_DB_NAME in "${list[@]}"; do echo -e "CB_USERNAME=$(openssl rand -hex 8)\nCB_PASSWORD=$(openssl rand -base64 18 | tr "=+/" 012 | cut -c1-24)" > /files/.${lowercase_DB_NAME}.env; done'
# 参考文献: https://terashim.com/posts/docker-build-secret/#network
# 動的に生成した秘密情報のenvファイルのみが配置された/filesディレクトリを公開する
# 同ネットワーク内の別コンテナから以下のようにネット経由で安全に秘密情報を渡す事が可能
# wget -O - -q http://localhost:8000/.sharedb.env
# NOTE: pushdはshで使え無さそうだったのでbashで実行
ENTRYPOINT ["/bin/bash", "-c", "ls files; pushd files/; python3 -m http.server; popd"]

FROM base as small
RUN /bin/bash -c 'list=(${lowercase_DB_NAMES//,/ }) && for lowercase_DB_NAME in "${list[@]}"; do echo -e "CB_USERNAME=$(openssl rand -hex 8)\nCB_PASSWORD=$(openssl rand -base64 18 | tr "=+/" 012 | cut -c1-24)" > /files/.${lowercase_DB_NAME}.env; done'
ENTRYPOINT ["/bin/bash", "-c", "ls files; pushd files/; python3 -m http.server; popd"]

FROM base as large
RUN /bin/bash -c 'list=(${lowercase_DB_NAMES//,/ }) && for lowercase_DB_NAME in "${list[@]}"; do echo -e "CB_USERNAME=$(openssl rand -hex 8)\nCB_PASSWORD=$(openssl rand -base64 18 | tr "=+/" 012 | cut -c1-24)" > /files/.${lowercase_DB_NAME}.env; done'
ENTRYPOINT ["/bin/bash", "-c", "ls files; pushd files/; python3 -m http.server; popd"]

FROM base as dep
RUN /bin/bash -c 'list=(${lowercase_DB_NAMES//,/ }) && for lowercase_DB_NAME in "${list[@]}"; do echo -e "CB_USERNAME=$(openssl rand -hex 8)\nCB_PASSWORD=$(openssl rand -base64 18 | tr "=+/" 012 | cut -c1-24)" > /files/.${lowercase_DB_NAME}.env; done'
ENTRYPOINT ["/bin/bash", "-c", "ls files; pushd files/; python3 -m http.server; popd"]