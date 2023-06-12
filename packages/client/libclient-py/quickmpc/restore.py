import csv
import glob
from decimal import Decimal
from typing import Any

import numpy as np
from natsort import natsorted


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
    schema = []
    result: Any = []

    column_number = get_meta(job_uuid, path)
    is_dim2 = True if len(
        glob.glob(f"{path}/dim2-{job_uuid}-*")) != 0 else False
    if column_number == 0:
        return [[]] if is_dim2 else []

    for party in range(party_size):
        if party == 0:
            for val in get_result(job_uuid, f"{path}/schema", party):
                schema.append(val)

        itr = 0
        for val in get_result(job_uuid, f"{path}/dim?", party):
            if itr >= len(result):
                result.append(Decimal(val))
            else:
                result[itr] += Decimal(val)
            itr += 1

    result_float = np.vectorize(float)(result)
    result = np.array(result_float)\
        .reshape(-1, column_number).tolist() if is_dim2 else result_float
    result = {"schema": schema, "table": result} if len(schema) else result
    return result
