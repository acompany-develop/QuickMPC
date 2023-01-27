BeverTripleService Container
====

## 前準備
<span style="color: red; ">※ 必ず全員やること</span>

[ghcr.ioにイメージをpushまたはpullする時にやること](/Docs/ghcr-io-push-pull.md)を行ってghcrからBTSのイメージをpullする権限を得る

## ローカルでの起動方法
`src/BeaverTripleService/`で以下のコマンドを実行
```sh
make run
```
これにより, 同一ホストネットワーク内であれば `127.0.0.1:64101`で接続可能
以下の様に`grpcurl`でCLIからリクエストを送ることも可能
```sh
$ grpcurl -plaintext -d '{"job_id": 1, "amount": 5}' 127.0.0.1:64101 engineToBts.EngineToBts/GetTriples
{
  "triples": [
    {
      "a": "8218791",
      "b": "9915790",
      "c": "9079842"
    },
    {
      "a": "4645217",
      "b": "-5036032",
      "c": "8470037"
    },
    {
      "a": "-426163",
      "b": "495462",
      "c": "9562034"
    },
    {
      "a": "-2297368",
      "b": "-2005170",
      "c": "-150099"
    },
    {
      "a": "5474059",
      "b": "-3305195",
      "c": "7914345"
    }
  ]
}
```

## 使用方法
バックグラウンドでコンテナを起動したい場合は今のコマンドを実行
```bash
make upd # docker-compose up -d --build
```

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
