Test
====
全サービスのテストコードと, それを実行するためのdocker-compose.ymlを置くディレクトリ

## 事前準備
本scriptsではオプションで監視ツールを有効化できる。
現時点での対応サービスは以下
- [Datadog](https://www.datadoghq.com)

### Datadogを有効化する場合
`scripts/`に`.env`ファイルを作成し、以下の形式でDatadogが用意した環境変数を設定できる。
`DD_SITE`と`DD_API_KEY`は必須なので、ご利用のDatadogアカウントを参照して適切な値を設定する。
DD_SIREは[こちら](https://docs.datadoghq.com/ja/agent/troubleshooting/site/)、DD_API_KEYは[こちら](https://docs.datadoghq.com/ja/account_management/api-app-keys/)を参照。

```
# scripts/.envの例
DD_SITE=<ここDatadogのリージョンを設定する:us5.datadoghq.com>
DD_API_KEY=<ここDatadogのAPI_KEYを設定する>
DD_APM_ENABLED=true
DD_LOGS_ENABLED=true
DD_LOGS_CONFIG_CONTAINER_COLLECT_ALL=true
DD_AC_EXCLUDE="name:datadog"
```

## docker-compose.ymlの書き方
DockerのImageには以下のステージ名を使用している.
| ステージ名 | 用途                                                                   |
| :--------- | :--------------------------------------------------------------------- |
| src        | packages/で開発する時に使用する                                             |
| dev        | 3台同時に建てたい時に使用するステージ                                  |
| small      | CIのsmallテストで使用するステージ                                      |
| medium     | CIのmediumテストで使用するステージ                                     |
| large      | CIのlargeテストで使用するステージ                                      |
| dep        | デプロイに使用するステージ                                             |
| builder    | すべてのソースコードをImageの中でbuildしておく基底ステージ             |
| *-runner   | 実行用途の軽量な`alpine`イメージであることを明示したい時につけるsuffix |

docker-compose.ymlでどのステージのImageを使うかは以下のように指定する.
```yaml
...

services:
  <ステージ名>_<コンテナ名>:
    ...
    build:
      context: ../
      dockerfile: packages/server/HogeContainer/Dockerfile
      target: src # dev, dep, small, medium, large等
    ...
```


## テスト方法
`scripts/`で以下のコマンドを実行
```sh
make test
```
特定のtestを指定して実行したい場合は以下のようにする<br>
オプションは複数指定できる<br>

### オプション <br>
t = hoge	# hoge配下のtestを実行<br>
p = huga	# huga*test.shを実行する <br>
m = build	# test.shのbuild()のみ実行<br>
m = run		# test.shのrun()のみ実行 <br>
dd = 1		# 監視SaaSの[Datadog](https://www.datadoghq.com)の有効化フラグ([事前準備](#事前準備)が必須)

```sh
make test t=./computation_container/ # scripts/computation_container/ 直下のみのテストを実行したい場合
make test p=small # `small*test.sh`を実行したい場合
make test m=build # `*test.sh`のbuild処理のみ実行したい場合
make test m=run # `*test.sh`のrun処理のみ実行したい場合
make test t=./computation_container/ p=small m=run # scripts/computation_container/直下のsmall*test.shでrun処理のみ実行したい場合
make test m=run dd=1 # datadog-agentを有効化しながら`*test.sh`のrun処理のみ実行したい場合
```

## テストの追加
以下の形式でtest.shを作成

NOTE: 関数の内部処理はexit_statusのANDを取るために必ずワンライナーで記述する

(例
```
build() {
    docker-compose -f docker-compose.yml pull dev_bts && \
    docker-compose -f docker-compose.yml build medium_cc1 medium_cc2 medium_cc3
}
```

```sh
# !!!!!!!!!! NOTE: 関数の内部処理はexit_statusのANDを取るために必ずワンライナーで記述する !!!!!!!!!!


# build処理を記述する関数
build() {
	# ここにbuild処理を記述
	docker-compose -f docker-compose.yml build small_cc
}

# runの前に実行されるsetup処理を記述する関数
# INFO: runを冪等に実行できるように初期化を行う
setup() {
	# ここにrunの前処理を記述
	docker-compose -f docker-compose.yml down -v
}

# run処理を記述する関数
# NOTE: この関数は例外的にワンライナーで書かなくて良い
run() {
	# ここにtest実行処理を記述
	docker-compose -f docker-compose.yml run small_cc /bin/sh -c "cd /QuickMPC && bazel test //test/unit_test:all --test_env=IS_TEST=true --test_output=errors"
}

# runの後に実行されるteardown処理を記述する関数
# INFO: runの後に副作用を残さないように初期化を行う
teardown() {
	# ここにrunの後処理を記述
	docker-compose -f docker-compose.yml down -v
}
```

## デバッグ方法
`scripts/`で以下のコマンドを実行
```sh
make debug # `make debug p=mc`と等価
```
特定のdebug.shを指定して実行したい場合は以下のようにする <br>
オプションは複数指定できる<br>

### オプション <br>
t = hoge	# hoge配下のtestを実行<br>
p = huga	# huga*debug.shを実行する <br>
m = build	# debug.shのbuild()のみ実行<br>
m = run		# debug.shのrun()のみ実行 <br>
dd = 1		# 監視SaaSの[datadog](https://www.datadoghq.com)の有効化フラグ([事前準備](#事前準備)が必須)

```sh
make debug t=./computation_container/ # scripts/computation_container/ 直下のみのデバッグを実行したい場合
make debug p=cc # `cc*debug.sh`を実行したい場合
make debug m=build # `*debug.sh`のbuild処理のみ実行したい場合
make debug m=run # `*debug.sh`のrun処理のみ実行したい場合
make debug t=./computation_container/ m=run # scripts/computation_container/直下のdebug.shでrun処理のみ実行したい場合
make debug m=run dd=1 # datadog-agentを有効化しながら`*debug.sh`のrun処理のみ実行したい場合
```

## テストの追加
以下の形式でdebug.shを作成
```sh
build() {
	# ここにbuild処理を記述
	docker-compose -f docker-compose.yml build dev_cc1 dev_cc2 dev_cc3
}

setup() {
	# ここにrunの前処理を記述
	docker-compose -f docker-compose.yml down -v
}

run() {
	# ここにdebug実行処理を記述
	docker-compose -f docker-compose.yml up dev_cc1 dev_cc2 dev_cc3
}
```
