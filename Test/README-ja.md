Test
====
全サービスのテストコードと, それを実行するためのdocker-compose.ymlを置くディレクトリ

## docker-compose.ymlの書き方
DockerのImageには以下のステージ名を使用している.
| ステージ名 | 用途                                                                   |
| :--------- | :--------------------------------------------------------------------- |
| src        | src/で開発する時に使用する                                             |
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
      dockerfile: src/HogeContainer/Dockerfile
      target: src # dev, dep, small, medium, large等
    ...
```


## テスト方法
`Test/`で以下のコマンドを実行
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

```sh
make test t=./ComputationContainer/ # Test/ComputationContainer/ 直下のみのテストを実行したい場合
make test p=small # `small*test.sh`を実行したい場合
make test m=build # `*test.sh`のbuild処理のみ実行したい場合
make test m=run # `*test.sh`のrun処理のみ実行したい場合
make test t=./ComputationContainer/ p=small m=run # Test/ComputationContainer/直下のsmall*test.shでrun処理のみ実行したい場合
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
	docker-compose -f docker-compose.yml run small_cc /bin/sh -c "cd /QuickMPC && bazel test //Test/UnitTest:all --test_env=IS_TEST=true --test_output=errors"
}

# runの後に実行されるteardown処理を記述する関数
# INFO: runの後に副作用を残さないように初期化を行う
teardown() {
	# ここにrunの後処理を記述
	docker-compose -f docker-compose.yml down -v
}
```

## デバッグ方法
`Test/`で以下のコマンドを実行
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

```sh
make debug t=./ComputationContainer/ # Test/ComputationContainer/ 直下のみのデバッグを実行したい場合
make debug p=cc # `cc*debug.sh`を実行したい場合
make debug m=build # `*debug.sh`のbuild処理のみ実行したい場合
make debug m=run # `*debug.sh`のrun処理のみ実行したい場合
make debug t=./ComputationContainer/ m=run # Test/ComputationContainer/直下のdebug.shでrun処理のみ実行したい場合
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
