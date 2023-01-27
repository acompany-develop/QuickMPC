from quickmpc import QMPC
from tabulate import tabulate
from typing import List
import sys


def output_table(title: int, schema: List[str], table: List):
    print("party"+str(title))
    print(tabulate(
        table,
        headers=schema,
        tablefmt="grid",
        floatfmt='.11f'))


if __name__ == '__main__':
    # QuickMPC Setting
    qmpc: QMPC = QMPC(
        ["http://localhost:9001",
         "http://localhost:9002",
         "http://localhost:9003"]
    )

    if not len(sys.argv) == 2:
        print('Usage: $ pipenv run python demo_sharize.py [file_path]',
              file=sys.stderr)
        sys.exit(1)
    # csv dataをパースする
    filename: str = sys.argv[1]

    secrets, schema = qmpc.parse_csv_file(filename)
    # データをシェア化する
    res = qmpc.demo_sharize(secrets)
    for party_id, share in enumerate(res['results']):
        output_table(party_id+1, schema, share)
