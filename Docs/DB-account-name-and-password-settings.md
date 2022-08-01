# DBのPasswordとアカウント名の設定について

## 背景

Db(CouchbaseServer)のPasswordとUserNameがデフォルト値のままなので何かしらの変更が必要

前提として、

### サービス提供中はDbのPasswordとUserNameを知る必要があるのは誰なのか？

1. ~~ユーザが知っていればいい~~
    1. ユーザ(or サーバ管理者)定義のものを使う
2. ~~弊社のみが知っていればいい~~
    1. 弊社が勝手に発行したものをエンジンの中に見えないように埋め込む
3. ~~弊社とユーザの両者が知っていればいい~~
    1. 弊社が勝手に発行したものを伝えて設定してもらう
4. エンジン以外誰にも知られたくない
    1. エンジンがよしなに自動で作ってシステム以外誰からも見えないようにしておく

⇨ 4のエンジン以外誰にも知られたくない

# 提案

以下の要領でエンジンがよしなに自動で作ってシステム以外誰からも見えないようにしておく

`Config/DbContainer/Db/*/.env`から以下記述を削除

```bash
CB_USERNAME=Administrator
CB_PASSWORD=password
```

代わりに以下の機構によって秘密情報を設定する

```yaml
-src/DbContainer
 ├── Db
 │   ├── Dockerfile # shareDb,tripleDbの共通Dockerfile

     ︙

 ├── docker-compose.yml # dbg, sharedb, tripledb, secrets-serverのコンテナ環境記述ファイル
 ├── Makefile
 └── secrets-server.Dockerfile # 秘密情報を生成して他コンテナへと安全に渡すためのサーバ
```

ユーザ名, パスワードは `secrets-server.Dockerfile` のビルド時に生成しておき, 秘密情報返却サーバ `http://secrets-server/.*`を立てておく.

他コンテナの起動時に`secrets-server` に対して秘密情報を要求し, 環境変数にexportする.

この後, `secrets-server` のイメージ(CACHEを含めて)を破壊すると, 秘密情報はどこにも永続化されずに破壊される.

```bash
FROM couchbase:community-6.5.1

...

# docker-composeから環境変数受け取り
ARG DB_NAME
# ENTRYPOINTで使えるようにENVで登録
ENV _DB_NAME=${DB_NAME}

# 動的に生成したユーザ名とパスワードをsecrets-serverからネットワーク経由で受け取り、環境変数に設定して初期化&サーバ立ち上げ
# ドキュメント: Docs/DB-account-name-and-password-settings.md
ENTRYPOINT ["/bin/sh", "-c", "export $(curl -sS http://secrets-server:8000/.$_DB_NAME.env | xargs) && ./configure-node.sh"]
```

~~(DockerImageのビルド時にログでバレる...?要検証)~~

```yaml
version: "3.3"
services:
  dbg:
    container_name: dbgate_container
    build:
      context: ../../
      dockerfile: src/DbContainer/DbGate/Dockerfile
    ...
    networks:
      default: {}
      ...
    depends_on:
      sharedb:
        condition: service_started
      tripledb:
        condition: service_started

  sharedb:
    container_name: sharedb_container
    build:
      context: ../../
      dockerfile: src/DbContainer/Db/Dockerfile
      args:
        - DB_NAME=sharedb
    # -snip-
    networks:
      default: {}
    # -snip-
    depends_on:
      secrets-server:
        condition: service_started

  tripledb:
    container_name: tripledb_container
    build:
      context: ../../
      dockerfile: src/DbContainer/Db/Dockerfile
      args:
        - DB_NAME=tripledb
        ...
    networks:
      default: {}
      ...
    depends_on:
      secrets-server:
        condition: service_started

  secrets-server:
    container_name: secrets-server
    tty: true
    build:
      context: ../../
      dockerfile: src/DbContainer/secrets-server.Dockerfile
      args:
        - DB_NAMES=sharedb,tripledb
    networks:
      - default

...
```

```makefile
...

up:
	docker-compose up -d --build
	sleep 15
	# secrets-serverのcontainerを閉じる
	docker-compose stop secrets-server
	docker-compose rm -f secrets-server

...

# Dbのデータ・秘密情報を完全に削除
rm-db:
	- rm -r ./Db/shareDb/data/lib
	- rm -r ./Db/tripleDb/data/lib
	- yes | docker system prune -a --filter label=secrets-server
	- docker-compose rm -fvs secrets-server

...
```

```docker
FROM ubuntu:18.04

RUN apt-get update && \
    apt-get install -y \
    build-essential \
    libssl-dev \
    curl \
    python3-pip

# docker-composeから環境変数受け取り
# 例) DB_NAMES=sharedb,tripledb
ARG DB_NAMES
# ENTRYPOINTで使えるようにENVで登録
ENV _DB_NAMES=${DB_NAMES}

RUN mkdir /files
# DB_NAMESを`,`区切りでパースしてすべてのdbに対してユーザ名とパスワードを動的に生成
# CB_USERNAMEは8[bytes]*2[桁/bytes]=16[桁]
# CB_PASSWORDはLENGTH=24[桁]にするために、--base64を18[bytes]*4/3[桁/bytes]=24[桁]生成しておき、
# Couchbaseのパスワードに使えないらしき`=+/`使えないのでtrコマンドで置換し、念の為cutコマンドで1-24文字分切り取る
RUN /bin/bash -c 'list=(${_DB_NAMES//,/ }) && for DB_NAME in "${list[@]}"; do echo -e "CB_USERNAME=$(openssl rand -hex 8)\nCB_PASSWORD=$(openssl rand -base64 18 | tr "=+/" 012 | cut -c1-24)" > /files/.${DB_NAME}.env; done'

# NOTE: 秘密情報を消し去りたいとき、Imageの削除だけでなくビルドキャッシュも削除する必要がある
# その際に`docker system prune --filter label=secrets-server`でこのImageのキャッシュだけ削除できるようにするためのもの
LABEL secrets-server=

# 参考文献: https://terashim.com/posts/docker-build-secret/#network
# 動的に生成した秘密情報のenvファイルのみが配置された/filesディレクトリを公開する
# 同ネットワーク内の別コンテナから以下のようにネット経由で安全に秘密情報を渡す事が可能
# wget -O - -q http://localhost:8000/.sharedb.env
# NOTE: pushdはshで使え無さそうだったのでbashで実行
ENTRYPOINT ["/bin/bash", "-c", "ls files; pushd files/; python3 -m http.server; popd"]
```

# 理由

[Dockerイメージビルド時の秘密情報の扱い方に関するまとめ](https://terashim.com/posts/docker-build-secret/)

- DbGate, *Dbはデプロイ時にはイメージをghcrなどで共有するため, 秘密情報を永続化したり, RUN,COPY, ENV, ARGなどで渡せない(historyに残ってしまう)
    - そのため、ホストマシンで秘密情報を生成し、他コンテナに内部ネットワーク経由で渡すコンテナを導入することにした

# コメント

- [解決済み] ~~初期化できない問題~~

    ```bash
    CB_PASSWORD=V7Wc7mbGqTGvKQ+pTssAaWgP
    # のとき、
    sharedb_container | Waiting for bucket initialization...
    sharedb_container | Waiting for bucket initialization...
    sharedb_container | Waiting for bucket initialization...
    sharedb_container | Waiting for bucket initialization...
    sharedb_container | Waiting for bucket initialization...
    # が永遠に続くが、

    CB_PASSWORD=HAFQZxLlmA2yDHe27Fb7onp8
    # のとき、ちゃんと初期化が完了する
    # ためしに+を無くしてみたら通ったことから、
    # これはおそらくCouchbaseのパスワードに`+`が使えない事による問題っぽい

    # ドキュメントには+がつかえないという記述は
    ```

- CouchbaseのUserNameとPasswordの仕様は以下を参考

    記号は扱いが面倒なので—base64から記号を特定の数字に置換して扱うことにした

    [Usernames and Passwords](https://docs.couchbase.com/server/current/learn/security/usernames-and-passwords.html)
