[English version](./README.md)

# デモの実行方法
デモは実際の使用状況を想定して，QuickMPC-libClient-pyをpip installしたDemo用の環境で行う．開発用のPython環境とは異なる環境が必要な点に注意する．

### 1. コンテナを立ちあげる
[QuickMPC/demo/README.md](../README.md)の**QuickMPCを構築する**を参考にして，QuickMPCを立ち上げる


### 2. 本ディレクトリで下記コマンドを実行して環境を構築する
```console
$ pip install quickmpc
```
### 3. ディレクトリを移動して実行する
[ステップ1](#1-コンテナを立ちあげる)にてGCPで立ち上げた場合は実行ファイルの接続情報を書き換える.

```
qmpc: QMPC = QMPC(
        ["https://{サーバ1のドメイン}:50000",
         "https://{サーバ2のドメイン}:50000"]
    )
```

```console
$ cd scripts/
$ python execute_demo.py # 統計演算のデモを実行
$ python join_demo.py # 横結合のデモを実行
```
