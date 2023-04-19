from typing import List


def large_data(size: int, data_num: int):
    schema_size: int = 10
    schema: List[str] = ["id"] + \
        [f"s{i}_{data_num}" for i in range(schema_size-1)]

    mod = 47
    data: List[List[str]] = [
        [str(i)] + [str(((i*size + k) % mod)/mod)for k in range(schema_size-1)]
        for i in range(size)
    ]
    return [schema]+data
