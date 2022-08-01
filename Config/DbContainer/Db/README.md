src/DbContainer/Db/Dockerfileにて以下の方法でパス指定するため、
この階層のディレクトリ名は全て小文字にする

```docker
ARG lowercase_DB_NAME
# ENTRYPOINTで使えるようにENVで登録
ENV lowercase_DB_NAME=${lowercase_DB_NAME}
COPY ./Config/DbContainer/Db/${lowercase_DB_NAME}/startup /startup
```
