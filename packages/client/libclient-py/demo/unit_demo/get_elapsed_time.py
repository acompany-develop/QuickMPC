
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

    # job_uuidを指定して計算結果を取得する
    job_uuid: str = "10b6bfda-8776-4d8c-ad9a-b40fa277ff37"
    res = qmpc.get_elapsed_time(job_uuid)
    elapsed_time = res["elapsed_time"]
    logger.info(elapsed_time)
