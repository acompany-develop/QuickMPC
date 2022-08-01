Scripts/
---
特定のブランチへのマージと同時にECS上にQuickMPCを自動デプロイするのを実現するためのリポジトリ

```bash
# Scripts/の中身
.
├── Pipfile # pipenv環境下で用いるライブラリ管理ファイル
├── Pipfile.lock # pipenv環境下で用いるライブラリ管理ファイル
├── README.md
├── invoke.yaml # invokeの設定ファイル
├── outputs # invokeタスクのファイル出力置き場
│   └── ...
├── requirements.txt # システムpython(pip3)環境下でのライブラリ管理ファイル
└── tasks.py # invokeを利用したタスク記述スクリプト

2 directories, 8 files
```


# 前提条件
- Pipenv
    - version 2022.1.8
- Python
    - version 3.8.9

# 事前準備
[Docker ComposeとAmazon ECSのチュートリアル実行](/Docs/docker-compose-and-ecs-tutorial-run.md)を行うために必要な事前準備を行う。

本リポジトリでは、タスクランナーとしてPython3で動作する[fabric](https://www.fabfile.org/)と[invoke](https://www.pyinvoke.org/)を利用する。

簡単に説明するとinvokeはローカル環境のタスクランナーで、fabricはリモート環境のタスクランナーである。
fabricはinvokeの上に実装されている。

`Scripts/`にて、以下のコマンドを実行することでこれらをインストールできる。
```bash
$ pip3 install -r requirements.txt
```

その後、invokeコマンドが実行可能なことを確認する。
```
$ invoke -V
Invoke 1.6.0
```

デフォルトレジストリへの認証。
```
$ aws ecr get-login-password --region [region] | docker login --username AWS --password-stdin [aws_account_id].dkr.ecr.[region].amazonaws.com
Login Succeeded
```


# 使い方

1. AWSインフラリソースの作成
```bash
invoke create-infra -p <party number>
```

2. 作成したAWSインフラリソースのIDを取得
```bash
invoke get-infra-ids -p <party number>
```

3. セキュリティグループの更新
```bash
invoke update-security-group -p <party number>
```

4. ALBの証明書作成
```bash
invoke create-certs -p <party number> -d <domain prefix>
```

5. AWS CodePipelineの作成
```bash
invoke create-pipeline -p <party number>
```

6. 設定ファイル生成
```bash
invoke generate-configs -p <party number> -d <domain prefix>
```

7. Applicationのデプロイ
```bash
invoke deploy-to-s3 -p <party number>
```

8. 作成した AWS リソースの削除
```bash
invoke delete-all -p <party number>
```

# その他
デバッグのために, コンテナに入る際はECS Execを使用する. task-id等はダッシュボードなどから確認可能. Session Manager pluginがインストールされている必要があるため注意.

```:bash
aws ecs execute-command --cluster cluster-name \
    --task task-id \
    --container container-name \
    --interactive \
    --command "/bin/sh"

// 具体例
aws ecs execute-command --cluster compose-infrastructure0-cluster --task 44304196b7414ba3bf3daee1bc7e8fd6 --container computation_container --interactive --command "/bin/ash"

```
