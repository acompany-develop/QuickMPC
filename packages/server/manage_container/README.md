Manage Container
=====

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
make login # docker-compose exec cc /bin/bash
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

## テスト方法
### unit_test
各ソースファイルと同じディレクトリ内に配置されている．
```console
$ go test $(go list -f {{.Dir}} ./... | grep -v integration_test)
```

### integration_test
`IntegraionTest/`以下に配置されている．
MC，DBを3-Party立てている必要がある．
```console
$ go test ./integration_test/...
```
