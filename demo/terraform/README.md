# QMPCをk8s上で動かす手順

## インフラ構築
terraformインストール
[公式ドキュメント](https://developer.hashicorp.com/terraform/tutorials/aws-get-started/install-cli)を参考にインストール
### AWS
1. aws-cliインストール
[公式ドキュメント](https://docs.aws.amazon.com/ja_jp/cli/latest/userguide/getting-started-install.html)を参考にインストール
2. config設定
`ダッシュボード > セキュリティ認証情報 > AWS IAM認証情報 > アクセスキーの作成`
から，アクセスキーとシークレットキーを取得して以下のように設定
```
$ aws configure
AWS Access Key ID [None]:
AWS Secret Access Key [None]:
Default region name [None]:
Default output format [None]:
```
3. makeコマンド実行
※ カスタマイズしたい場合は先に`QuickMPC-Internal/Test/k8s/terraform/aws/variables.tf`を適当に書き換える
```
$ make create-aws-instance
```
### GCP
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
`QuickMPC-Internal/Test/k8s/terraform/gcp/variables.tf`の`service_account_json`及び`project_id`を設定
5. makeコマンド実行
※ カスタマイズしたい場合は先に`QuickMPC-Internal/Test/k8s/terraform/gcp/variables.tf`を適当に書き換える
```
$ make create-gcp-instance
```

## kubeadmインストール
作成したインスタンスにssh接続する（インスタンス作成時に`./terraform/<aws/gcp>/.key_pair/`に鍵を作成済みのため，これを使用してsshすると楽）
また，接続後に本リポジトリをcloneし，以下のコマンドを実行
```
$ make install
```

## k8sクラスター起動
```
$ make init
```

## マニフェスト作成
`QuickMPC-Internal/Test/k8s/config.yaml`に適切なパーティ情報を記述し，以下のコマンドを実行
```
$ make generate-manifests
```

## マニフェストデプロイ
```
$ make deploy-manifests
```
