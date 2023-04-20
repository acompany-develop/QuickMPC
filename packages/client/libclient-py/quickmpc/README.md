# QuickMPC-libClient-pyが提供する機能

## QMPC.send_share_from_csv_file
csvファイルからテーブルデータを読み込みShare化して送信する
### Parameters
- file: `str`
	- 読み込むファイル名

### Returns
- res: `Dict`
	- res["is_ok"]: `bool`
		- 送信が成功したかどうか
	- res["data_id"]: `str`
		- テーブルデータのID

## QMPC.send_share_from_csv_data
テーブルデータをShare化して送信する
### Parameters
- data: `List[List[str]]`
	- パースするテーブルデータ

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
	- res["job_uuid"]: `str`
		- 計算スレッドのUUID

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
	- res["job_uuid"]: `str`
		- 計算スレッドのUUID

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
	- res["job_uuid"]: `str`
		- 計算スレッドのUUID

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
	- res["job_uuid"]: `str`
		- 計算スレッドのUUID

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
	- res["job_uuid"]: `str`
		- 計算スレッドのUUID

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
	- res["job_uuid"]: `str`
		- 計算スレッドのUUID

## QMPC.get_computation_result
計算結果を取得する
### Parameters
- job_uuid: `str`
	- 計算スレッドのUUID
### Returns
- res: `Dict`
	- res["is_ok"]: `bool`
		- 送信が成功したかどうか
	- res["results"]: `List[float]`
		- 計算結果

## QMPC.get_data_list
[deplecated]
エンジンに保存されているテーブルデータのIDを全て取得する
### Parameters
### Returns
- res: `Dict`
	- res["is_ok"]: `bool`
		- 送信が成功したかどうか
	- res["results"]: `List[str]`
		- テーブルデータのIDのリスト
