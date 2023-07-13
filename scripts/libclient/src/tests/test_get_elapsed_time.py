import pytest
from utils import data_frame, qmpc


@pytest.mark.parametrize(
    ("size"),
    [
        (1), (10), (100), (1000),
    ]
)
def test_sum(size: int):
    df = data_frame([[1, 2, 3] for _ in range(size)],
                    columns=["s1", "s2", "s3"])
    sdf = qmpc.send_to(df)
    sdf_res = sdf.sum([1, 2, 3])
    tm = sdf_res.get_elapsed_time()
    assert tm is not None
