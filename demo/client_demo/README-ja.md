[English version](./README.md)

# デモの実行方法
デモは実際の使用状況を想定して，QuickMPC-libClient-pyをpip installしたDemo用の環境で行う．開発用のPython環境とは異なる環境が必要な点に注意する．

### 1. コンテナを立ちあげる
[QuickMPC/scripts/README-ja.md](../../../../scripts/README-ja.md)の**デバッグ方法**を参考にして，QuickMPCの全てのコンテナをdevステージで立ち上げる


### 2. 本ディレクトリで下記コマンドを実行して環境を構築する
```console
$ pipenv install --skip-lock
```
Python3.7以外で実行確認したい場合は，install前に次のコマンドを実行する．
```console
$ pipenv --rm # 今入っている環境を削除する
$ pipenv --python=3.8 # Python3.8の場合
$ pipenv --python=3.9 # Python3.9の場合
```

### 3. ディレクトリを移動して実行する
各操作ごとのデモは`unit_demo/`，全体のデモは`integration_demo/`で実行可能．
また，`send_share.py`，`send_asss_share.py`，`demo_sharize.py`はコマンドライン引数にファイルのpathを指定する必要がある.
```console
$ cd unit_demo/
$ pipenv run python send_share.py [file_path] # シェア送信のデモを実行
$ pipenv run python execute_computation.py # 計算リクエストのデモを実行
```
なお，`unit_demo/execute_computation.py`などの一部のunit_demoは，動作がsend_shareなどから得られるID情報に依存するため，適宜IDを書き換える必要がある．
