
# QuickMPC-libClient-pyが提供する機能

## QMPC.parse_csv_file
csvファイルからテーブルデータを読み込んでパースする．
### Parameters
- file: `str`
	- 読み込むファイル名

### Returns
- parse_data: `Tuple[List[List[float]], List[str]]`
	- parse_data[0]: schemaを除くテーブルデータ
	- parse_data[1]: schema

## QMPC.csv_file_to_bitvector
csvファイルからテーブルデータを読み込んでbitvectorにパースする．
### Parameters
- file: `str`
	- 読み込むファイル名
- exclude: `List[int]`
	- bitvector化する対象から除外する列

### Returns
- parse_data: `Tuple[List[List[float]], List[str]]`
	- parse_data[0]: schemaを除くbitvector化したテーブルデータ
	- parse_data[1]: bitvector化したschema
	- 例えば"s"というschemaを持つ列を3つのbitvectorに分割すると，`["s0","s1","s2"]`となる

## QMPC.parse_csv_data
テーブルデータをパースする．
### Parameters
- data: `List[List[str]]`
	- パースするテーブルデータ

### Returns
- parse_data: `Tuple[List[List[float]], List[str]]`
	- parse_data[0]: schemaを除くテーブルデータ
	- parse_data[1]: schema

## QMPC.parse_csv_data_to_bitvector
テーブルデータをbitvectorにパースする．
### Parameters
- data: `List[List[str]]`
	- パースするテーブルデータ
- exclude: `List[int]`
	- bitvector化する対象から除外する列

### Returns
- parse_data: `Tuple[List[List[float]], List[str]]`
	- parse_data[0]: schemaを除くbitvector化したテーブルデータ
	- parse_data[1]: bitvector化したschema
	- 例えば"s"というschemaを持つ列を3つのbitvectorに分割すると，`["s0","s1","s2"]`となる

## QMPC.send_share
エンジンにテーブルデータをShare化して送信する
### Parameters
- secrets: `List[List[str]]`
	- 送信するテーブルデータ
- schema: `List[str]`
	- テーブルデータのschema
- matching_column: `int`
	- 結合する場合に用いる列
- party_size: `int`
	- エンジンのParty数
- piece_size: `int`
	- 分割データの最大サイズ
### Returns
- res: `Dict`
	- res["is_ok"]: `bool`
		- 送信が成功したかどうか
	- res["data_id"]: `str`
		- テーブルデータのID

## QMPC.delete_share
エンジンに保存されたテーブルデータの削除する
### Parameters
- data_ids: `List[str]`
	- 削除するdata_idのリスト
### Returns
- res: `Dict`
	- res["is_ok"]: `bool`
		- 送信が成功したかどうか

## QMPC.mean
平均値を計算する
### Parameters
- join_order: `Tuple[List[str], List[int], List[int]]`
	- join_order[0]: data_idのリスト
	- join_order[1]: テーブルデータの結合方向(1:横(open)，2:縦(open)，3:横(share))
	- join_order[2]: 結合に用いるIDの列リスト
	- 例えば(["d1", "d2"], [1], [1, 2])であれば，data_idがd1のテーブルデータとd2のテーブルデータをd1の1列目とd2の2列目をopenした値で突合させて横結合する
- src: `List[int]`
	- 平均値を計算する列リスト
### Returns
- res: `Dict`
	- res["is_ok"]: `bool`
		- 送信が成功したかどうか
	- res["job_id"]: `str`
		- 計算スレッドのID

## QMPC.variance
分散を計算する
### Parameters
- join_order: `Tuple[List[str], List[int], List[int]]`
	- join_order[0]: data_idのリスト
	- join_order[1]: テーブルデータの結合方向(1:横(open)，2:縦(open)，3:横(share))
	- join_order[2]: 結合に用いるIDの列リスト
	- 例えば(["d1", "d2"], [1], [1, 2])であれば，data_idがd1のテーブルデータとd2のテーブルデータをd1の1列目とd2の2列目をopenした値で突合させて横結合する
- src: `List[int]`
	- 分散を計算する列リスト
### Returns
- res: `Dict`
	- res["is_ok"]: `bool`
		- 送信が成功したかどうか
	- res["job_id"]: `str`
		- 計算スレッドのID

## QMPC.sum
総和を計算する
### Parameters
- join_order: `Tuple[List[str], List[int], List[int]]`
	- join_order[0]: data_idのリスト
	- join_order[1]: テーブルデータの結合方向(1:横(open)，2:縦(open)，3:横(share))
	- join_order[2]: 結合に用いるIDの列リスト
	- 例えば(["d1", "d2"], [1], [1, 2])であれば，data_idがd1のテーブルデータとd2のテーブルデータをd1の1列目とd2の2列目をopenした値で突合させて横結合する
- src: `List[int]`
	- 総和を計算する列リスト
### Returns
- res: `Dict`
	- res["is_ok"]: `bool`
		- 送信が成功したかどうか
	- res["job_id"]: `str`
		- 計算スレッドのID

## QMPC.correl
相関係数を計算する
### Parameters
- join_order: `Tuple[List[str], List[int], List[int]]`
	- join_order[0]: data_idのリスト
	- join_order[1]: テーブルデータの結合方向(1:横(open)，2:縦(open)，3:横(share))
	- join_order[2]: 結合に用いるIDの列リスト
	- 例えば(["d1", "d2"], [1], [1, 2])であれば，data_idがd1のテーブルデータとd2のテーブルデータをd1の1列目とd2の2列目をopenした値で突合させて横結合する
- inp: `Tuple[List[int], List[int]]`
	- inp[0]: 相関係数の左列リスト
	- inp[1]: 相関係数の右列リスト
### Returns
- res: `Dict`
	- res["is_ok"]: `bool`
		- 送信が成功したかどうか
	- res["job_id"]: `str`
		- 計算スレッドのID

## QMPC.linear_regression
線形回帰の学習を行う
### Parameters
- join_order: `Tuple[List[str], List[int], List[int]]`
	- join_order[0]: data_idのリスト
	- join_order[1]: テーブルデータの結合方向(1:横(open)，2:縦(open)，3:横(share))
	- join_order[2]: 結合に用いるIDの列リスト
	- 例えば(["d1", "d2"], [1], [1, 2])であれば，data_idがd1のテーブルデータとd2のテーブルデータをd1の1列目とd2の2列目をopenした値で突合させて横結合する
- inp: `Tuple[List[int], List[int]]`
	- inp[0]: 設計変数行列のリスト
	- inp[1]: 目的値の列リスト
### Returns
- res: `Dict`
	- res["is_ok"]: `bool`
		- 送信が成功したかどうか
	- res["job_id"]: `str`
		- 計算スレッドのID

## QMPC.logistic_regression
ロジスティック回帰の学習を行う
### Parameters
- join_order: `Tuple[List[str], List[int], List[int]]`
	- join_order[0]: data_idのリスト
	- join_order[1]: テーブルデータの結合方向(1:横(open)，2:縦(open)，3:横(share))
	- join_order[2]: 結合に用いるIDの列リスト
	- 例えば(["d1", "d2"], [1], [1, 2])であれば，data_idがd1のテーブルデータとd2のテーブルデータをd1の1列目とd2の2列目をopenした値で突合させて横結合する
- inp: `Tuple[List[int], List[int]]`
	- inp[0]: 設計変数行列のリスト
	- inp[1]: 目的値の列リスト
### Returns
- res: `Dict`
	- res["is_ok"]: `bool`
		- 送信が成功したかどうか
	- res["job_id"]: `str`
		- 計算スレッドのID

## QMPC.meshcode
メッシュコードを計算する
### Parameters
- join_order: `Tuple[List[str], List[int], List[int]]`
	- join_order[0]: data_idのリスト
	- join_order[1]: テーブルデータの結合方向(1:横(open)，2:縦(open)，3:横(share))
	- join_order[2]: 結合に用いるIDの列リスト
	- 例えば(["d1", "d2"], [1], [1, 2])であれば，data_idがd1のテーブルデータとd2のテーブルデータをd1の1列目とd2の2列目をopenした値で突合させて横結合する
- src: `List[int]`
	- メッシュコードを計算する列リスト
### Returns
- res: `Dict`
	- res["is_ok"]: `bool`
		- 送信が成功したかどうか
	- res["job_id"]: `str`
		- 計算スレッドのID

## QMPC.decision_tree
決定木の学習を行う
### Parameters
- join_order: `Tuple[List[str], List[int], List[int]]`
	- join_order[0]: data_idのリスト
	- join_order[1]: テーブルデータの結合方向(1:横(open)，2:縦(open)，3:横(share))
	- join_order[2]: 結合に用いるIDの列リスト
	- 例えば(["d1", "d2"], [1], [1, 2])であれば，data_idがd1のテーブルデータとd2のテーブルデータをd1の1列目とd2の2列目をopenした値で突合させて横結合する
- inp: `Tuple[List[int], List[int]]`
	- inp[0]: 設計変数行列のリスト
	- inp[1]: 目的値の列リスト
### Returns
- res: `Dict`
	- res["is_ok"]: `bool`
		- 送信が成功したかどうか
	- res["job_id"]: `str`
		- 計算スレッドのID

## QMPC.get_join_table
テーブルを結合する
### Parameters
- join_order: `Tuple[List[str], List[int], List[int]]`
	- join_order[0]: data_idのリスト
	- join_order[1]: テーブルデータの結合方向(1:横(open)，2:縦(open)，3:横(share))
	- join_order[2]: 結合に用いるIDの列リスト
	- 例えば(["d1", "d2"], [1], [1, 2])であれば，data_idがd1のテーブルデータとd2のテーブルデータをd1の1列目とd2の2列目をopenした値で突合させて横結合する
### Returns
- res: `Dict`
	- res["is_ok"]: `bool`
		- 送信が成功したかどうか
	- res["job_id"]: `str`
		- 計算スレッドのID

## QMPC.get_computation_result
計算結果を取得する
### Parameters
- job_id: `str`
	- 計算スレッドのID
### Returns
- res: `Dict`
	- res["is_ok"]: `bool`
		- 送信が成功したかどうか
	- res["results"]: `List[float]`
		- 計算結果

## QMPC.get_data_list
エンジンに保存されているテーブルデータのIDを全て取得する
### Parameters
### Returns
- res: `Dict`
	- res["is_ok"]: `bool`
		- 送信が成功したかどうか
	- res["results"]: `List[str]`
		- テーブルデータのIDのリスト
