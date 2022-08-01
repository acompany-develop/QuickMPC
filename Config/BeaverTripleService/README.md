## 概要
srcステージのみ `Config.src.json` それ以外のステージでは `Config.json` を読みにいくようにしている.

## 仕組み
Dockerfileで
```Dockerfile
# src/BeaverTripleService/Dockerfile
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
// src/BeaverTripleService/ConfigStore/Config.go
...
func init() {
    stage := getEnv("STAGE", "")
    var ConfigJsonFilePath string
    if stage == "src" {
        ConfigJsonFilePath = "/QuickMPC-BTS/Config/Config.src.json"
    } else {
        ConfigJsonFilePath = "/QuickMPC-BTS/Config/Config.json"
    }
    err := utils.ParseJsonFIle(ConfigJsonFilePath, &Conf)
    if err != nil {
        panic(err)
    }
}
```