# QuickMPC-libClient-pyを介したTestやBenchmark
## Test
`libclient/src/tests/`以下がTestコードであり，実行は次のコマンドで行う．
```console
$ make test t=libclient p=medium
```
### 特定のTestのみ動作させる
`scripts/docker-compose.medium.yml`のcommandを記述した部分で，test optionとして`-k {実行したいtest名}`を追加する．
```yaml
# before
command: ["/bin/bash", "-c", "pipenv run pytest src/tests -s -v -log-cli-level=DEBUG"]
# after
command: ["/bin/bash", "-c", "pipenv run pytest src/tests -s -v -log-cli-level=DEBUG -k correl"]
```

## Benchmark
`libclient/src/benchmark/`以下がBenchmarkコードであり，実行は次のコマンドで行う．
```console
$ make test t=libclient p=benchmark
```
### 特定のBenchmarkのみ動作させる
`scripts/docker-compose.benchmark.yml`のcommandを記述した部分で，test optionとして`-k {実行したいtest名}`を追加する．
```yaml
# before
command: ["/bin/bash", "-c", "pipenv run pytest src/benchmark -s -v -log-cli-level=DEBUG"]
# after
command: ["/bin/bash", "-c", "pipenv run pytest src/benchmark -s -v -log-cli-level=DEBUG -k correl"]
```
### サーバに置いたQuickMPCに対してBenchmarkを動作させる
[scripts/libclient/src/utils.pyで指定しているIP](./src/utils.py#L6-L10)
を対象のIPに変更する．
```Python
qmpc: QMPC = QMPC([
    "http://<party1のIP>:50000",
    "http://<party2のIP>:50000",
    "http://<party3のIP>:50000",
])
```
