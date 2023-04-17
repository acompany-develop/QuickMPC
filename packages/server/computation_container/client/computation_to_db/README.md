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
  "matching_column": 1,
  "meta": {
    "schema": "vector<SchamaType>",
    "piece_id": "int"
  }
}
```
SchemaTypeは以下で定義される．
```Json
{
 "name": "string",
 "type": "int"
}
```

- data_id：テーブルデータのID
- value：テーブルデータ本体．数値を文字列化したデータが入る
- sent_at：テーブルデータが送信された日時の文字列
- matching_column: 突合に用いる列番号．1-index
- schema：テーブルの各列のラベル
  - name：ラベル名
  - type：列の値の型情報
- piece_id：分割したデータのうち何番目のデータか．0-index
### Example
```Json
{
  "data_id": "95ac225f2f9c4d72f95c85373fd7ade9d3c22520654f45be569ae9c06c801709",
  "value": [["1","2"],["3","4"]],
  "sent_at": "1996-04-02 00:00:00",
  "matching_column": 1,
  "meta": {
    "schema": [
      {"name": "s1", "type": 1},
      {"name": "s2", "type": 1}
    ],
    "piece_id": 0
  }
}
```

## resultDB
実行中のステータスを表すStatusファイルと実行結果が格納されたDB．

### StatusファイルのFormat
`status_{status名}`というファイルが生成され，中には何も記載されていない．
各種Statusは[common_types.proto](https://github.com/acompany-develop/QuickMPC/blob/6ccd25d70797c29c4a518f87b927b3e3993942b9/proto/common_types/common_types.proto#L7-L14)に定義されている．
結果の保存が完了してJobが終了すると`completed`というファイルがStatusファイルとは別で生成される．
```console
$ ls
completed  status_COMPLETED  status_PRE_JOB  status_RECEIVED
dim1_0     status_COMPUTE    status_READ_DB
```

### 計算結果のFormat
`dim1_{piece_id}`,`dim2_{piece_id}`,`schema_{piece_id}`のいずれかが生成されてFormatは共通して下記．
```Json
{
  "job_uuid": "string",
  "result": "vector<string>",
  "meta": {
    "column_number": "int",
    "piece_id": "int"
  }
}
```
- job_uuid：jobのUUID
- result：計算結果．全て1次元配列で保存されるため，parseはclientに委ねられる
- column_number：計算結果の列数．2次元の場合のparseに用いられる
- piece_id：分割したデータの内，何番目のデータか．0-index
### Example
#### mean，sum，variance
`dim1_{piece_id}`が生成される．
```Json
{
  "job_uuid": "11ecbfe4-6879-492a-9396-677e8af71a3f",
  "meta": {
    "column_number": 3,
    "piece_id": 0
  },
  "result": [
    "358879410315362192256.60406361000000000000",
    "-13267146656954070499.18324522000000000000",
    "123968118293122970333.90567691000000000000"
  ]
}
```
#### correl，meshcode
`dim2_{piece_id}`が生成される．
```Json
{
  "job_uuid": "43a36299-f0b7-43eb-a554-f7fcf9542ac4",
  "meta": {
    "column_number": 3,
    "piece_id": 0
  },
  "result": [
    "526557084340329216.73165318000000000000",
    "18359293928344826031.92501963000000000000",
    "-3889930561108553035.34126033000000000000",
    "1397924581144026367.92980468000000000000",
    "8510392564393579009.91548902000000000000",
    "-5717400980662968640.58619546000000000000"
  ]
}
```
#### join table
`schama_{piece_id}`と`dim2_{piece_id}`の2つのファイルが生成される．
##### schema_{piece_id}
```Json
{
  "job_uuid": "2490985c-f69e-4294-9500-91bd86aeea64",
  "result": [
    "{\"name\":\"attr1\",\"type\":\"SHARE_VALUE_TYPE_FIXED_POINT\"}",
    "{\"name\":\"attr2\",\"type\":\"SHARE_VALUE_TYPE_FIXED_POINT\"}",
    "{\"name\":\"attr3\",\"type\":\"SHARE_VALUE_TYPE_FIXED_POINT\"}"
  ],
  "meta": {
    "column_number": 3,
    "piece_id": 0
  }
}
```
##### dim2_{piece_id}
```Json
{
  "job_uuid": "2490985c-f69e-4294-9500-91bd86aeea64",
  "meta": {
    "column_number": 3,
    "piece_id": 0
  },
  "result": [
    "4561551908293362636.02215021",
    "-15127870372172533519.64980847",
    "-14893551307295469992.53099650",
    "9475404461181305842.93393583",
    "-12958388830167869664.46944241",
    "5257983048358581332.678723316"
  ]
}
```
