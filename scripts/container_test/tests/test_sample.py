import time

from container import Container


def test_sample():
    dev_cc = Container("dev_unit_cc1")
    dev_cc.up()
    time.sleep(5)
    dev_cc.down()
