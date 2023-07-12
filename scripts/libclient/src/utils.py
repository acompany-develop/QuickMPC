import os
from typing import List

import pandas as pd
from quickmpc import QMPC


def get_endpoints():
    party_size = os.getenv("PARTY_SIZE")
    if party_size:
        try:
            endpoints = [f"http://{os.environ[f'PARTY{i}']}:50000"
                         for i in range(1, int(party_size)+1)]
            return endpoints
        except KeyError:
            raise
    else:
        return [
            "http://localhost:50001",
            "http://localhost:50002",
            "http://localhost:50003",
        ]


qmpc: QMPC = QMPC(
    get_endpoints()
)


def data_frame(values: List[List], columns=None):
    df = pd.DataFrame(values, columns=columns)
    df["__qmpc_sort_index__"] = df.index
    return df
