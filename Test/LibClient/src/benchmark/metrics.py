import dataclasses

import numpy as np
from sklearn.metrics import (accuracy_score, confusion_matrix, f1_score,
                             mean_absolute_error, mean_squared_error,
                             precision_score, r2_score, recall_score)
from tabulate import tabulate


@dataclasses.dataclass
class MetricsOutputer():

    genre: str
    headers: list = dataclasses.field(default_factory=list, init=False)
    table: list = dataclasses.field(default_factory=list, init=False)

    def __post_init__(self):
        if self.genre == "classification":
            self.headers = ["", "accuracy", "precision", "recall", "f1 score"]
        elif self.genre == "regression":
            self.headers = ["", "MAE", "MSE", "RMSE", "R2"]
        else:
            raise RuntimeError(
                "出力形式として'classification'か'regression'のいずれかを選択してください．")

    def append(self, name: str, test_y: list, pred_y: list) -> None:
        score: list = \
            [name,
             accuracy_score(test_y, pred_y),
             precision_score(test_y, pred_y, average='micro'),
             recall_score(test_y, pred_y, average='micro'),
             f1_score(test_y, pred_y, average='micro')] \
            if self.genre == "classification" else \
            [name,
             mean_absolute_error(test_y, pred_y),
             mean_squared_error(test_y, pred_y),
             np.sqrt(mean_squared_error(test_y, pred_y)),
             r2_score(test_y, pred_y)]
        self.table.append(score)

    def output(self):
        print()
        print(tabulate(
            self.table,
            headers=self.headers,
            tablefmt="grid"))
