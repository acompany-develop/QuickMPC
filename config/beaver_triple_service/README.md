## 概要
srcステージのみ `config.src.json` それ以外のステージでは `config.json` を読みにいくようにしている.

## 仕組み
Dockerfileで
```Dockerfile
# packages/server/beaver_triple_service/Dockerfile
FROM ubuntu:18.04 as base
...

FROM base as src
ENV STAGE=src

...

FROM base as builder
...
```
のようにsrcステージだけSTAGEという環境変数を設定し,

アプリケーションで以下のように分岐することで切り替えを実現
```go
// packages/server/beaver_triple_service/config_store/config.go
...
func init() {
    stage := getEnv("STAGE", "")
    var ConfigJsonFilePath string
    if stage == "src" {
        ConfigJsonFilePath = "/QuickMPC/config/config.src.json"
    } else {
        ConfigJsonFilePath = "/QuickMPC/config/config.json"
    }
    err := utils.ParseJsonFIle(ConfigJsonFilePath, &Conf)
    if err != nil {
        panic(err)
    }
}
```