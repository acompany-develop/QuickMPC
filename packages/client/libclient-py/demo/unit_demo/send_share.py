import sys
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

    if not len(sys.argv) == 2:
        print('Usage: $ pipenv run python send_share.py [file_path]',
              file=sys.stderr)
        sys.exit(1)
    # csv dataをパースする
    filename: str = sys.argv[1]

    secrets, schema = qmpc.parse_csv_file(filename)

    # データをシェア化し送信する
    res = qmpc.send_share(secrets, schema)
    logger.info(res)
