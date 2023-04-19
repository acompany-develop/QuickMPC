import sys
import logging

from quickmpc import QMPC

logger = logging.getLogger(__name__)
logging.basicConfig(level=logging.DEBUG)

if __name__ == '__main__':
    # QuickMPC Setting
    qmpc: QMPC = QMPC(
        ["http://localhost:50001",
         "http://localhost:50002",
         "http://localhost:50003"]
    )

    if not len(sys.argv) == 2:
        print('Usage: $ pipenv run python send_share.py [file_path]',
              file=sys.stderr)
        sys.exit(1)

    # データをシェア化し送信する
    filename: str = sys.argv[1]
    res = qmpc.send_share_from_csv_file(filename)
    logger.info(res)
