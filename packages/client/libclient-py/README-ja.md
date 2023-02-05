# QuickMPC-libClient-py

[English version](/README.md)

## Install Package
### 開発時
```console
$ git clone https://github.com/acompany-develop/QuickMPC-libClient-py.git
$ cd QuickMPC-libClient-py
$ pipenv sync --dev
```

## unit_test

### 全てのテスト
```console
$ pipenv run tox
```
Python3.7，3.8，3.9でのテスト，構文チェックが行われる．

### Python3.7のテスト
```console
$ pipenv run tox -e py37
```

## Demo
このデモは実際の使用を想定して本リポジトリをpip installする形で行われる．動作手順は[demo/README.md](./demo/README.md)に記載．

## Coding Style
Pythonファイルは[pep8](https://peps.python.org/pep-0008/)に準拠しており，
[flake8](https://github.com/PyCQA/flake8)により検査される．

下記コマンドで全てのファイルをフォーマットできる．
```console
$ pipenv run make fmt
```

## Python version
3.7.10

テストは3.7,3.8,3.9でされているため，いずれも動作する．
