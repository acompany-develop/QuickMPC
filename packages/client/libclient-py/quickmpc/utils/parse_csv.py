import csv
import logging
from hashlib import sha512
from typing import List, Tuple

import numpy as np

logger = logging.getLogger(__name__)


def format_check(secrets: List[List[float]],
                 schema: List[str]) -> bool:
    # 存在チェック
    if not (schema and secrets):
        logger.error("Schema or secrets table are not exists.")
        return False
    # 重複チェック
    if len(schema) != len(set(schema)):
        logger.error("Duplicate schema name.")
        return False
    # サイズチェック
    if np.any([len(s) != len(schema) for s in secrets]):
        logger.error("schema size and table colummn size are different.")
        return False
    return True


def to_float(val: str) -> float:
    """ If val is a float, convert as is; if it is a string, hash it. """
    try:
        return float(val)
    except ValueError:
        # k,m are constants used in the comparison operation
        # Due to the limitation of comparison operation,
        # k bits are taken out and divided by 2^m.
        k: int = 48
        m: int = 20
        hs: str = sha512(val.encode()).hexdigest()
        val_int: int = int(hs[:(k >> 2)], 16)
        val_float: float = val_int / pow(2, m)
        return val_float


def parse(data: List[List[str]]) \
        -> Tuple[List[List[float]], List[str]]:
    schema: List[str] = data[0]
    secrets: List[List[float]] = [
        [to_float(x) for x in row] for row in data[1:]]

    if not format_check(secrets, schema):
        raise RuntimeError("規定されたフォーマットでないデータです．")

    return secrets, schema


def parse_to_bitvector(data: List[List[str]], exclude: List[int] = []) \
        -> Tuple[List[List[float]], List[str]]:
    secrets, schema = parse(data)

    secrets_bitbevtor: List[List[float]] = []
    schema_bitvector: List[str] = []
    for col, (sec, sch) in enumerate(zip(np.transpose(secrets), schema)):
        # 列が除外リストに含まれていたらそのままappend
        if col in exclude:
            secrets_bitbevtor.append(sec)
            schema_bitvector.append(sch+"#0")
            continue

        # 座標圧縮
        position: dict = {}
        it: int = 0
        for key in sec:
            if key not in position:
                position[key] = it
                it += 1

        # bitvector化
        for key, val in position.items():
            bitvector: list = [1 if (key == k) else 0 for k in sec]
            sch_val: str = sch+"#"+str(val)
            secrets_bitbevtor.append(bitvector)
            schema_bitvector.append(sch_val)

    return np.transpose(secrets_bitbevtor).tolist(), schema_bitvector


def parse_csv(filename: str) -> Tuple[List[List[float]], List[str]]:
    with open(filename) as f:
        reader = csv.reader(f)
        text: List[List[str]] = [row for row in reader]
        return parse(text)


def parse_csv_to_bitvector(filename: str, exclude: List[int] = []) ->  \
        Tuple[List[List[float]], List[str]]:
    with open(filename) as f:
        reader = csv.reader(f)
        text: List[List[str]] = [row for row in reader]
        return parse_to_bitvector(text, exclude)
