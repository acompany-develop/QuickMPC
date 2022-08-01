## AnytoDbクラスの使用方法
### クエリ送信
1. Clientを作成する
```C++
// ShareDBを操作する場合
auto client = AnyToDb::Client("sharedb");
```

2. N1QLを作成する
```C++
// bucketを指定する
// sharedbであれば"share"か"result""
auto n1ql = AnyToDb::N1QL("share");

// N1QLクラスのメソッドでクエリを生成する
auto query = n1ql.insert("hoge");
```

3. DBにクエリを送る
```C++
// クエリの結果をJSON文字列として得る
auto response = client.executeQuery(query);
```


### N1QL文
下記のN1QL文を用意している．なお，N1QLクラスを使わずquery文字列を直で書いてもクエリは送れる．
- insert(N1QLValue data)
  - {key: value}形式のdataを保存する
  - keyを指定しない場合デフォルトでCasが与えられる
  - 保存した要素を取得する
- bulkinsert(std::vector<N1QLValue> data)
  - {key: value}形式のdataを複数個保存する
  - insertよりも効率的なため可能な場合はこちらの使用を推奨
- select()
  - バケット内の全てを取得する
- select_id(string id_name, string id)
  - {"id_name": id}の要素を持つ値を全て取得する
- delete_id(string id_name, string id)
  - {"id_name": id}の要素を持つ値を全て削除する
  - 削除した要素を取得する
- delete_order(int num)
  - 保存が古い順にnum個削除する
  - 削除した要素を取得する


### N1QL文追加
新たなN1QL文が欲しい場合はN1QLクラスに追加する．
ただしN1QL文は共通部分が多いため，N1QLClauseクラスにそれらの処理を抽出すると使い回しがしやすくなる．
ただし，追加する場合は下の**注意**を読むこと
```C++
N1QL::newQuery(){
  // AとBは他のクエリでも使いまわせる
  return clause.commonA() +
         clause.commonB();
}

// よく使う文を抽出する
N1QL::N1QLClause::commonA(){
  return "common query A";
}
N1QL::N1QLClause::commonB(){
  return "common query B";
}

```

### 注意
要素数を取得するために，SELECT Countクエリは叩いてはならない．
なお，詳細に関しては [CouchbaseにN1QLでSELECT Count(*) クエリを叩かず要素数を数える](/Docs/count-elements-without-hitting-query.md) を参照いただきたい．
### Injection対策
N1QLClauseクラスにInjection対策としてescape_injectionメソッドが用意されている．
新たなN1QL文を作成する場合はパラメータに対してこのメソッドを適用することとする．
```C++
N1QL::N1QLClause::common(string param){
    // paramを直接埋め込まず，サニタイズしてから埋め込む
    return "hoge"
           escape_injection(param, N1QL_Hoge_Param) +
           "huga";
}
```
