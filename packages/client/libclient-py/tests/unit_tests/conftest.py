""" Testの前の前処理を定義
ローカルでhttpsサーバを3台立てており，
yieldのタイミングで全てのTestが開始
"""
import glob
import os

import pytest

from .local_server import serve
from .local_server_v0 import serve_v0


@pytest.fixture(scope='session')
def run_server1():
    """ run server """
    server = serve(1)
    server.start()

    """ test start """
    yield

    """ test end """
    for file in glob.glob("./tests/unit_tests/dim1-uuid*"):
        os.remove(file)

    server.stop(0)


@pytest.fixture(scope='session')
def run_server2():
    """ run server """
    server = serve(2)
    server.start()

    """ test start """
    yield

    """ test end """
    server.stop(0)


@pytest.fixture(scope='session')
def run_server3():
    """ run server """
    server = serve(3)
    server.start()

    """ test start """
    yield

    """ test end """
    server.stop(0)


@pytest.fixture(scope='session')
def run_server4():
    """ run server """
    server = serve_v0(4)
    server.start()

    """ test start """
    yield

    """ test end """
    for file in glob.glob("./tests/unit_tests/dim1-uuid*"):
        os.remove(file)

    server.stop(0)


@pytest.fixture(scope='session')
def run_server5():
    """ run server """
    server = serve_v0(5)
    server.start()

    """ test start """
    yield

    """ test end """
    server.stop(0)


@pytest.fixture(scope='session')
def run_server6():
    """ run server """
    server = serve_v0(6)
    server.start()

    """ test start """
    yield

    """ test end """
    server.stop(0)
