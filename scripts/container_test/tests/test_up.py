import pytest

from container import Containers


@pytest.mark.parametrize(
    ("container"), [
        (Containers(["dev_cc1"])),
        (Containers(["dev_cc2"])),
        (Containers(["dev_cc3"])),
        (Containers(["dev_mc1"])),
        (Containers(["dev_mc2"])),
        (Containers(["dev_mc3"])),
        (Containers(["dev_bts"])),
    ]
)
def test_up(container):
    # コンテナ単体で起動してhealthcheckが通るかtest
    Containers.down_all()
    container.up()
    Containers.down_all()
