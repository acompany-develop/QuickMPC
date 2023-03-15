import math
import os
from typing import List

import numpy as np
import pytest

from quickmpc import Schema, ShareValueTypeEnum
from quickmpc.utils.parse_csv import (parse, parse_csv, parse_csv_to_bitvector,
                                      parse_to_bitvector)


def schema_fp(name: str):
    return Schema(name=name,
                  type=ShareValueTypeEnum.SHARE_VALUE_TYPE_FIXED_POINT)


def schema_int(name: str):
    return Schema(name=name,
                  type=ShareValueTypeEnum
                  .SHARE_VALUE_TYPE_UTF_8_INTEGER_REPRESENTATION)


# 元データ
normal_data: List[List[str]] = [s.split(",") for s in [
    "id,attr1,attr2,attr3,attr4,attr5,attr6",
    "hoge,0,0.77,0.63,0.35,0.39,0.35",
    "huga,0,0.37,0.36,0.43,0.41,0.39",
    "piyo,1,0.34,0.34,0.44,0.50,0.32",
    "moge,1,0.47,0.43,0.34,0.29,0.34",
    "moga,0,0.67,0.41,0.25,0.49,0.25",
]]
bitvector_data: List[List[str]] = [s.split(",") for s in [
    "id,attr1,attr2,attr3",
    "hoge,0,1,3",
    "huga,0,2,1",
    "moge,1,0,4",
    "moga,0,1,2",
]]
data3: List[List[str]] = [s.split(",") for s in [
    "id,id:id",
    "hoge,hoge",
    "huga,huga",
    "moge,moge",
    "moga,moga",
]]

# 正しくparseされたデータ
d1_schema_str: List[str] = ['id', 'attr1', 'attr2',
                            'attr3', 'attr4', 'attr5', 'attr6']
d1_schema: List[Schema] = [schema_fp(name) for name in d1_schema_str]
d1_secrets: List[List[float]] = [
    [230379555.4797964, 0, 0.77, 0.63, 0.35, 0.39, 0.35],
    [10723675.973257065, 0, 0.37, 0.36, 0.43, 0.41, 0.39],
    [117576607.23670769, 1, 0.34, 0.34, 0.44, 0.5, 0.32],
    [211114761.8482437, 1, 0.47, 0.43, 0.34, 0.29, 0.34],
    [13292676.303739548, 0, 0.67, 0.41, 0.25, 0.49, 0.25]
]

d2_schema_str: List[str] = ['id#0', 'attr1#0', 'attr1#1',
                            'attr2#0', 'attr2#1', 'attr2#2',
                            'attr3#0', 'attr3#1', 'attr3#2', 'attr3#3']
d2_schema: List[Schema] = [schema_fp(name) for name in d2_schema_str]
d2_secrets: List[List[float]] = [
    [230379555.4797964, 1, 0, 1, 0, 0, 1, 0, 0, 0],
    [10723675.973257065, 1, 0, 0, 1, 0, 0, 1, 0, 0],
    [211114761.8482437, 0, 1, 0, 0, 1, 0, 0, 1, 0],
    [13292676.303739548, 1, 0, 1, 0, 0, 0, 0, 0, 1]
]

d3_schema: List[Schema] = [schema_int('id'), schema_fp('id:id')]
d3_secrets: List[List[float]] = [
    [1752131429, 230379555.4797964],
    [1752524641, 10723675.973257065],
    [1836017509, 211114761.8482437],
    [1836017505, 13292676.303739548]
]


def test_parse():
    """ 正しくパースできるかTest """
    secrets, schema = parse(normal_data, matching_column=1)
    assert (np.allclose(secrets, d1_secrets))
    assert (schema == d1_schema)


def test_parse_to_bitvector():
    """ 正しくパースできるかTest """
    secrets, schema = parse_to_bitvector(
        bitvector_data, [0], matching_column=1)
    assert (np.allclose(secrets, d2_secrets))
    assert (schema == d2_schema)


def test_parse_str():
    secrets, schema = parse(data3)
    assert (np.allclose(secrets, d3_secrets))
    assert (schema == d3_schema)


def test_parse_errorhandring():
    """ 異常値を与えてエラーが出るかTest """
    with pytest.raises(Exception):
        # 行が足りずシェアがない
        parse([["id", "a", "b", "c"]])
    with pytest.raises(Exception):
        # schemaに同じものが含まれる
        parse([["id", "a", "a"],
               ["id1", "1", "2"],
               ["id2", "3", "4"]])
    with pytest.raises(Exception):
        # 正方行列でない
        parse([["id", "a", "b"],
               ["id1", "1", "2"],
               ["id2", "3", "4", "5"]])
        parse([["id", "a", "b"],
               ["id1", "1", "2"],
               ["id2"]])


@pytest.mark.parametrize(
    ("csv_file", "expected_secrets", "expected_schema"),
    [
        # 動作確認
        ("normal.csv", d1_secrets, d1_schema),

        # エッジケース
        ("edge_data.csv",
         [[0.0, 0.0, 10000000000.0, -10000000000.0, 1e-11, -1e-11,
           int("64397529887256390100288588569511327578050402500065"
               "00964400196167125347081798938571747784908624301827"
               "77201819885628926259701100123336814710398377257456"
               "07187030800060309020333162876456608685909824737553"
               "03756694212026153794090246875821116372080175855683"
               "82720125176289892945495133027390555430570362645668"
               "86569765390160600458927845218460215249518930528396"
               "77565645389555503267034104711780236545705321812598"
               "73733217009527267504458268967025153180079154528337"
               "51068319637097352589402147037401510643116481468848"
               "596766486584746707218145923849790876466338")]],
         [schema_fp('id'), schema_fp('zero'), schema_fp('int_max'),
          schema_fp('int_min'), schema_fp('float_min_plus'),
          schema_fp('float_min_minus'), schema_int('string_max'), ]),

        # 文字列
        ("string_data.csv",
         [[0.0,
           int("64376492020959182960102910068921920137578270955447"
               "00994229324997296606723909755334743502970718979797"
               "0623272423014052591589722"),
           int("18821911475279072699387439378566387956111513430849"
               "98741952969572035357165535685798000133453408036689"
               "60160103388093358070845619527428351083947816319970"
               "68197752686532392007825472475930054751868291136212"
               "77019627540944278401988123354850270368777761874656"
               "45316362109187736348507908126831678265681238598501"
               "23577310226349872232686643901854391335408463304253"
               "01682911916053212936940769061936237574578117604085"
               "88352253853016582783122458127379747103590307703326"
               "57151190966625605364130654678784763647229799193680"
               "09523441872290151221507683287637256104630532783466"
               "58111173391924857763294806692219085593993637534359"
               "1571891468414377755286"),
           int("14912542658855752609410365103382549324896634886740"
               "87676470590141269487934510022829817548299216842792"
               "98438903311001642333143441433438704924300538949095"
               "32942191147182049154933726045184579039893027681132"
               "56355932385594038940420768768484463399674315383985"
               "47301138620521332048182374493629643773074940611526"
               "75879512118388895164305703580504060293022378036614"
               "66495841636938406245380426862623189171317152868783"
               "70613211773451673474674673254532557304972825179677"
               "97812813079538349280461257033910282465046779757529"
               "81254609824400360070011263743009764449856399529757"
               "60824439920591075847525994360947110332554548784783"
               "796109833618687851418330161690984711082784722224058"),
           int("28119672167872720019552145190445826596296897272947"
               "03346740607363576652787689147056884078078276981761"
               "405396389018008701107809927286173590891114641"),
           1234567890]],
         [
             schema_fp('id'), schema_int('alphabet'), schema_int('hiragana'),
             schema_int('katakana'), schema_int('chinese_characters'),
             schema_fp('large_number'),  # TODO: 文字列として解釈してほしい
         ]),
    ]
)
def test_parse_csv(csv_file, expected_secrets, expected_schema):
    """ csvを正しくパースできるかTest """
    secrets, schema = parse_csv(
        f"{os.path.dirname(__file__)}/test_files/{csv_file}",
        matching_column=1)
    for row, row_expected in zip(secrets, expected_secrets):
        for x, y in zip(row, row_expected):
            if type(x) == int:
                x == y
            else:
                math.isclose(x, y)
    assert (schema == expected_schema)


def test_parse_csv_to_bitvector():
    """ csvを正しくパースできるかTest """
    secrets, schema = parse_csv_to_bitvector(
        f"{os.path.dirname(__file__)}/test_files/bitvector.csv",
        [0], matching_column=1)
    assert (np.allclose(secrets, d2_secrets))
    assert (schema == d2_schema)


@pytest.mark.parametrize(
    ("csv_file", "expected_exception"),
    [
        # ファイルが存在しない
        ("hoge", Exception),

        # 列数が異なる
        ("diff_col.csv", Exception),

        # テーブルが空
        ("empty.csv", Exception),

        # 空のデータが存在する
        ("none.csv", Exception),

        # csv形式じゃない
        ("not_csv.csv", Exception),
    ]
)
def test_parse_csv_errorhandring(csv_file, expected_exception):
    """ 異常値を与えてエラーが出るかTest """
    with pytest.raises(expected_exception):
        parse_csv(f"{os.path.dirname(__file__)}/test_files/{csv_file}")
