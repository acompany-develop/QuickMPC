import os

import pytest
from utils import qmpc

from benchmark.metrics import Timer, print_elapsed_time
import quickmpc.pandas as qpd

# types: (iterate_num, path)
# testに使うファイルを指定
is_ci_benchmark = os.getenv("IS_CI_BENCHMARK")
if is_ci_benchmark:
    # CIでbenchmarkを実行する場合
    test_parameters = [
        (1, "data/data_10m.csv")
    ]
else:
    # localで実行する場合にはここにデータを追加する
    test_parameters = [
        (3, "data/table_data_10x10.csv"),
        (3, "data/table_data_100x10.csv"),
    ]


@pytest.fixture(scope="module")
def get_sdf(print_elapsed_time):
    sdf_dict: dict = {}

    def get(path: str):
        if path in sdf_dict:
            return sdf_dict[path]
        df = qpd.read_csv(path, index_col="id")
        with Timer() as tm:
            sdf = qmpc.send_to(df)
        print_elapsed_time(tm.elapsed_time(), "send_share", path)
        # NOTE: index用の列が追加されているためschema_sizeは-1しておく
        sdf_dict[path] = (sdf, len(df.columns) - 1)
        return sdf_dict[path]
    return lambda path: get(path)


@pytest.mark.parametrize(
    ("iterate_num", "path"), test_parameters
)
def test_mean(iterate_num: int, path: int,
              get_sdf, print_elapsed_time):
    for _ in range(iterate_num):
        sdf, _ = get_sdf(path)
        print_elapsed_time(sdf.mean([2]).get_elapsed_time(),
                           "mean", path)


@pytest.mark.parametrize(
    ("iterate_num", "path"), test_parameters
)
def test_sum(iterate_num: int, path: int,
             get_sdf, print_elapsed_time):
    for _ in range(iterate_num):
        sdf, _ = get_sdf(path)
        print_elapsed_time(sdf.sum([2]).get_elapsed_time(),
                           "sum", path)


@pytest.mark.parametrize(
    ("iterate_num", "path"), test_parameters
)
def test_variance(iterate_num: int, path: int,
                  get_sdf, print_elapsed_time):
    for _ in range(iterate_num):
        sdf, _ = get_sdf(path)
        print_elapsed_time(sdf.variance([2]).get_elapsed_time(),
                           "variance", path)


@pytest.mark.parametrize(
    ("iterate_num", "path"), test_parameters
)
def test_correl(iterate_num: int, path: int,
                get_sdf, print_elapsed_time):
    for _ in range(iterate_num):
        sdf, schema_size = get_sdf(path)
        print_elapsed_time(sdf.correl([2], [schema_size]).get_elapsed_time(),
                           "correl", path)


@pytest.mark.parametrize(
    ("iterate_num", "path"), test_parameters
)
def test_hjoin(iterate_num: int, path: int,
               get_sdf, print_elapsed_time):
    for _ in range(iterate_num):
        sdf, schema_size = get_sdf(path)
        print_elapsed_time(sdf.join(sdf).get_elapsed_time(),
                           "hjoin", path)
