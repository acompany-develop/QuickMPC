from typing import Any, Dict

import pytest

from quickmpc.request.qmpc_request import QMPCRequest

local_ip_list = [
    "http://localhost:50001",
    "http://localhost:50002",
    "http://localhost:50003"
]


class TestQMPCRequest:
    qmpc_request = QMPCRequest(local_ip_list)
