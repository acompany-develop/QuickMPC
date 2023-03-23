# BTS
QuickMPCで使われるTripleを生成するサービス

## ローカルでの起動方法
`packages/server/beaver_triple_service/`で以下のコマンドを実行
```sh
make run
```
これにより, 同一ホストネットワーク内であれば `127.0.0.1:64101`で接続可能
以下の様に`grpcurl`でCLIからリクエストを送ることも可能
```sh
$ grpcurl -plaintext -d '{"job_id": 1, "amount": 5}' 127.0.0.1:64101 enginetobts.engine_to_bts/GetTriples
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

## テスト方法
`scripts/`で以下のコマンドを実行
```sh
make test
```
特定のtestを指定して実行したい場合は以下のようにする
```sh
make test t=./beaver_triple_service/triple_generator
# scripts/beaver_triple_service/triple_generator/ 直下のみのテストを実行したい場合
make test p=unit # `unit*test.sh`を実行したい場合
make test m=build # `*test.sh`のbuild処理のみ実行したい場合
make test m=run # `*test.sh`のrun処理のみ実行したい場合
```

## 開発方法
`packages/server/beaver_triple_service/`で以下のコマンドを実行
```sh
make up-build
make upd-build # バックグラウンドで起動したい場合はこちら
```

その後, VSCodeの左タブから`Remote Explorer` > 上のトグルから`Containers`を選択 > `beavertripleservice`にカーソルを合わせる > 新規フォルダアイコンを選択 > 開く場所を選択してsrc_btsコンテナの中で開発が行えるようになる.

![image](https://user-images.githubusercontent.com/33140349/142567126-52b8e392-a81c-4630-bf6c-6f801653770a.png)

## Container Image

GitHub Packages のコンテナレジストリにイメージを用意している

| tag             | description                                                    |
|-----------------|----------------------------------------------------------------|
| ${date}        | 日時: `${date}` に作成されたイメージ                   |
| ${version}     | GitHub tagをトリガーにreleaseされたイメージ |

Dockerfile で使用される各 build stage については以下のリンクを参照

[QuickMPC/scripts/README-ja.md#how-to-develop-docker-composeyml](../scripts/README-ja.md#how-to-develop-docker-composeyml)

## grpcurlでのdebug
※ portはよしなに変更してください
```bash
grpcurl -d '{"job_id": 1, "amount": 10}' beaver_triple_service:54100 enginetobts.engine_to_bts/GetTriples
```

## grpcサーバのヘルスチェック
```bash
grpc_health_probe -addr=localhost:54100
```

## JWT token の生成
YAML ファイルを入力に JWT token を生成します

### 手順
btsのイメージを取得します

  ※ tagはこちらの[ページ](https://github.com/acompany-develop/QuickMPC/pkgs/container/quickmpc-bts)は参照のうえ、latestのtagに変更してください
  ```
  docker pull ghcr.io/acompany-develop/quickmpc-bts:<tag>
  ```

JWT tokenの生成

  ```console
  docker run --rm \
    -v ./path/to/directory_with_config.yml:<mount_path> \
    ghcr.io/acompany-develop/quickmpc-bts:<tag> ./beaver_triple_service generateJwt \
    --file /path/to/config.yml \
    --output <mount_path>                                           # use own configuration
  ```

--output で指定したPATH直下にクライアントとサーバ向けにそれぞれ `server.<ymlのファイル名>.env`, `client.<ymlのファイル名>.env` というファイル形式の設定ファイルが書き込まれます

```
// server.<ymlのファイル名>.env
JWT_SECRET_KEY=...
```

```
// client.// client.<ymlのファイル名>.env
.env
BTS_TOKEN=...
```

これを`config/computation_container/compute{1,2,3}/`直下に `.env` という名前で配置し、
docker-compose.ymlのccコンテナのenv_fileで上記のパスを指定することでJWTが利用可能になります。
