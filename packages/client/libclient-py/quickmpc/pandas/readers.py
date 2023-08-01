import pandas as pd
from pandas.core.shared_docs import _shared_docs
from pandas.io.parsers.readers import _doc_read_csv_and_table
from pandas.util._decorators import Appender

from quickmpc.pandas.parser import to_float


# pandas.read_csvからdocumentを持ってくる
@Appender(
    _doc_read_csv_and_table.format(
        func_name="read_csv",
        summary="Read a comma-separated values (csv) file into DataFrame.",
        _default_sep="','",
        storage_options=_shared_docs["storage_options"],
        decompression_options="",  # TODO: 取得できなかったので原因を特定する
        # decompression_options=_shared_docs["decompression_options"]
        # % "filepath_or_buffer",
    )
)
def read_csv(*args, index_col: str, **kwargs) -> pd.DataFrame:
    """csvからテーブルデータを読み込む

    テーブル結合処理に用いる列がどの列かを `index_col` で列名を指定する必要がある．
    `index_col` 以外の引数は全てpandasのread_csvと同じ．
    以下のdocumentは `pandasのdocument <https://pandas.pydata.org/docs/reference/api/pandas.read_csv.html?highlight=read_csv>`_ より．

    .. docにはpandasからコピーしたdocumentが表示される
    """  # noqa: E501
    df = pd.read_csv(*args, **kwargs)
    # ID列を数値化
    df[index_col] = df[index_col].map(lambda x: to_float(x))
    # send_share時にID列でsortできる様にID列を座標圧縮した列を追加する
    df["__qmpc_sort_index__"] = df.index
    df = df.sort_values(by=index_col)
    df = df.reset_index(drop=True)
    df = df.sort_values(by="__qmpc_sort_index__")
    df["__qmpc_sort_index__"] = df.index
    df = df.reset_index(drop=True)
    return df
