import logging

from quickmpc import QMPC

logger = logging.getLogger(__name__)
logging.basicConfig(level=logging.DEBUG)

if __name__ == '__main__':
    # QuickMPC Setting
    qmpc: QMPC = QMPC(
        ["http://localhost:9001",
         "http://localhost:9002",
         "http://localhost:9003"]
    )

    res = qmpc.get_data_list()
    logger.info(res)
