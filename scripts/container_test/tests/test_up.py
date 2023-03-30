import pytest

from container import Containers
from utils import all_containers


@pytest.mark.parametrize(
    ("container"), [
        (Containers(["dev_unit_cc1"])),
        (Containers(["dev_unit_cc2"])),
        (Containers(["dev_unit_cc3"])),
        (Containers(["dev_unit_mc1"])),
        (Containers(["dev_unit_mc2"])),
        (Containers(["dev_unit_mc3"])),
        (Containers(["dev_unit_bts"])),
    ]
)
def test_up(container):
    # コンテナ単体で起動してhealthcheckが通るかtest
    ac = all_containers()
    ac.down()
    container.up()
    ac.down()
