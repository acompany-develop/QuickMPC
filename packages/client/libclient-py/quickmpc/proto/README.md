# grpcコード生成方法
レポジトリに各ファイルを含めているため，protoファイルを変更する場合以外では再生成不要
## .protoからpythonファイルを生成する
```bash
cd quickmpc/proto
pipenv run python generate_grpc.py
```
次の3ファイルが生成されれば成功
- libc_to_manage_pb2.py
- libc_to_manage_pb2_grpc.py
- common_types/common_types_pb2.py

