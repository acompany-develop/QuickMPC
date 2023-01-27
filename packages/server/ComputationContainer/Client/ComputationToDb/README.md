# データベースの値の保存形式
現在sharedb，resultdbの2つがが存在する．データは全てJson形式で保存されている．
## sharedb
Clientから送信されたテーブルデータが格納されたDB
### Format
```Json
{
  "data_id": "string",
  "value": "vector<vector<string>>",
  "sent_at": "string",
  "meta": {
    "schema": "vector<string>",
    "piece_id": "int"
  }
}
```

- data_id：テーブルデータのID
- value：テーブルデータ本体．数値を文字列化したデータが入る
- sent_at：テーブルデータが送信された日時の文字列
- schema：テーブルの各列のラベル
- piece_id：分割したデータのうち何番目のデータか．1-index
### Example
```Json
{
  "data_id": "95ac225f2f9c4d72f95c85373fd7ade9d3c22520654f45be569ae9c06c801709",
  "value": [["1","2","3"],["4","5","6"]],
  "sent_at": "1996-04-02 00:00:00",
  "meta": {
    "schema": ["s1","s1","s3"],
    "piece_id": 0
  }
}
```

## resultDB
Jobの計算結果が格納されたDB
### Format
```Json
{
  "job_uuid": "string",
  "status": "int",
  "result": "string",
  "meta": {
    "piece_id": "int"
  }
}
```
- job_uuid：jobのID
- status：JobのStatus．各値の詳細は[こちら](https://github.com/acompany-develop/QuickMPC/blob/81548efe382797191f367b4125d177fc2f1d1b1c/proto/common_types/common_types.proto#L7-L13)
- result：計算結果．jsonの詳細な形式は下記参照
- piece_id：分割したデータの内，何番目のデータか．0-index
### Example
#### mean，sum，variance，linear regression，logistic regression
resultは1次元配列のJson文字列
```Json
{
  "job_uuid": "10b6bfda-8776-4d8c-ad9a-b40fa277ff37",
  "status": 5,
  "result": "[\"1\",\"2\"]",
  "meta": {
    "piece_id": 0
  }
}
```
#### correl，meshcode
resultは2次元配列のJson文字列
```Json
{
  "job_uuid": "10b6bfda-8776-4d8c-ad9a-b40fa277ff37",
  "status": 5,
  "result": "[[\"1\",\"2\"],[\"3\",\"4\"]]",
  "meta": {
    "piece_id": 0
  }
}
```
#### SID3
resultはJson文字列
```Json
{
  "job_uuid": "10b6bfda-8776-4d8c-ad9a-b40fa277ff37",
  "status": 5,
  "result": "\"{\"att_class\":2,\"children\":[{\"att_class\":0,\"children\":[],\"class\":0.0,\"gini\":0.1,\"size\":3}],\"class\":0.0,\"gini\":0.2,\"size\":2}\""
  "meta": {
    "piece_id": 0
  }
}

```

resultを展開すると以下．
```Json
{
  "att_class":2,
  "children": [
    {
      "att_class":0,
      "children": [],
      "class":0.0,
      "gini":0.1,
      "size":3
    }
  ],
  "class":0.0,
  "gini":0.2,
  "size":2
}
```

#### join table
resultはJson文字列
```Json
{
  "job_uuid": "10b6bfda-8776-4d8c-ad9a-b40fa277ff37",
  "status": 5,
  "result": "\"{\"schema\":[\"s1\",\"s2\"],\"table\":[[\"1\",\"2\"],[\"3\",\"4\"]]}\""
  "meta": {
    "piece_id": 0
  }
}
```
resultを展開すると以下．
```Json
{
  "schema":["s1","s2"],
  "table":[["1","2"],["3","4"]]
}
```
