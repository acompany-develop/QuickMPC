Computation Container
===

## 使用方法
バックグラウンドでコンテナを起動したい場合は今のコマンドを実行
```bash
make upd # docker-compose up -d --build
```

### イメージを構築
事前にイメージを構築したい場合は以下のコマンドを実行
```bash
make build # docker-compose build
```

### イメージからコンテナの起動
構築済みのイメージからコンテナを起動する場合は以下のコマンドを実行
```bash
make up # docker-compose up --build
```
バックグラウンドで実行したい場合は以下:
```bash
make upd # docker-compose up -d --build
```
もし存在していない場合は、自動的に構築

### 起動中のコンテナに入る
実行中のコンテナのbashに入る時、以下のコマンドを実行
```bash
make login # docker-compose exec cc /bin/bash
```

### 削除
containers, networks, images, and volumesを停止かつ削除するとき以下のコマンドを実行
```bash
make rm-all # docker-compose down --rmi all --volumes
```
コンテナのみを削除する場合は以下を実行
```bash
make rm # docker-compose rm -fs
```

コンテナとネットワークを削除する場合は以下を実行
```bash
make down # docker-compose down
```

### How to start Computation Container

#### Host side settings

This is an optional instruction.
If it is skipped, logging stack trace will be skipped when Comutation Container is killed with signal.

Set core dump file destination using shell script.

```bash
./scripts/host_core_setting.sh  # needs permission to write /proc/sys/kernel/core_pattern
```

#### Start Computation Container inside a container

Launch Computation Container executable using shell script.

```bash
/QuickMPC/scripts/wrapped_run.sh /QuickMPC/bazel-bin/computation_container
```
