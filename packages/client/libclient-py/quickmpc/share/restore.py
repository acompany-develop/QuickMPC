import csv
import glob
from typing import Any

import google.protobuf.json_format
import numpy as np
from natsort import natsorted

from quickmpc.proto.common_types.common_types_pb2 import Schema
from quickmpc.share.share import Share
from quickmpc.utils import if_present


def get_meta(job_uuid: str, path: str) -> int:
    """結果データからmeta情報を取り出す

    Parameters
    ----------
    job_uuid: str
        計算結果のID
    path: str
        計算結果を保存したpath

    Returns
    -------
    int
        metaデータ
    """
    file_name = glob.glob(f"{path}/dim?-{job_uuid}-*")[0]
    with open(file_name, 'r') as f:
        reader = csv.reader(f)
        # 1列目のデータを取得
        meta = next(reader)
        return int(meta[0])


def get_result(job_uuid: str, path: str, party: int):
    """結果データからmeta情報を取り出す

    Parameters
    ----------
    job_uuid: str
        計算結果のID
    path: str
        計算結果を保存したpath

    Yields
    ------
    List[str]
        テーブルデータの行
    """
    for file_name in natsorted(glob.glob(f"{path}-{job_uuid}-{party}-*")):
        with open(file_name, 'r') as f:
            reader = csv.reader(f)
            # 1列目は読まない
            next(reader)
            for row in reader:
                for val in row:
                    yield val


def restore(job_uuid: str, path: str, party_size: int) -> Any:
    """ファイルに保存された結果データを復元する

    Parameters
    ----------
    job_uuid: str
        計算結果のID
    path: str
        計算結果を保存したpath
    party_size: int
        MPCのパーティ数

    Returns
    -------
    Any
        復元した計算結果
    """
    column_number = get_meta(job_uuid, path)

    schema: Any = [None]*column_number
    results: Any = []

    is_schema = True if len(
        glob.glob(f"{path}/schema-{job_uuid}-*")) != 0 else False
    is_dim2 = True if len(
        glob.glob(f"{path}/dim2-{job_uuid}-*")) != 0 else False
    if column_number == 0:
        if is_schema:
            return {"schema": [], "table": [[]]}
        elif is_dim2:
            return [[]]
        else:
            return []

    for party in range(party_size):
        if party == 0:
            for i, val in enumerate(get_result(
                                    job_uuid, f"{path}/schema", party)):
                col_sch = google.protobuf.json_format.Parse(
                    val, Schema())
                schema[i] = col_sch

        itr = 0
        for val in get_result(job_uuid, f"{path}/dim?", party):
            f = Share.get_pre_convert_func(schema[itr % column_number])
            if itr >= len(results):
                results.append(f(val))
            else:
                results[itr] += f(val)
            itr += 1
    results = np.array(results)\
        .reshape(-1, column_number).tolist() if is_dim2 else results
    if is_dim2 and len(results) == 0:
        schema = []
        results = [[]]

    results = if_present(results, Share.convert_type, schema)
    results = {"schema": schema, "table": results} if is_schema else results
    return results
