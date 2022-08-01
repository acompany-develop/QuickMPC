CouchbaseServerのコンテナを立ち上げる際に, エントリーポイントとして以下を設定している.

```bash
ENTRYPOINT ["./configure-node.sh"]
```

`./configure-node.sh` が初期化スクリプトになっており, [couchbasefakeit](https://github.com/brantburnett/couchbasefakeit)というリポジトリを参考にしている.

CouchbaseServerを立ち上げるコンテナ内に必要な各種ディレクトリ, ファイルの概要は以下.

- data/: DBを永続化している場所
- nodestatus/: `initialized`というファイルの有無で初期化済みか否かを判断する
    - **初期化が途中で失敗した場合は手動でこれを消し, 必要に応じて永続か先のファイルも消す必要があることに注意!!!**
    - DockerfileでImageの中に作成しておく
- scripts/: 初期化スクリプトの集合
    - DockerfileでImageの中にコピーしておく
- startup/: 初期化スクリプトが参照するためのファイル置き場
    - **このディレクトリの詳細は[引用元のドキュメント](https://github.com/brantburnett/couchbasefakeit#creating-indexes)を参照**
    - docker-composeでコンテナ立ち上げの際にConfigからマウントする

## 永続化

Couchbaseをコンテナの外で永続化したい場合は, `data/` ディレクトリを作り, docker-compose.ymlに以下の行を追加してマウントする. これによってDBの内部データがホスト側に蓄積されていく.

```bash
volumes:
	- type: bind
	        source: ./Db/data
	        target: /opt/couchbase/var
```

## 初期化済みチェック

引用元のcouchbasefakeitでは, `/nodestatus/initialized`というファイルの有無で初期化済みか否かを判断するように初期化スクリプトが実装されている.

そのため, 一度初期化が途中で失敗したり再初期化を行いたい場合は, コンテナ立ち上げ後, 中のshellに入って`/nodestatus/initialized` をrmする必要がある.

## CouchbaseServerの初期化パラメータ

初期化の際の構成を表すパラメータは`.env`, もしくは`.db.env`に環境変数として定義されており, 各種パラメータの説明は[ここ](https://github.com/brantburnett/couchbasefakeit#environment-variables)を参照.

```bash
環境変数	説明
-------------------------------------------------------------------------------------------------------
CB_CLUSTER_NAME	クラスターの名前を指定します
CB_DATARAM	メガバイト単位のデータサービスRAM、デフォルト 512
CB_INDEXRAM	メガバイト単位のインデックスサービスRAM、デフォルト 256
CB_SEARCHRAM	検索（FTS）サービスRAM（メガバイト）、デフォルト 256
CB_ANALYTICSRAM	メガバイト単位の分析サービスRAM。CB_SERVICESにcbasが追加された場合にのみ適用されます
CB_EVENTINGRAM	メガバイト単位のイベントサービスRAM。CB_SERVICESにeventingが追加された場合にのみ適用されます
CB_SERVICES	有効にするサービス、デフォルト kv,n1ql,index,fts
CB_INDEXSTORAGE	インデックスストレージモード、forestdb（デフォルト）またはmemory_optimized
CB_USERNAME	Couchbaseユーザー名、デフォルト Administrator
CB_PASSWORD	Couchbaseパスワード、デフォルト password
-------------------------------------------------------------------------------------------------------
CB_SERVICESおよびCB_INDEXSTORAGEの値は、Couchbase RESTAPIのパラメーターに対応します。

注：CB_SERVICESに分析サービスcbasを作成するように構成する場合は、CB_ANALYTICSRAMの最小値に1024を設定してください。
```

## バケットの構成

バケットを自動的に作成したい場合は, `/startup` ディレクトリに`bucket.json` を配置するだけ.
`bucket.json` の書き方は[ここ](https://github.com/brantburnett/couchbasefakeit#bucket-configuration)を参照.

```bash
# bucket.jsonの例
# 例) sampleバケットとdefaultバケットを作りたい場合

[
  {
    "name": "sample",
    "ramQuotaMB": 100,
    "bucketType": "couchbase",
    "authType": "sasl",
    "saslPassword": "",
    "evictionPolicy": "fullEviction",
    "replicaNumber": 0,
    "flushEnabled": 1
  },
  {
    "name": "default",
    "ramQuotaMB": 100,
    "bucketType": "couchbase",
    "authType": "sasl",
    "saslPassword": "",
    "evictionPolicy": "fullEviction",
    "replicaNumber": 0,
    "flushEnabled": 1
  }
]
```

## インデックスの作成

バケットにindexを作成したい場合は,
`/startup` ディレクトリ下のバケット名のディレクトリのなかに`indexes.n1ql` を配置する.

`default` というバケットにインデックスを作成したい場合は, `startup/default/indexes.n1ql` を作る.

`indexes.n1ql` の書き方は[ここ](https://github.com/brantburnett/couchbasefakeit#creating-indexes)を参照.

```bash
CREATE PRIMARY INDEX `#primary` ON `default` USING GSI WITH {"defer_build": true};
BUILD INDEX ON `default` (`#primary`);
```
