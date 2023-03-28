import pytest

from container import Container
from utils import init


@pytest.mark.parametrize(
    ("container"), [
        (Container("dev_unit_cc1")),
        (Container("dev_unit_cc2")),
        (Container("dev_unit_cc3")),
        (Container("dev_unit_mc1")),
        (Container("dev_unit_mc2")),
        (Container("dev_unit_mc3")),
        (Container("dev_unit_bts")),
    ]
)
def test_up(container):
    # コンテナ単体で起動してhealthcheckが通るかtest
    init()
    container.up()
    init()
