# インフラ構築
terraformインストール
[公式ドキュメント](https://developer.hashicorp.com/terraform/tutorials/aws-get-started/install-cli)を参考にインストール
## GCP
1. gcloud cliインストール
[公式ドキュメント](https://cloud.google.com/sdk/docs/install?hl=ja)を参考にインストール
2. サービスアカウントキー作成
`IAMと管理(Sandboxプロジェクト) > サービスアカウント > terraform@***.iam.gserviceaccount.com > キー > 鍵を追加 > 新しい鍵を作成 > json`
から，terraform用サービスアカウントのjsonキーを取得
3. config設定
```
$ gcloud auth activate-service-account SERVICE_ACCOUNT@DOMAIN.COM --key-file=/path/key.json --project=PROJECT_ID
```
4. terraformリソースの設定
`QuickMPC/demo/terraform/gcp/variables.tf`の`service_account_json`及び`project_id`を設定
5. makeコマンド実行
※ カスタマイズしたい場合は先に`QuickMPC/demo/terraform/gcp/variables.tf`を適当に書き換える
```
$ make create-gcp-instance
```
