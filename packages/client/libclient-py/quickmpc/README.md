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
- data_ids: `List[str]`
	- data_idのリスト
- src: `List[int]`
	- 平均値を計算する列リスト
- debug_mode: `bool`
  - keyword引数．`True`の場合はdebug用の違法高速マッチングを行う
### Returns
- res: `Dict`
	- res["is_ok"]: `bool`
		- 送信が成功したかどうか
	- res["job_uuid"]: `str`
		- 計算スレッドのUUID

## QMPC.variance
分散を計算する
### Parameters
- data_ids: `List[str]`
	- data_idのリスト
- src: `List[int]`
	- 分散を計算する列リスト
- debug_mode: `bool`
  - keyword引数．`True`の場合はdebug用の違法高速マッチングを行う
### Returns
- res: `Dict`
	- res["is_ok"]: `bool`
		- 送信が成功したかどうか
	- res["job_uuid"]: `str`
		- 計算スレッドのUUID

## QMPC.sum
総和を計算する
### Parameters
- data_ids: `List[str]`
	- data_idのリスト
- src: `List[int]`
	- 総和を計算する列リスト
- debug_mode: `bool`
  - keyword引数．`True`の場合はdebug用の違法高速マッチングを行う
### Returns
- res: `Dict`
	- res["is_ok"]: `bool`
		- 送信が成功したかどうか
	- res["job_uuid"]: `str`
		- 計算スレッドのUUID

## QMPC.correl
相関係数を計算する
### Parameters
- data_ids: `List[str]`
	- data_idのリスト
- inp: `Tuple[List[int], List[int]]`
	- inp[0]: 相関係数の左列リスト
	- inp[1]: 相関係数の右列リスト
- debug_mode: `bool`
  - keyword引数．`True`の場合はdebug用の違法高速マッチングを行う
### Returns
- res: `Dict`
	- res["is_ok"]: `bool`
		- 送信が成功したかどうか
	- res["job_uuid"]: `str`
		- 計算スレッドのUUID

## QMPC.meshcode
メッシュコードを計算する
### Parameters
- data_ids: `List[str]`
	- data_idのリスト
- src: `List[int]`
	- メッシュコードを計算する列リスト
- debug_mode: `bool`
  - keyword引数．`True`の場合はdebug用の違法高速マッチングを行う
### Returns
- res: `Dict`
	- res["is_ok"]: `bool`
		- 送信が成功したかどうか
	- res["job_uuid"]: `str`
		- 計算スレッドのUUID

## QMPC.get_join_table
テーブルを結合する
### Parameters
- data_ids: `List[str]`
	- data_idのリスト
- debug_mode: `bool`
  - keyword引数．`True`の場合はdebug用の違法高速マッチングを行う
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
