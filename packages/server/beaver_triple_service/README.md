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
YAML ファイルと SECRET_KEY  を入力に JWT token を生成する．

### 手順
#### btsのイメージ取得

※ tagはこちらの[ページ](https://github.com/acompany-develop/QuickMPC/pkgs/container/quickmpc-bts)は参照のうえ，latestのtagに変更する．
```console
docker pull ghcr.io/acompany-develop/quickmpc-bts:<tag>
```

#### 設定を作成
[sample設定](https://github.com/acompany-develop/QuickMPC/tree/main/packages/server/beaver_triple_service/cmd/jwt_generator/sample)を参考にして，適当な名前 `<name>` を定めて次の2ファイルを生成する．
- `<name>.yaml`
- `server.<name>.env`

`server.<name>.env`に記載するkeyはBASE64で生成する．

#### JWT tokenを生成
次のコマンドを実行する．

```bash
docker run --rm \
  --env-file=/path/to/server.<name>.env \
  -v /absolute_path/to/directory_with_config.yaml:<mount_path> \
  ghcr.io/acompany-develop/quickmpc-bts:<tag> ./beaver_triple_service generateJwt \
  --file <mount_path>/to/<name>.yaml \
  --output <mount_path>

# 具体例
docker run --rm \
  --env-file=/config_dir/server.sample.env \
  -v /config_dir:/tmp \
  ghcr.io/acompany-develop/quickmpc-bts:0.3.3 ./beaver_triple_service generateJwt \
  --file /tmp/sample.yaml \
  --output /tmp
```

`--output` で指定したPATH直下にクライアント向けの`client.<name>.env` というファイル形式の設定ファイルが以下の形式で生成される．
```env
BTS_TOKEN=...
```
#### Clientに配置
`client.<name>.env`に記載されている環境変数(`BTS_TOKEN`)を`config/computation_container/compute{1,2,3}/`直下の `.env` に追記し，
docker-compose.ymlのccコンテナのenv_fileでこのパスを指定する．
