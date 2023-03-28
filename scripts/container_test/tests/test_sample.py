import time

from container import Container


def test_sample():
    bts = Container("dev_unit_bts")
    bts.up()

    cc = Container("dev_unit_cc1")
    cc.up()

    bts.down()
    cc.down()
