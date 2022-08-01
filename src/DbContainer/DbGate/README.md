Db Gate
====

## gRPCのコード生成方法
.protoファイル内でgo_package optionを指定
例
```
option go_package = "Proto/Huga";
```

コード生成
コンテナ内で実行
Proto/HugaディレクトリにHuga.pb.goが出力される
```
protoc Proto/Hoge/Huga.proto --go_out=plugins=grpc:.
```