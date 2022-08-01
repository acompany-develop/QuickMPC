All Container
====
全サービスのソースコードを置くディレクトリ

## Dockerfileの書き方
DockerのImageには以下のステージ名を使用している.
| ステージ名 | 用途                                                                   |
| :--------- | :--------------------------------------------------------------------- |
| src        | src/で開発する時に使用する                                             |
| dev        | 3台同時に建てたい時に使用するステージ                                  |
| small      | CIのsmallテストで使用するステージ                                      |
| medium     | CIのmediumテストで使用するステージ                                     |
| large      | CIのlargeテストで使用するステージ                                      |
| dep        | デプロイに使用するステージ                                             |
| builder    | すべてのソースコードをImageの中でbuildしておく基底ステージ             |
| *-runner   | 実行用途の軽量な`alpine`イメージであることを明示したい時につけるsuffix |

それぞれのステージは[マルチステージビルド](https://matsuand.github.io/docs.docker.jp.onthefly/develop/develop-images/multistage-build/)によって構造的に記述されており,
例えば以下のような形式で記述する.
```Dockerfile
FROM ubuntu:18.04 as base

# 諸々のセットアップ処理
...

# src/で開発する時に使用するステージ
FROM base as src
...

# すべてのソースコードをImageの中でbuildしておく基底ステージ
FROM base as builder
COPY hoge huga
...

# 3台同時に建てたい時に使用するステージ
FROM builder as dev # builderを継承している
...

# デプロイに使用する実行専用ステージ
FROM alpine:latest as dep-runner
COPY --from=builder hoge huga
...

# CIのsmallテストで使用するステージ
FROM builder as small # builderを継承している
...

# CIのmediumテストで使用する実行専用ステージ
FROM builder as medium # builderを継承している
...

# CIのlargeテストで使用する実行専用ステージ
FROM alpine:latest as large-runner
COPY --from=builder hoge huga
...

```

なお, docker-compose.ymlでどのステージのImageを使うかは以下のように指定する.
```yaml
...

services:
  hoge:
    ...
    build:
      context: ../../
      dockerfile: src/HogeContainer/Dockerfile
      target: src # dev, dep, small, medium, large等
    ...
```

## 使用方法

### イメージを構築
事前にイメージを構築したい場合は以下のコマンドを実行
```bash
make build # docker-compose build
```

### イメージからコンテナの起動
イメージからコンテナを起動する場合は以下のコマンドを実行
```bash
make up # docker-compose up --build
```
バックグラウンドで実行したい場合は以下を実行
```bash
make upd # docker-compose up -d
```
```bash
make upd-build # docker-compose up -d --build
```
もし存在していない場合は、自動的に構築

### 起動中のコンテナに入る
実行中のコンテナのbashに入る時、以下のコマンドを実行
```bash
make login-cc # docker-compose exec cc /bin/bash
make login-dbg
make login-sharedb
make login-mc
```

### 削除
コンテナ、ネットワーク、イメージ、ボリュームを停止して削除したい場合は以下を実行
```bash
make rm-all # docker-compose down --rmi all --volumes
```

コンテナのみを停止して削除したい場合は以下を実行
```bash
make rm # docker-compose rm -fs
```

コンテナとネットワークを停止して削除したい場合は以下を実行
```bash
make down # docker-compose down
```

データベースのデータを削除する場合は以下を実行
```bash
make rm-db
```


### IntelliSenseの有効化
VsCodeリモートコンテナを使用する場合は、C++のintelliSenseを有効にするために以下のコマンドを実行する必要がある

```console
root@container_id:/QuickMPC# bazel //:compdb
```

詳細は以下のMarkdownを参照:
- [Visual Studio Codeでコンテナの中に入って開発する方法](/Docs/how-to-develop-with-vscode.md#例-intellisense)
### formatter機能の有効化(VSCodeを使っている人)
この機能を使うと自動的に整形してくれる

詳細は以下のMarkdownを参照:
- [Visual Studio Codeでコンテナの中に入って開発する方法](/Docs/how-to-develop-with-vscode.md)
