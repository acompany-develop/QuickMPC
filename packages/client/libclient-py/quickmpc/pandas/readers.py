import pandas as pd

from quickmpc.pandas.parser import to_float_for_matching


def read_csv(*args, index_col: str, **kwargs) -> pd.DataFrame:
    """csvからテーブルデータを読み込む．

    テーブル結合処理に用いる列がどの列かを`index_col`で指定する必要がある．
    `index_col`以外の引数は全てpandasのread_csvと同じ．

    Parameters
    ----------
    filepath_or_buffer: FilePath | ReadCsvBuffer[bytes] | ReadCsvBuffer[str],
        らしい
    index_col: str
        ID列としたいカラム名

    Returns
    ----------
    pd.DataFrame
        読み込んだテーブルデータ
    """
    df = pd.read_csv(*args, **kwargs)
    # ID列を数値化
    df[index_col] = df[index_col].map(lambda x: to_float_for_matching(x))
    # send_share時にID列でsortできる様にID列を座標圧縮した列を追加する
    df["__qmpc_sort_index__"] = df.index
    df = df.sort_values(by=index_col)
    df = df.reset_index(drop=True)
    df = df.sort_values(by="__qmpc_sort_index__")
    df["__qmpc_sort_index__"] = df.index
    df = df.reset_index(drop=True)
    return df
