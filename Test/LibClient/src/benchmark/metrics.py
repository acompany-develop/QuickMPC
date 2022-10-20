import dataclasses
import statistics
import time
from typing import Dict

import numpy as np
from sklearn.metrics import (accuracy_score, confusion_matrix, f1_score,
                             mean_absolute_error, mean_squared_error,
                             precision_score, r2_score, recall_score)
from tabulate import tabulate


@dataclasses.dataclass
class Metrics():
    __value_dict: Dict[str, list] = dataclasses.field(
        default_factory=dict, init=False)

    def add(self, key: str, val: float) -> None:
        if key not in self.__value_dict:
            self.__value_dict[key] = []
        self.__value_dict[key].append(val)

    def get_size(self) -> int:
        sample: list = [*self.__value_dict.values()]
        return len(sample[0]) if len(sample) > 0 else 0

    def get_metrics_names(self) -> list:
        return [key for key in self.__value_dict.keys()]

    def get_min_list(self) -> list:
        return [min(lst) for lst in self.__value_dict.values()]

    def get_max_list(self) -> list:
        return [max(lst) for lst in self.__value_dict.values()]

    def get_mean_list(self) -> list:
        return [statistics.mean(lst) for lst in self.__value_dict.values()]

    def get_median_list(self) -> list:
        return [statistics.median(lst) for lst in self.__value_dict.values()]


@dataclasses.dataclass
class MetricsOutputer():

    genre: str
    __metrics_dict: Dict[str, Metrics] = dataclasses.field(
        default_factory=dict, init=False)

    def __post_init__(self):
        genre_list: list = ["classification", "regression"]
        if self.genre not in genre_list:
            raise RuntimeError(
                "出力形式として'classification'か'regression'のいずれかを選択してください．")

    def append(self, name: str, test_y: list, pred_y: list) -> None:
        if name not in self.__metrics_dict:
            self.__metrics_dict[name] = Metrics()
        if self.genre == "classification":
            self.__metrics_dict[name].add(
                "accuracy", accuracy_score(test_y, pred_y))
            self.__metrics_dict[name].add("precision", precision_score(
                test_y, pred_y, average='micro'))
            self.__metrics_dict[name].add("recall", recall_score(
                test_y, pred_y, average='micro'))
            self.__metrics_dict[name].add("f1 score", f1_score(
                test_y, pred_y, average='micro'))
        elif self.genre == "regression":
            self.__metrics_dict[name].add(
                "MAE",  mean_absolute_error(test_y, pred_y))
            self.__metrics_dict[name].add(
                "MSE", mean_squared_error(test_y, pred_y))
            self.__metrics_dict[name].add("RMSE", np.sqrt(
                mean_squared_error(test_y, pred_y)))
            self.__metrics_dict[name].add("R2", r2_score(test_y, pred_y))

    def output(self):
        sample: list = [*self.__metrics_dict.values()]
        metrics_sample: list = sample[0] if len(sample) > 0 else Metrics()
        size: int = metrics_sample.get_size()
        headers: list = metrics_sample.get_metrics_names()
        min_table: list = []
        max_table: list = []
        mean_table: list = []
        median_table: list = []
        print()
        print(f"{size} iterations of benchmark results")
        for name, metrics in self.__metrics_dict.items():
            min_table.append([name]+metrics.get_min_list())
            max_table.append([name]+metrics.get_max_list())
            mean_table.append([name]+metrics.get_mean_list())
            median_table.append([name]+metrics.get_median_list())
        print(tabulate(min_table, headers=["[min]"]+headers, tablefmt="grid"))
        print(tabulate(max_table, headers=["[max]"]+headers, tablefmt="grid"))
        print(tabulate(mean_table, headers=[
              "[mean]"]+headers, tablefmt="grid"))
        print(tabulate(median_table, headers=[
              "[median]"]+headers, tablefmt="grid"))


@dataclasses.dataclass
class PrintTime:

    __name: str
    __start: float = dataclasses.field(init=False)

    def __enter__(self):
        self.__start = time.time()
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        elapsed_time = time.time() - self.__start
        print(f"{self.__name}: {elapsed_time} ms")
