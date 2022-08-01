# QuickMPC
[![Build CC, MC, DC, and Run Build and Test in Each Containers](https://github.com/acompany-develop/QuickMPC/actions/workflows/main.yml/badge.svg?branch=develop)](https://github.com/acompany-develop/QuickMPC/actions/workflows/main.yml)

<img src="https://user-images.githubusercontent.com/1694907/182115030-90fda7cf-068a-48bb-ba50-ee12be6af0b4.png" width=128>

[English version](./README.md)

QuickMPCはPEC（Privacy-Enhancing Computation）のひとつであるSMPC（Secure Multiparty Computation）の実用的なエンジンです。

基本的な統計量計算や軽量な機械学習に用いるプライバシーデータを、秘匿化しつつ計算することができます。これにより世界中の企業やエンジニアはSMPCの専門的な知識不要で、PythonとJavaScript(TypeScript)のクライアントライブラリを用いて、SMPC機能を利用したSaaSプロダクトを簡単に開発できるようになります。
![oss_eg](https://user-images.githubusercontent.com/1694907/182254973-ee3092a6-ee28-49bb-aaf6-637225271a0b.png)

## 依存ツール/コマンド
- `make`
- `docker`
- `docker-compose`

## Getting Started
[QuickMPC-libClient-pyのデモ](/QuickMPC-libClient-py/demo/README.md)

## 開発時の注意
### 推奨スペック

- メモリ `16GB` 以上
- ストレージ `20GB` 以上
## QuickMPC全体のテスト方法
1. ディレクトリを移動する
```sh
cd QuickMPC/Test
```
2. 全コンテナを起動する
```sh
make debug t=./ManageContainer # 全てのコンテナが起動
```
3. libClientのDemoを実行する
- libClient-ts(https://github.com/acompany-develop/QuickMPC-libClient-ts/tree/develop/Demo/Develop)
- libClient-py(https://github.com/acompany-develop/QuickMPC-libClient-py/tree/develop/Demo)

## 各コンテナのテスト方法
詳細：https://github.com/acompany-develop/QuickMPC/tree/develop/Test
1. ディレクトリを移動する
```sh
cd QuickMPC/Test
```
2. テストを開始する
```sh
# 全てのコンテナをテストする場合
make test
# Computation Containerの場合
make test t=./ComputationContainer/
# Manage Containerの場合
make test t=./ManageContainer/
# Db Containerの場合
make test t=./DbContainer/
```

## 各コンテナの起動方法
3コンテナ立ててあえて手動でtestしたい時に使用する
```sh
make debug t=./ComputationContainer # dev_cc{1,2,3}, dev_debgate{1,2,3}, dev_sharedb{1,2,3}, dev_secrets-server, dev_btsの立ち上げ
make debug t=./DbContainer # dev_debgate{1,2,3}, dev_sharedb{1,2,3}, dev_secrets-serverの立ち上げ
make debug t=./ManageContainer # 全てのコンテナが起動
```

## 各コンテナの開発方法
### Computation Container
詳細：https://github.com/acompany-develop/QuickMPC/tree/develop/src/ComputationContainer#readme
1. ディレクトリを移動する
```sh
cd src/ComputationContainer
```
2. コンテナを起動してコンテナに入る
```sh
make upd
make login
cd QuickMPC
```
なお、make loginの代わりにVsCodeの`Remote - Containers`というプラグインを使うことで、`Attatch Visual Studio Code`ボタンからContainerの中に入ってlocal環境と同様にVsCode上で開発を行うことができるのでそちらを推奨

3. 開発してbulidやテストを行う
```sh
# build
bazel build //:all
# test
bazel test ... --test_env=IS_TEST=true --test_output=errors
```

#### Bazelが生成する一時ファイルの削除
bazelが生成するファイルはコンテナ内の`/QuickMPC`ディレクトリで以下を実行すると削除できる

```
bazel clean
```

### Manage Container

詳細：https://github.com/acompany-develop/QuickMPC/tree/develop/src/ManageContainer#readme
1. ディレクトリを移動する
```sh
cd src/ManageContainer
```
2. コンテナを起動してコンテナに入る
```sh
make upd
make login
```
3. 開発してbuildやテストを行う
```sh
# /QuickMPC直下にvendorディレクトリを生成し，依存パッケージを全てコピーする
go mod vendor
# build
go build
# test
go test ... -v
```

### Db Container
DbGateとDBの二つで構成される．

### Dbgate
詳細：https://github.com/acompany-develop/QuickMPC/tree/develop/src/DbContainer
1. ディレクトリを移動する
```sh
cd src/DbContainer
```
2. コンテナを起動してコンテナに入る
```sh
make upd
make login
```
3. 開発してbuildやテストを行う
```sh
# /QuickMPC直下にvendorディレクトリを生成し，依存パッケージを全てコピー
go mod vendor
# build
go build
# test
go test ... -v
```
### DB
詳細：https://github.com/acompany-develop/QuickMPC/tree/develop/src/DbContainer/Db#readme
#### Couchbase Serverの管理画面に入る
http://localhost:8091 へアクセスし、バケットの作成
開発環境では、以下の設定を前提として実装
```bash
username: <secrets-serverで作成した`CB_USERNAME`>
password: <secrets-serverで作成した`CB_PASSWORD`>
bucket: share
```

#### Queryを試す
http://localhost:8091/ui/index.html#!/query/workbench?scenarioZoom=minute へアクセス
例
```
CREATE PRIMARY INDEX share_sample_index ON `share`;
SELECT * FROM `share`;
```
