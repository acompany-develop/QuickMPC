import collections
from typing import List

import numpy as np
import pandas as pd
import seaborn
from sklearn.model_selection import train_test_split
from sklearn.preprocessing import MinMaxScaler
from utils import qmpc


def __my_train_test_split(df, test_size, index_y=[0]):
    train, test = train_test_split(df, test_size=test_size)
    # sklearnの学習が１クラスのみだとエラーが出るため，
    # trainデータの前5個以内に1クラスだけが固まらないように調整する
    for idx in index_y:
        c = collections.Counter(train[:5, idx])
        if len(c) == 1:
            return __my_train_test_split(df, test_size, index_y)
    return train, test


def titanic(is_bitvector=False):
    """ titanicデータを整形して返す """
    df = seaborn.load_dataset("titanic")
    df['sex'] = df['sex'].apply(
        lambda x: 0 if x == 'male' else 1)
    df['embarked'] = df['embarked'].apply(
        lambda x: 0 if x == 'C' else 1 if x == 'Q' else 2)
    df['class'] = df['class'].apply(
        lambda x: 0 if x == 'First' else 1 if x == 'Second' else 2)

    # 数値以外の列を削除
    numerical_col = [col for col in df.columns
                     if df[col].dtype == "int64" or df[col].dtype == "float64"]
    df = df[numerical_col]
    df.fillna(df[df.columns].median(), inplace=True)

    # bitvectorにする場合はさらに離散値のみ取り出す
    if is_bitvector:
        numerical_col = [col for col in df.columns if df[col].dtype == "int64"]
        df = df[numerical_col]
        df_list = df.T.reset_index().T.values.tolist()
        df_list = [[f"id{i}"] + row for i, row in enumerate(df_list)]
        secrets, schema = qmpc.parse_csv_data_to_bitvector(df_list, [0])
        train, test = __my_train_test_split(
            np.array(secrets), test_size=0.25, index_y=[0, 1])
        return (train[:, 3:], train[:, 1:3],
                test[:, 3:], test[:, 1:3],
                schema[3:], schema[1:3])

    # 01に正規化
    mm = MinMaxScaler()
    df_mm = mm.fit_transform(df)
    schema = df.columns.tolist()

    # 75%を学習データ，25%をテストデータとする
    train, test = __my_train_test_split(df_mm, test_size=0.25)
    return (train[:, 1:], train[:, 0],
            test[:, 1:], test[:, 0],
            schema[1:], [schema[0]])


def iris(is_bitvector=False):
    """ irisデータを整形して返す """
    df = seaborn.load_dataset("iris")
    df['species'] = df['species'].apply(
        lambda x: 1 if x == 'setosa' else 2 if x == 'versicolor' else 3)

    # そのままでは全て連続値でbitvectorとして扱いづらいので，
    # 値を範囲ごとに区切って離散値を当てはめる
    if is_bitvector:
        df['sepal_length'] = df['sepal_length'].apply(
            lambda x: 1 if x < 5.0 else 2 if x < 6.0 else 3 if x < 7.0 else 4)
        df['sepal_width'] = df['sepal_width'].apply(
            lambda x: 1 if x < 3.0 else 2 if x < 4.0 else 3)
        df['petal_length'] = df['petal_length'].apply(
            lambda x: 1 if x < 2.0 else 2 if x < 3.0 else 3 if x < 4.0
            else 4 if x < 5.0 else 5 if x < 6.0 else 6)
        df['petal_width'] = df['petal_width'].apply(
            lambda x: 1 if x < 1.0 else 2 if x < 2.0 else 3)
        df_list = df.T.reset_index().T.values.tolist()
        df_list = [[f"id{i}"] + row for i, row in enumerate(df_list)]
        secrets, schema = qmpc.parse_csv_data_to_bitvector(df_list, [0])
        train, test = __my_train_test_split(
            np.array(secrets), test_size=0.25, index_y=[0, 1])
        return (train[:, 1:16], train[:, 16:],
                test[:, 1:16], test[:, 16:],
                schema[1:16], schema[16:])

    df = pd.get_dummies(df, columns=["species"])
    # 01に正規化
    mm = MinMaxScaler()
    df_mm = mm.fit_transform(df)
    schema = df.columns.tolist()

    train, test = __my_train_test_split(
        df_mm, test_size=0.25, index_y=[4, 5, 6])
    return (train[:, :4], train[:, 4:],
            test[:, :4], test[:, 4:],
            schema[:4], schema[4:])


def penguins(is_bitvector=False):
    """ penguinsデータを整形して返す """
    df = seaborn.load_dataset("penguins")
    df['sex'] = df['sex'].apply(lambda x: 0 if x == 'Male' else 1)
    df['species'] = df['species'].apply(
        lambda x: 1 if x == 'Adelie' else 2 if x == 'Chinstrap' else 3)
    df['island'] = df['island'].apply(
        lambda x: 1 if x == 'Torgersen' else 2 if x == 'Biscoe' else 3)

    df.fillna(df[df.columns].median(), inplace=True)

    # そのままではbitvectorとして扱いづらいので
    # 値を範囲ごとに区切って離散値を当てはめる
    if is_bitvector:
        df['bill_length_mm'] = df['bill_length_mm'].apply(
            lambda x: 1 if x < 40.0 else 2 if x < 50.0 else 3)
        df['bill_depth_mm'] = df['bill_depth_mm'].apply(
            lambda x: 1 if x < 15.0 else 2 if x < 20.0 else 3)
        df['flipper_length_mm'] = df['flipper_length_mm'].apply(
            lambda x: 1 if x < 190.0 else 2 if x < 200.0 else 3)
        df['body_mass_g'] = df['body_mass_g'].apply(
            lambda x: 1 if x < 4000.0 else 2 if x < 5000.0 else 3)
        df_list = df.T.reset_index().T.values.tolist()
        df_list = [[f"id{i}"] + row for i, row in enumerate(df_list)]
        secrets, schema = qmpc.parse_csv_data_to_bitvector(df_list, [0])
        train, test = __my_train_test_split(
            np.array(secrets), test_size=0.25, index_y=[0, 1])
        return (train[:, 4:], train[:, 1:4],
                test[:, 4:], test[:, 1:4],
                schema[4:], schema[1:4])

    df = pd.get_dummies(df, columns=["island", "species"])
    # 01に正規化
    mm = MinMaxScaler()
    df_mm = mm.fit_transform(df)
    schema = df.columns.tolist()

    train, test = __my_train_test_split(
        df_mm, test_size=0.25, index_y=[6, 7, 8, 9, 10])
    return (train[:, :8], train[:, 8:],
            test[:, :8], test[:, 8:],
            schema[:8], schema[8:])


def large_data(size: int, data_num: int):
    schema_size: int = 10
    schema: List[str] = ["id"] + \
        [f"s{i}_{data_num}" for i in range(schema_size-1)]

    mod = 47
    data: List[List[str]] = [
        [str(((i*size + k) % mod)/mod)for k in range(schema_size)]
        for i in range(size)
    ]
    return qmpc.parse_csv_data([schema]+data)
