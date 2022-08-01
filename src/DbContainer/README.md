Db Container
====

## ディレクトリ構造
```
-src/DbContainer
 ├── Db
 │   ├── Dockerfile # shareDbのDockerfile

     ︙

 ├── docker-compose.yml # dbg, sharedb, secrets-serverのコンテナ環境記述ファイル
 ├── Makefile
 └── secrets-server.Dockerfile # 秘密情報を生成して他コンテナへと安全に渡すためのサーバ
 ```

### secrets-serverについて
秘密情報を他コンテナに安全に渡すためのコンテナ
Imageの中に秘密情報が存在するため、ghcrなどにpushしてはいけない
もし秘密情報を作り直したい場合は、DockerImageの削除とCACHEを使用せずにbuildし直す必要がある

具体的には以下のコマンドを実行することで秘密情報が抹消され、
同時に永続化されたDBも破壊される
```
make rm-db
```
imageの削除, containerの削除だけではcacheが残っていて抹消できないので注意

この後再構築したい場合は、以下でよい
```
make up
```

### secrets-serverの動作
1. `make up` した場合、secret-serverコンテナが起動
2. ユーザ名とパスワードを生成
3. curlでアクセスしてきた同一ネットワーク内のコンテナに上記を返却
4. 受け取ったコンテナはexportして一時的に環境変数に設定しコマンドを走らせる(stopすると環境変数からも消える)
5. secret-serverはここでコンテナを削除する(またupすれば呼び出せる)


## 使用方法

### イメージを構築
事前にイメージを構築したい場合は以下のコマンドを実行
```bash
make build # docker-compose build
```

### イメージからコンテナの起動
構築済みのイメージからコンテナを起動する場合は以下のコマンドを実行
```bash
make up # docker-compose up --build
```
バックグラウンドで実行したい場合は以下:
```bash
make upd # docker-compose up -d --build
```
もし存在していない場合は、自動的に構築

### 起動中のコンテナに入る
実行中のコンテナのbashに入る時、以下のコマンドを実行
```bash
make login-dbg # docker-compose exec dbg /bin/bash
make login-sharedb # docker-compose exec sharedb /bin/bash
```

### 削除
containers, networks, images, and volumesを停止かつ削除するとき以下のコマンドを実行
```bash
make rm-all # docker-compose down --rmi all --volumes
```
コンテナのみを削除する場合は以下を実行
```bash
make rm # docker-compose rm -fs
```

コンテナとネットワークを削除する場合は以下を実行
```bash
make down # docker-compose down
```

データベースのデータを削除する場合は以下を実行
```bash
make rm-db
```
