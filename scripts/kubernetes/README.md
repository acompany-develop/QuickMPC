# インフラ構築
terraformインストール
[公式ドキュメント](https://developer.hashicorp.com/terraform/tutorials/aws-get-started/install-cli)を参考にインストール
## GKEでのデプロイ
1. gcloud cliインストール
[公式ドキュメント](https://cloud.google.com/sdk/docs/install?hl=ja)を参考にインストール
2. サービスアカウントキー作成
`IAMと管理(Sandboxプロジェクト) > サービスアカウント > terraform-gke@***.iam.gserviceaccount.com > キー > 鍵を追加 > 新しい鍵を作成 > json`
から，terraform用サービスアカウントのjsonキーを取得
3. config設定
```
$ gcloud auth activate-service-account SERVICE_ACCOUNT@DOMAIN.COM --key-file=/path/key.json --project=PROJECT_ID
```
4. kubeip用のサービスアカウントキー作成
`IAMと管理(Sandboxプロジェクト) > サービスアカウント > kubeip-service-account@***.iam.gserviceaccount.com > キー > 鍵を追加 > 新しい鍵を作成 > json`
から，kubeip用サービスアカウントのjsonキーを取得<br>
※ サービスアカウントがない場合、kubeipの[リポジトリ](https://github.com/doitintl/kubeip)のREADME.mdを参考に作成してください
5. kubeip用のサービスアカウントキーの配置
`QuickMPC/scripts/kubernetes/src/json`ディレクトリに`key.json`にリネームの上配置する
6. terraformリソースの設定
`QuickMPC/scripts/kubernetes/terraform/variables.tf`の`service_account_json`及び`project_id`を設定
7. makeコマンド実行<br>
`QuickMPC/scripts`ディレクトリにて以下のコマンドを実行する
※ カスタマイズしたい場合は先に`QuickMPC/scripts/kubernetes/terraform/variables.tf`を適当に書き換える
```
$ make create-gke-cluster
```
