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
このデモは実際の使用を想定して本リポジトリをpip installする形で行われる．動作手順は[../../../demo/client_demo/README.md](../../../demo/client_demo/README-ja.md)に記載．

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

## 入力仕様
`qpd.read_csv`で受け取るCSVデータではあらゆる数値，文字列を入力として受け付け，
数値は64bit浮動小数，文字列は任意制度整数として扱う．
そのため，64bit浮動小数で表現できない数値については精度が保証されない．特にマッチングで使用する列の場合は全く異なる値にparseされる可能性があるため，文字列として入力することを推奨する．
