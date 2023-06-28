import string
from random import Random

import pytest
from utils import get_result, qmpc

import tests.common


def generate_random_string(seed: int, length: int) -> str:
    random = Random(seed)
    chars = [random.choice(string.printable) for _ in range(length)]
    return ''.join(chars)


@pytest.mark.parametrize(
    ("secret"),
    [
        # normal case
        'secret',
        # random case
        generate_random_string(seed=0, length=512),
        # alphabet
        "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ",
        # hiragana
        "ぁあぃいぅうぇえぉおかがきぎくぐけげこごさざしじすずせぜそぞ"
        "ただちぢっつづてでとどなにぬねのはばぱひびぴふぶぷへべぺほぼぽ"
        "まみむめもゃやゅゆょよらりるれろゎわゐゑをんゔゕゖ",
        # katakana
        "ァアィイゥウェエォオカガキギクグケゲコゴサザシジスズセゼソゾ"
        "タダチヂッツヅテデトドナニヌネノハバパヒビピフブプヘベペホボポ"
        "マミムメモャヤュユョヨラリルレロヮワヰヱヲンヴヵヶヷヸヹヺ",
        # kanji
        "春眠不覚暁処処聞啼鳥夜来風雨声花落知多少",
        # large number
        # NOTE: 全角数字も数字と解釈されてしまい文字列として復元されない
        # "１２３４５６７８９０",
        # mix
        "abcあいうアイウ你好世界１２３",
        # 最長住所
        "漢漢漢漢漢漢漢漢漢漢漢漢漢漢漢漢漢漢漢漢漢漢漢漢漢漢漢漢漢漢漢"
        "漢漢漢漢漢漢漢漢漢漢漢漢漢漢漢漢漢漢漢漢漢漢漢漢漢漢漢漢漢漢漢"
        "漢漢漢漢漢漢漢漢漢漢漢漢漢漢漢漢漢漢漢漢漢漢漢漢漢漢漢漢漢漢漢"
        "漢漢漢漢漢漢漢漢漢漢漢漢漢漢漢漢漢漢漢漢漢漢漢漢漢漢漢漢漢漢漢"
        "漢漢漢漢漢漢漢漢漢漢漢漢漢漢漢漢漢漢漢漢漢漢漢漢漢漢漢漢漢漢漢漢漢漢漢漢",
        # emoji
        "😀😁😂🤣😃😄😅😆😉😊😋😎😍😘😗😙😚☺🙂🤗🤩🤔🤨😐😑😶🙄😏😣😥😮"
        "🤐😯😪😫😴😌😛😜😝🤤😒😓😔😕🙃🤑😲☹🙁😖😞😟😤😢😭😦😧😨😩🤯😬"
        "😰😱😳🤪😵😡😠🤬😷🤒🤕🤢🤮🤧😇🤠🤡🤥🤫🤭🧐🤓",
    ]
)
def test_restor_string(secret: str):
    secrets = [['0', secret]]
    schema = ['id', 'str']

    data_id = tests.common.data_id(secrets, schema)

    res = get_result(qmpc.get_join_table([data_id]))
    assert (res["is_ok"])

    assert res['results']['table'] == [[secret]]
