# QuickMPC
[![Build and Run Tests on CC, MC, and BTS Containers](https://github.com/acompany-develop/QuickMPC/actions/workflows/test-server-all.yml/badge.svg)](https://github.com/acompany-develop/QuickMPC/actions/workflows/test-server-all.yml)
[![Syntax Check and Unit Test on libclient-py](https://github.com/acompany-develop/QuickMPC/actions/workflows/test-client-libclient-py.yml/badge.svg)](https://github.com/acompany-develop/QuickMPC/actions/workflows/test-client-libclient-py.yml)

<img src="https://user-images.githubusercontent.com/1694907/182115030-90fda7cf-068a-48bb-ba50-ee12be6af0b4.png" width=128>

[English version](./README.md)

QuickMPCはPEC（Privacy-Enhancing Computation）のひとつであるSMPC（Secure Multiparty Computation）の実用的なエンジンです。

基本的な統計量計算に用いるプライバシーデータを、秘匿化しつつ計算することができます。これにより世界中の企業やエンジニアはSMPCの専門的な知識不要で、Pythonのクライアントライブラリを用いて、SMPC機能を利用したSaaSプロダクトを簡単に開発できるようになります。

![oss_eg](https://user-images.githubusercontent.com/1694907/182254973-ee3092a6-ee28-49bb-aaf6-637225271a0b.png)

## 依存ツール/コマンド
- `make`
- `docker`
- `docker-compose`
- [`vscode-yaml-sort`](https://marketplace.visualstudio.com/items?itemName=PascalReitermann93.vscode-yaml-sort)

## Getting Started
[QuickMPC-libClient-pyのデモ](packages/client/libclient-py/demo/README-ja.md)

## 開発時の注意
### 推奨スペック
- CPU `4コア` 以上
- メモリ `64GB` 以上
- ストレージ `64GB` 以上

## QuickMPC全体のテスト方法
1. ディレクトリを移動する
```sh
cd QuickMPC/scripts
```
2. 全コンテナを起動する
```sh
make debug t=./manage_container # 全てのコンテナが起動
```
3. libclientのdemoを実行する
- [libClient-py/demo](packages/client/libclient-py/demo/README-ja.md)

## 各コンテナのテスト方法
[詳細はこちら](scripts/README-ja.md)
1. ディレクトリを移動する
```sh
cd QuickMPC/scripts
```
2. テストを開始する
```sh
# 全てのコンテナをテストする場合
make test
# Computation Containerの場合
make test t=./computation_container/
# Manage Containerの場合
make test t=./manage_container/
```

## 各コンテナの起動方法
2コンテナ立ててあえて手動でtestしたい時に使用する
```sh
make debug t=./computation_container # dev_cc{1,2,3}, dev_btsの立ち上げ
make debug t=./manage_container # 全てのコンテナが起動
```

## 各コンテナの開発方法
### Computation Container
[詳細はこちら](packages/server/computation_container/README.md)
1. ディレクトリを移動する
```sh
cd packages/server/computation_container
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

[詳細はこちら](packages/server/manage_container/README.md)
1. ディレクトリを移動する
```sh
cd packages/server/manage_container
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
