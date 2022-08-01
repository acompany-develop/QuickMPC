Kubernetes (k8s) 上に QuickMPC をデプロイするためのスクリプトと設定ファイル群

```console
developer@host:/path/to/QuickMPC/Test/k8s$ tree .
.
├── Pipfile                         # Pipenv 環境下で用いるライブラリ管理ファイル
├── Pipfile.lock                    # Pipenv 環境下で用いるライブラリ管理ファイル
├── README.md                       # 本ファイル
├── config
│   ├── common                      # パーティ数に依存しない設定ファイルを置くディレクトリ
│   │   └── beaver-triple-service
│   │       └── Config.json
│   ├── party0                      # templates/config を元に生成される各パーティの設定ファイル
│   ├── ...
│   └── party${N-1}
├── docker-compose
│   └── docker-compose.yml          # templates/docker-compose.yml.jinja を元に生成
├── kompose
│   └── docker-compose.yml          # kompose で変換可能な docker-compose.yml の生成先
├── manifests                       # kompose により生成される k8s マニフェストを書き出すディレクトリ
├── requirements.txt                # pipenv 以外で環境を構築するためのファイル（`pipenv requirements` により生成）
├── tasks.py                        # invoke を利用したタスク記述スクリプト
└── templates                       # jinja2 を利用したテンプレートを置くディレクトリ
    ├── config                      # 各パーティの設定ファイルのテンプレートを置くディレクトリ
    └── docker-compose.yml.jinja    # docker-compose.yml を生成するためのテンプレート

46 directories, 83 files
```

# 事前準備

本ディレクトリでは、タスクランナーとしてPython3で動作する[fabric](https://www.fabfile.org/)と[invoke](https://www.pyinvoke.org/)を利用する。

簡単に説明するとinvokeはローカル環境のタスクランナーで、fabricはリモート環境のタスクランナーである。
fabricはinvokeの上に実装されている。

本ディレクトリにて、以下のコマンドを実行することでこれらをインストールできる。
```bash
$ pip3 install -r requirements.txt
```

その後、invokeコマンドが実行可能なことを確認する。
```
$ invoke --version
Invoke 1.7.1
```

invoke でタブ補完を有効にしたい場合、以下のコマンドを実行する。

```bash
echo "source <(invoke --print-completion-script bash)" >> ~/.bashrc
source ~/.bashrc
```

# 使い方

1. 設定ファイルの生成

```bash
invoke generate-config -p <party number>
```

2. `docker-compose.yml` の生成

```bash
invoke generate-docker-compose -p <party number> -d <docker image tag>
# 具体例
invoke generate-docker-compose -d ':s20220608'
```

3. kompose で変換可能な `docker-compose.yml` の生成

```bash
invoke filter-for-kompose
```

4. k8s マニフェストの生成

```bash
invoke generate-k8s-manifests
```

5. k8s マニフェストの hostPath を修正

```bash
invoke replace-k8s-host-path
```

6. k8s マニフェストと設定ファイルをデプロイ先に送信

```bash
invoke transfer-files -d <destination>
# 具体例
invoke transfer-files -d ubuntu@ec2-13-215-46-105.ap-southeast-1.compute.amazonaws.com
```
