import pandas as pd

from quickmpc.pandas.parser import to_float


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
    df[index_col] = df[index_col].map(lambda x: to_float(x))
    # join時にQMPCのCC側でID列でsortできる様に、座圧を行いindexに設定しておく
    df["original_index"] = df.index
    df = df.sort_values(by=index_col)
    df = df.reset_index(drop=True)
    df = df.sort_values(by="original_index")
    df = df.drop('original_index', axis=1)
    df.set_index(index_col)
    return df
