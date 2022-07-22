# QuickMPC-libClient-pyを介したTestやBenchmark
## Test
`LibClient/src/tests/`以下がTestコードであり，実行は次のコマンドで行う．
```console
$ make test t=LibClient p=medium
```
特定のTestのみ動作させたい場合は
[Test/docker-compose.ymlのcommandを記述した部分](https://github.com/acompany-develop/QuickMPC/blob/86bf97a1959a7b8351324e1ee55081530fbce08d/Test/docker-compose.yml#L1084)
で，test optionとして`-k {実行したいtest名}`を追加する．
```yaml
# before
command: ["/bin/bash", "-c", "pipenv run pytest src/tests -s -v -log-cli-level=DEBUG"]
# after
command: ["/bin/bash", "-c", "pipenv run pytest src/tests -s -v -log-cli-level=DEBUG -k correl"]
```

## Benchmark
`LibClient/src/benchmark/`以下がBenchmarkコードであり，実行は次のコマンドで行う．
```console
$ make test t=LibClient p=benchmark
```
特定のBenchmarkのみ動作させたい場合は
[Test/docker-compose.ymlのcommandを記述した部分](https://github.com/acompany-develop/QuickMPC/blob/86bf97a1959a7b8351324e1ee55081530fbce08d/Test/docker-compose.yml#L1107)
で，test optionとして`-k {実行したいtest名}`を追加する．
```yaml
# before
command: ["/bin/bash", "-c", "pipenv run pytest src/benchmark -s -v -log-cli-level=DEBUG"]
# after
command: ["/bin/bash", "-c", "pipenv run pytest src/benchmark -s -v -log-cli-level=DEBUG -k correl"]
```


## QuickMPC-libClient-pyのバージョン変更のやり方
```sh
# submoduleとしてのQuickMPC-libClient-pyでcheckoutする
cd QuickMPC-libClient-py
git checkout <該当のブランチ，又はコミットハッシュ>
# いつも通りのcommitをする
cd ../
git add .
git commit -m 'commit message'
```
