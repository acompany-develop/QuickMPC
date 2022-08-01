Couchbase Server
---

## 参考
https://github.com/brantburnett/couchbasefakeit

詳しい説明は, [CouchbaseServerの初期化スクリプト](/Docs/initialize-script-for-couchbase.md)を参照してください.

## ディレクトリとファイルの概要

- data/: DBを永続化している場所
- nodestatus/: `initialized`というファイルの有無で初期化済みか否かを判断する
  - <span style="color: red;">初期化が途中で失敗した場合は手動でこれを消し, 必要に応じて永続か先のファイルも消す必要があることに注意</span>
  - srcステージではhostからbindする
  - devステージでは名前付きvolumeで永続化する
- scripts/: 初期化スクリプトの集合
  - DockerfileでImageの中にコピーしておく
- startup/: 初期化スクリプトが参照するためのファイル置き場
  - <span style="color: red;">このディレクトリの詳細は[引用元のドキュメント](https://github.com/brantburnett/couchbasefakeit#creating-indexes)を参照</span>
  - DockerfileでImageの中にコピーしておく
