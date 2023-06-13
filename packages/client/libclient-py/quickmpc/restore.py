import csv
import glob
from typing import Any

import numpy as np
from natsort import natsorted

import google.protobuf.json_format
from .proto.common_types.common_types_pb2 import Schema
from .share import Share
from .utils.if_present import if_present


def get_meta(job_uuid: str, path: str):
    file_name = glob.glob(f"{path}/dim?-{job_uuid}-*")[0]
    with open(file_name, 'r') as f:
        reader = csv.reader(f)
        # 1列目のデータを取得
        meta = next(reader)
        return int(meta[0])


def get_result(job_uuid: str, path: str, party: int):
    for file_name in natsorted(glob.glob(f"{path}-{job_uuid}-{party}-*")):
        with open(file_name, 'r') as f:
            reader = csv.reader(f)
            # 1列目は読まない
            next(reader)
            for row in reader:
                for val in row:
                    yield val


def restore(job_uuid: str, path: str, party_size: int) -> Any:
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
